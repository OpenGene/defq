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
    cmd.add<string>("in1", 'i', "input file name", true, "");
    cmd.add<string>("sample_sheet", 's', "a CSV file contains three columns (filename, index1, index2)", true, "");
    cmd.add<string>("out_folder", 'o', "output folder, default is current working directory", false, ".");
    cmd.add<string>("suffix1", 'f', "the suffix to be appended to the output file name given in sample sheet, default is none", false, "");
    cmd.add<string>("undetermined", 'u', "the file name of undetermined data, default is Undetermined", false, "Undetermined");
    cmd.add<int>("compression", 'z', "compression level for gzip output (1 ~ 9). 1 is fastest, 9 is smallest, default is 2.", false, 2);

    cmd.parse_check(argc, argv);
    
    stringstream ss;
    for(int i=0;i<argc;i++){
        ss << argv[i] << " ";
    }
    command = ss.str();

    time_t t1 = time(NULL);

    Options opt;

    opt.in1 = cmd.get<string>("in1");
    opt.samplesheet = cmd.get<string>("sample_sheet");
    opt.outFolder = cmd.get<string>("out_folder");
    opt.suffix1 = cmd.get<string>("suffix1");
    opt.umdeterminedFileName = cmd.get<string>("undetermined");
    opt.compression = cmd.get<int>("compression");

    opt.validate();
        
    Processor p(&opt);
    p.process();

    return 0;
}