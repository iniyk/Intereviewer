/* 
 * File:   BoxRunner.h
 * Author: iniyk
 *
 * Created on 2014年3月31日, 下午1:04
 */

#ifndef BOXRUNNER_H
#define	BOXRUNNER_H

#include "GlobalHelpers.h"
#include "sandbox.h"
#include "ini.hpp"

namespace Intereviewer {

    class BoxRunner {
    public:
        BoxRunner();
        BoxRunner(const BoxRunner& orig);
        virtual ~BoxRunner();

        void Setup(INI::Parser &_config);
        Status StartUp(const String &target,
                                        int fd_in[], int fd_out[],
                                        const String &lang,
                                        int time_limit_all = 0, int memory_limit = 0);
        Status EndUp();
        int GetStatus();
        pid_t GetPid();
    private:
        StrVector result_name;
        String language;
        std::vector<int16_t> ban_list;
        INI::Parser* config;
        pid_t box_pid;
        minisbox_t msb;
        StrVector arg_vector;
        char* argv[MAX_ARG_NUMBER];
        int running_status;

        void setup_policy();
    };
}

#endif	/* BOXRUNNER_H */

