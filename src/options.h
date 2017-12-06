#ifndef OPTIONS_H
#define OPTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <string>

using namespace std;


class Options{
public:
    Options();
    void init();
    bool validate();

public:
    // file name of read1 input
    string in1;
    // thread number = file number + 1
    int thread;
    // compression level
    int compression;

};

#endif