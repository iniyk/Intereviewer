#include "boxrunner.h"

namespace Intereviwer{

void BoxRunner::init(Config *_config) {
    //args = (char **) malloc(MAX_ARG_NUMBER);
    memset(args, 0, sizeof(args));
    memory_limit = AUTO_MEMORY_LIMIT;
    time_limit = AUTO_TIME_LIMIT;
    config = _config;
//init result_name
    result_name.clear();
    result_name.push_back("PD");
    result_name.push_back("OK");
    result_name.push_back("RF");
    result_name.push_back("ML");
    result_name.push_back("OL");
    result_name.push_back("TL");
    result_name.push_back("RT");
    result_name.push_back("AT");
    result_name.push_back("IE");
    result_name.push_back("BP");
}

void BoxRunner::SetLanguage(const String &lang) {
    language = lang;
}

void BoxRunner::SetTimeLimit(int val) {
    time_limit = val;
}

void BoxRunner::SetMemoryLimit(int val) {
    memory_limit = val;
}

int BoxRunner::Run(const String &target_path, int fd_rd, int fd_wt, const String &lang) {
    strcpy(args[0], target_path.c_str());

    if (sandbox_init(&msb.sbox, (const char**)args) != 0) {
        return -1;
    }
    
    for (int i=0; i<MAX_INT64; ++i) {
        msb.sc_table[i] = _CONT;
    }
    
    if (config->selectSection(lang) == 0) {
#ifdef __x86__
        
#endif
    } else {
        
    }
}

int BoxRunner::Status() {
    int ret;

    pid_t tmp = waitpid(sand_pid, &ret, WNOHANG);

    if (tmp == 0) return 0;

    return WEXITSTATUS(ret);
}

int BoxRunner::Kill() {
    return (int)kill(0, SIGKILL);
}
}