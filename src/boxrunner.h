/*
 * Class Name : BoxRunner
 * Func : 
 *   Run user's program in the way which could be execute by sandbox.
 *   Supplied language : C, CPP, Java, Python
 */
 
#ifndef _JUDGER_COMPILER_
#define _JUDGER_COMPILER_

#include "common.h"
#include "sandbox.h"

namespace Judger{
    class BoxRunner {
    public:
        BoxRunner();
        /**
         * init this object
         */
        void init(LogFunc lf, const Config &config);
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
        int Run(const String &target_path, int fd_rd, int fd_wt);
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
        int status();
        /**
         * kill sandbox process
         * @return 0 kill success
         */
        int kill();
    private:
        //Sandbox Defination
        const String result_name[] = {"PD", "OK", "RF", "ML", "OL", "TL", "RT", "AT", "IE", "BP", NULL};
        typedef action_t* (*rule_t)(const sandbox_t*, const event_t*, action_t*);
        typedef struct {
           sandbox_t sbox;
           policy_t default_policy;
           rule_t sc_table[INT16_MAX + 1];
        } minisbox_t;
        typedef enum {
            P_ELAPSED = 0, P_CPU = 1, P_MEMORY = 2,
        } probe_t;
        //Sandbox Function
        res_t probe(const sandbox_t* psbox, probe_t key);
        void policy(const policy_t*, const event_t*, action_t*);
        action_t* _KILL_RF(const sandbox_t*, const event_t*, action_t*);
        action_t* _CONT(const sandbox_t*, const event_t*, action_t*);

        int memory_limit, time_limit;
        char args[MAX_ARG_NUMBER][MAX_STR_LENGTH];
        String result, language;
        
        int16_t ban_list[MAX_POLICY];
        
        minisbox_t msb;
        pid_t sand_pid;
        
        LogFunc LOG;
        
        void setup_policy();
    };
}