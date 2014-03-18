/*
 * Class Name : BoxRunner
 * Func :
 *   Run user's program in the way which could be execute by sandbox.
 *   Supplied language : C, CPP, Java, Python
 */
#ifndef _JUDGER_COMPILER_
#define _JUDGER_COMPILER_

#include "GlobalHelpers.h"
#include "sandbox.h"
#include "config.h"

namespace Intereviwer{
    class BoxRunner {
    public:
        BoxRunner();
        /**
         * init this object
         */
        void init(Config *_config);
        /**
         * Setup language
         * @param lang language
         */
        void SetLanguage(const String &lang);
        /**
         * Setup Time limit
         * @param val time limit in mircosecond
         */
        void SetTimeLimit(int val);
        /**
         * Setup Memory limit
         * @param val memory limit in kb
         */
        void SetMemoryLimit(int val);
        /**
         * add a run-time arguement
         * @param arg run-time arg, such as -t
         */
        void addArg(const String &arg);
        /**
         * run target file
         * @param  target_path target file path
         * @param  fd_rd       read file describe code
         * @param  fd_wt       write file describe code
         * @return             pid>0 normal return\n
         *                     -1 sandbox error\n
         *                     -2 target file invalid
         */
        int Run(const String &target_path, int fd_rd, int fd_wt, const String &lang);
        /**
         * get last run result message
         * @return if Run() hasn't been used since last init() it will return ""\n
         *         else will get the running message : TLE, OLE, RF, MLE, RE
         */
        String Result();
        /**
         * get sandbox running status
         * @return 0 running\n
         *         1 has been exit
         */
        int Status();
        /**
         * kill sandbox process
         * @return 0 kill success
         */
        int Kill();
    private:
        //Sandbox Defination
        StrVector result_name;
        
        int memory_limit, time_limit;
        char* args[MAX_ARG_NUMBER];
        String result, language;

        int16_t ban_list[MAX_POLICY];
        Config* config;

        minisbox_t msb;
        pid_t sand_pid;

        void setup_policy();
    };
}

#endif
