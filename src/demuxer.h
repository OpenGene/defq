#ifndef DEMUXER_H
#define DEMUXER_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "options.h"
#include "read.h"
#include <map>

using namespace std;


class Demuxer{
public:
    Demuxer(Options* opt);
    ~Demuxer();
    int demux(Read* r);
    static bool test();


private:
    void init();
    inline int kmer2int(string& str);
    inline int base2val(char base);
private:
    Options* mOptions;
    int* mBuf;
    int mBufLen;
    bool mUseIndex1;
    bool mUseIndex2;
    bool mHasN;
    bool mFastMode;
    int mLongestIndex;
    int mShortestIndex;
    map<string, int> mIndexSample;

};

#endif