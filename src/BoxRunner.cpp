/* 
 * File:   BoxRunner.cpp
 * Author: iniyk
 * 
 * Created on 2014年3月31日, 下午1:04
 */

#include "BoxRunner.h"

namespace Intereviewer{

    BoxRunner::BoxRunner() {
    }

    BoxRunner::BoxRunner(const BoxRunner& orig) {
    }

    BoxRunner::~BoxRunner() {
    }

    void BoxRunner::Setup(INI::Parser &_config) {
        running_status = 0;
        config = &_config;
        result_name = split(config->top()("Player")("BoxRunner")["result_code"], ' ', true);
    }
    
    Status BoxRunner::StartUp(const String &_target,
                                        int fd_in[], int fd_out[],
                                        const String &lang,
                                        int time_limit_all, int memory_limit) {
        running_status = 0;
        language = lang;
        target = _target;
        String command = config->top()("LanguageRegister")(lang)["run_cmd"];
        int index;
        while ((index = command.find("%target")) > 0) {
            command = command.replace(index, 7, target);
        }
        arg_vector = split(command, ' ', true);
        
        //argv = (char**)malloc((int)arg_vector.size() * sizeof(char*));
        for (int i=0; i<MAX_ARG_NUMBER; ++i) {
            argv[i] = NULL;
        }
        
        for (int i=0; i<(int)arg_vector.size(); ++i) {
            argv[i] = (char*)malloc((arg_vector[i].length() + 1) * sizeof(char));
            strcpy(argv[i], arg_vector[i].c_str());
        }
        if (sandbox_init(&msb.sbox, (const char**)argv) != 0) {
            return ERROR_WHILE_INIT_DATA_STRUCT;
        }
        
        setup_policy();
        
        int fd_error = open("error.txt", O_WRONLY | O_CREAT);
        
        int wall_clock_rate = stringToInt(config->top()("Player")("BoxRunner")["wall_clock_rate"]);
        if (time_limit_all == 0) time_limit_all = stringToInt(config->top()("Player")("BoxRunner")["auto_time_limit_all"]);
        if (memory_limit == 0) memory_limit = stringToInt(config->top()("Player")("BoxRunner")["auto_memory_limit"]);
        
//#ifdef __DEBUG__
//        printf("CMD : %s\n", command.c_str());
//        printf("TIME_LIMIT : %d\n", time_limit_all);
//        printf("WALL_CLOCK : %d\n", wall_clock_rate * time_limit_all);
//        printf("MEMORY_LIMIT : %d\n", memory_limit);
//        printf("RES : ");
//        for (int i=0; i<(int)result_name.size(); ++i) {
//            printf("%s ", result_name[i].c_str());
//        }
//        printf("\n");
//#endif
        
        msb.sbox.task.ifd = fd_in[0];  
        msb.sbox.task.ofd = fd_out[1]; 
        msb.sbox.task.efd = fd_error; 
        msb.sbox.task.quota[S_QUOTA_WALLCLOCK] = time_limit_all * wall_clock_rate; 
        msb.sbox.task.quota[S_QUOTA_CPU] = time_limit_all;       
        msb.sbox.task.quota[S_QUOTA_MEMORY] = memory_limit;  
        msb.sbox.task.quota[S_QUOTA_DISK] = memory_limit;  
        
        if (!sandbox_check(&msb.sbox)) {
            return ERROR_WHILE_INIT_DATA_STRUCT;
        }
        
        box_pid = fork();
        if (box_pid == -1) {
            return ERROR_WHILE_FORK_PROCESS;
        } else if (box_pid == 0) {
            close(fd_in[1]);
            close(fd_out[0]);
            usleep(50000);
            running_status = 1;
            result_t res = *sandbox_execute(&msb.sbox);
            
            FILE* result_file = fopen("result.txt", "w");
            
            fprintf(result_file, "%s\n", result_name[res].c_str());
//            fprintf(result_file, "cpu: %ldms\n", probe(&msb.sbox, P_CPU));
//            fprintf(result_file, "mem: %ldkB\n", probe(&msb.sbox, P_MEMORY));
            
            sandbox_fini(&msb.sbox);
            fclose(result_file);
            close(fd_in[0]);
            close(fd_out[1]);
            close(fd_error);
            exit(0);
        } else {
            close(fd_in[0]);
            close(fd_out[1]);
            close(fd_error);
            
//#ifdef __DEBUG__
//            printf("PID : %d\n", box_pid);
//#endif
            
            return SUCCESS_RETURN;
        }
    }
    
    Status BoxRunner::EndUp() {
        int ret = kill(box_pid, SIGTERM);
        usleep(1000000);
        ret = ret & (kill(box_pid, SIGKILL));
        if (ret == 0) return SUCCESS_RETURN;
        else return ERROR_WHILE_USING_SYSTEM_API;
    }
    
    bool BoxRunner::GetStatus() {
//        int status;
//        pid_t ret;
//        ret = waitpid(box_pid, &status, WNOHANG);
//        if (ret == 0) {
//            return running_status;
//        }  else {
//            running_status = 0;
//            return running_status;
//        }
        String cmd = String("pstree -p ") + Inttostring(box_pid) + " | grep " + target;
        FILE* status = popen(cmd.c_str(), "r");
        char opt[MAX_STR_LENGTH];
        usleep(100000);
        if (fscanf(status, "%s", opt) == EOF) {
            //printf("Null File\n");
            pclose(status);
            return false;
        } else {
            //printf("%s\n", opt);
            pclose(status);
            return true;
        }
    }
    
    void BoxRunner::setup_policy() {
        StrVector ban_list_temp;
        String ban_list_config_temp;
#ifdef __x86_64__
        ban_list_config_temp = config->top()("LanguageRegister")(language)["syscall_ban_list_x86_64"];
#else
        ban_list_config_temp = config->top()("LanguageRegister")(language)["syscall_ban_list_i386"];
#endif
        ban_list_temp = split(ban_list_config_temp, ' ', true);
        ban_list.clear();
        
        for (int i=0; i<=INT16_MAX; ++i) {
            msb.sc_table[i] = _CONT;
        }

        for (int i=0; i<(int)ban_list_temp.size(); ++i) {
            int16_t now = abi32(stringToInt(ban_list_temp[i]));
            msb.sc_table[now] = _KILL_RF;
            ban_list.push_back(now);
        }
        msb.default_policy = msb.sbox.ctrl.policy;
        msb.default_policy.entry = (msb.default_policy.entry) ?: \
            (void*)sandbox_default_policy;
        msb.sbox.ctrl.policy = (policy_t){(void*)policy, (long)&msb};
    }
    
    pid_t BoxRunner::GetPid() {
        return box_pid;
    }
}
