#include <stdio.h>
#include "fastqreader.h"
#include "unittest.h"
#include <time.h>
#include "cmdline.h"
#include <sstream>
#include "util.h"
#include "options.h"
#include "processor.h"

string command;

int main(int argc, char* argv[]){
    // display version info if no argument is given
    if(argc == 1) {
        cout << "defq: multi-threaded FASTQ demultiplexing" << endl << "version " << VERSION_NUM << endl;
    }
    if (argc == 2 && strcmp(argv[1], "test")==0){
        UnitTest tester;
        tester.run();
        return 0;
    }
    cmdline::parser cmd;
    // input/output
    cmd.add<string>("in1", 'i', "read1 input file name", true, "");


    stringstream ss;
    for(int i=0;i<argc;i++){
        ss << argv[i] << " ";
    }
    command = ss.str();

    time_t t1 = time(NULL);

    Options opt;

    opt.in1 = cmd.get<string>("in1");
        
    Processor p(&opt);
    p.process();

    return 0;
}