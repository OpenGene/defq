#include "seprocessor.h"
#include "fastqreader.h"
#include <iostream>
#include <unistd.h>
#include <functional>
#include <thread>
#include <memory.h>
#include "util.h"

SingleEndProcessor::SingleEndProcessor(Options* opt){
    mOptions = opt;
    mProduceFinished = false;
    mDemuxer = new Demuxer(opt);
    mSampleSize = mOptions->samples.size();
}

SingleEndProcessor::~SingleEndProcessor() {
    delete mDemuxer;
}

bool SingleEndProcessor::process(){

    initPackRepository();
    std::thread producer(std::bind(&SingleEndProcessor::producerTask, this));

    int threadnum = mSampleSize + 1;

    mConfigs = new ThreadConfig*[threadnum];
    for(int t=0; t<threadnum; t++){
        mConfigs[t] = new ThreadConfig(mOptions, t);
    }

    std::thread** threads = new thread*[threadnum];
    for(int t=0; t<threadnum; t++){
        threads[t] = new std::thread(std::bind(&SingleEndProcessor::writeTask, this, mConfigs[t]));
    }

    std::thread demuxer(std::bind(&SingleEndProcessor::consumerTask, this));

    producer.join();
    demuxer.join();
    for(int t=0; t<threadnum; t++){
        threads[t]->join();
    }

    // clean up
    for(int t=0; t<threadnum; t++){
        delete threads[t];
        threads[t] = NULL;
        delete mConfigs[t];
        mConfigs[t] = NULL;
    }

    delete threads;
    delete mConfigs;

    return true;
}

bool SingleEndProcessor::processSingleEnd(ReadPack* pack){
    string* outputs = new string[mSampleSize + 1];
    for(int p=0;p<pack->count;p++){
        Read* r = pack->data[p];
        int sample = mDemuxer->demux(r);
        // Undetermined
        if(sample < 0) {
            sample = mSampleSize;
        }
        outputs[sample] += r->toString();
        delete r;
    }

    for(int i=0;i< mSampleSize+1; i++) {
        if(outputs[i].size() == 0)
            continue;
        char* data = new char[outputs[i].size()];
        memcpy(data, outputs[i].c_str(), outputs[i].size());
        mConfigs[i]->input(data, outputs[i].size());
    }

    delete pack->data;
    delete pack;
    delete[] outputs;

    return true;
}

void SingleEndProcessor::initPackRepository() {
    mRepo.packBuffer = new ReadPack*[PACK_NUM_LIMIT];
    memset(mRepo.packBuffer, 0, sizeof(ReadPack*)*PACK_NUM_LIMIT);
    mRepo.writePos = 0;
    mRepo.readPos = 0;
    mRepo.readCounter = 0;
    
}

void SingleEndProcessor::destroyPackRepository() {
    delete mRepo.packBuffer;
    mRepo.packBuffer = NULL;
}

void SingleEndProcessor::producePack(ReadPack* pack){
    std::unique_lock<std::mutex> lock(mRepo.mtx);
    while(((mRepo.writePos + 1) % PACK_NUM_LIMIT)
        == mRepo.readPos) {
        mRepo.repoNotFull.wait(lock);
    }

    mRepo.packBuffer[mRepo.writePos] = pack;
    mRepo.writePos++;

    if (mRepo.writePos == PACK_NUM_LIMIT)
        mRepo.writePos = 0;

    mRepo.repoNotEmpty.notify_all();
    lock.unlock();
}

void SingleEndProcessor::consumePack(){
    ReadPack* data;
    std::unique_lock<std::mutex> lock(mRepo.mtx);
    // read buffer is empty, just wait here.
    while(mRepo.writePos == mRepo.readPos) {
        if(mProduceFinished){
            lock.unlock();
            return;
        }
        mRepo.repoNotEmpty.wait(lock);
    }

    data = mRepo.packBuffer[mRepo.readPos];
    (mRepo.readPos)++;
    lock.unlock();

    processSingleEnd(data);


    if (mRepo.readPos >= PACK_NUM_LIMIT)
        mRepo.readPos = 0;

    mRepo.repoNotFull.notify_all();
}

void SingleEndProcessor::producerTask()
{
    int slept = 0;
    long readNum = 0;
    bool splitSizeReEvaluated = false;
    Read** data = new Read*[PACK_SIZE];
    memset(data, 0, sizeof(Read*)*PACK_SIZE);
    FastqReader reader(mOptions->in1, true);
    int count=0;
    while(true){
        Read* read = reader.read();
        if(!read){
            // the last pack
            ReadPack* pack = new ReadPack;
            pack->data = data;
            pack->count = count;
            producePack(pack);
            data = NULL;
            break;
        }
        data[count] = read;
        count++;
        // a full pack
        if(count == PACK_SIZE){
            ReadPack* pack = new ReadPack;
            pack->data = data;
            pack->count = count;
            producePack(pack);
            //re-initialize data for next pack
            data = new Read*[PACK_SIZE];
            memset(data, 0, sizeof(Read*)*PACK_SIZE);
            // reset count to 0
            count = 0;
            // if the consumer is far behind this producer, sleep and wait to limit memory usage
            while(mRepo.writePos - mRepo.readPos > PACK_IN_MEM_LIMIT){
                //cout<<"sleep"<<endl;
                slept++;
                usleep(100);
            }
            readNum += PACK_SIZE;
        }
    }

    std::unique_lock<std::mutex> lock(mRepo.readCounterMtx);
    mProduceFinished = true;
    lock.unlock();

    // if the last data initialized is not used, free it
    if(data != NULL)
        delete data;
}

void SingleEndProcessor::consumerTask()
{
    while(true) {
        std::unique_lock<std::mutex> lock(mRepo.readCounterMtx);
        if(mProduceFinished && mRepo.writePos == mRepo.readPos){
            lock.unlock();
            // notify all writer threads
            for(int i=0; i<mSampleSize+1; i++) {
                mConfigs[i]->setInputCompleted();
            }
            break;
        }
        if(mProduceFinished){
            consumePack();
            lock.unlock();
        } else {
            lock.unlock();
            consumePack();
        }
    }
}

void SingleEndProcessor::writeTask(ThreadConfig* config)
{
    while(true) {
        if(config->isCompleted()){
            break;
        }
        config->output();
    }
}