#ifndef OPTIONS_H
#define OPTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

using namespace std;

class Sample{
public:
    string index1;
    string index2;
    string file;
};

class Options{
public:
    Options();
    bool validate();

public:
    // file name of read1 input
    string in1;
    // compression level
    int compression;
    // sample sheet CSV file
    string samplesheet;
    // output folder
    string outFolder;
    // undetermined
    string umdeterminedFileName;
    // suffix of filename
    string suffix1;
    // sample sheet
    vector<Sample> samples;

private:
    void parseSampleSheet();

};

#endif