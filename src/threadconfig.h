#ifndef THREAD_CONFIG_H
#define THREAD_CONFIG_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include "writer.h"
#include "options.h"
#include <atomic>

using namespace std;

class ThreadConfig{
public:
    ThreadConfig(Options* opt,  int threadId);
    ~ThreadConfig();

    void initWriter(string filename1);
    void initWriter(ofstream* stream);
    void initWriter(gzFile gzfile);

    int getThreadId() {return mThreadId;}
    void cleanup();

    bool isCompleted();
    void output();
    void input(char* data, size_t size);
    bool setInputCompleted();

private:
    void deleteWriter();

private:
    Writer* mWriter1;
    Options* mOptions;

    // for spliting output
    int mThreadId;
    bool mInputCompleted;
    atomic_long mInputCounter;
    atomic_long mOutputCounter;
    char** mRingBuffer;
    size_t* mRingBufferSizes;

};

#endif