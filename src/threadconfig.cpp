#include "threadconfig.h"
#include "util.h"

ThreadConfig::ThreadConfig(Options* opt, int threadId){
    mOptions = opt;
    mThreadId = threadId;

    mWriter1 = NULL;

    mInputCounter = 0;
    mOutputCounter = 0;
    mInputCompleted = false;

    mRingBuffer = new char*[PACK_NUM_LIMIT];
    memset(mRingBuffer, 0, sizeof(char*) * PACK_NUM_LIMIT);
    mRingBufferSizes = new size_t[PACK_NUM_LIMIT];
    memset(mRingBufferSizes, 0, sizeof(size_t) * PACK_NUM_LIMIT);

    string filename = mOptions->umdeterminedFileName;
    if(mThreadId < mOptions->samples.size())
        filename = mOptions->samples[mThreadId].file;

    filename += mOptions->suffix1;

    string fullpath = joinpath(mOptions->outFolder, filename);
    initWriter(fullpath);
}

ThreadConfig::~ThreadConfig() {
    cleanup();
    delete mRingBuffer;
}

bool ThreadConfig::isCompleted() 
{
    return mInputCompleted && (mOutputCounter == mInputCounter);
}

bool ThreadConfig::setInputCompleted() {
    mInputCompleted = true;
}

void ThreadConfig::output(){
    while( mOutputCounter < mInputCounter) 
    {
        long target = mOutputCounter % PACK_NUM_LIMIT;
        mWriter1->write(mRingBuffer[target], mRingBufferSizes[target]);
        delete mRingBuffer[target];
        mRingBuffer[target] = NULL;
        mOutputCounter++;
    }
}

void  ThreadConfig::input(char* data, size_t size){
    long target = mInputCounter % PACK_NUM_LIMIT;
    mRingBuffer[target] = data;
    mRingBufferSizes[target] = size;
    mInputCounter++;
}

void ThreadConfig::cleanup() {
    deleteWriter();
}

void ThreadConfig::deleteWriter() {
    if(mWriter1 != NULL) {
        delete mWriter1;
        mWriter1 = NULL;
    }
}

void ThreadConfig::initWriter(string filename1) {
    deleteWriter();
    mWriter1 = new Writer(filename1, mOptions->compression);
}

void ThreadConfig::initWriter(ofstream* stream) {
    deleteWriter();
    mWriter1 = new Writer(stream);
}

void ThreadConfig::initWriter(gzFile gzfile) {
    deleteWriter();
    mWriter1 = new Writer(gzfile);
}