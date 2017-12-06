#include "options.h"
#include "util.h"

Options::Options(){
    in1 = "";
    compression = 2;
}

void Options::init() {
}

bool Options::validate() {
    if(in1.empty()) {
        error_exit("read1 input should be specified by --in1");
    } else {
        check_file_valid(in1);
    }

    return true;
}