#include "options.h"
#include "util.h"
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <string.h>

Options::Options(){
    in1 = "";
    compression = 2;
    umdeterminedFileName = "Undetermined";
}

void Options::parseSampleSheet() {
    if(samplesheet.empty()) {
        error_exit("sample sheet CSV file should be specified by -s or --sample_sheet");
    } else {
        check_file_valid(samplesheet);
    }

    ifstream file;
    file.open(samplesheet.c_str(), ifstream::in);
    const int maxLine = 1000;
    char line[maxLine];
    while(file.getline(line, maxLine)){
        // trim \n, \r or \r\n in the tail
        int readed = strlen(line);
        if(readed >=2 ){
            if(line[readed-1] == '\n' || line[readed-1] == '\r'){
                line[readed-1] = '\0';
                if(line[readed-2] == '\r')
                    line[readed-2] = '\0';
            }
        }
        string linestr(line);

        // comment or header line
        if(line[0] == '#')
            continue;

        vector<string> splitted;
        split(linestr, splitted, ",");
        // a valid line need 4 columns: name, left, center, right
        if(splitted.size()<2)
            continue;

        Sample s;
        s.file = trim(splitted[0]);
        s.index1 = trim(splitted[1]);
        if(splitted.size()>=3)
            s.index2 = trim(splitted[2]);

        this->samples.push_back(s);
    }

}

bool Options::validate() {
    if(in1.empty()) {
        error_exit("read1 input should be specified by --in1");
    } else {
        check_file_valid(in1);
    }

    if(!file_exists(outFolder)) {
        mkdir(outFolder.c_str(), 0777);
    }

    if(file_exists(outFolder) && !is_directory(outFolder)) {
        error_exit(outFolder + " is a file, not a directory");
    }

    if(!file_exists(outFolder) || !is_directory(outFolder)) {
        error_exit(outFolder + " is not a directory, or cannot be created");
    }

    parseSampleSheet();

    if(samples.size() == 0)
        error_exit("no sample found, did you provide a valid sample sheet CSV file by -s or --sample_sheet?");

    return true;
}
