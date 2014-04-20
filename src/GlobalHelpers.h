/*
 * File:   GlobalHelpers.h
 * Author: 51isoft
 *
 * Created on 2014年1月14日, 上午12:17
 */

#ifndef GLOBALHELPERS_H
#define	GLOBALHELPERS_H

#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include <map>
#include <string>
#include <vector>
#include <iterator>
#include <istream>
#include <sstream>
#include <fstream>

#include <unistd.h>
#include <sysexits.h>
#include <syscall.h>
#include <strings.h>
#include <errno.h>
#include <netdb.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/signal.h>
#include <sys/ptrace.h>
#include <sys/syscall.h>
#include <sys/user.h>
#include <sys/stat.h>

#include <netinet/in.h>

#include <arpa/inet.h>

#include <errno.h>
#include <netdb.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include "Logger.h"
//#include "SocketHandler.h"
#include "Exception.h"
#include "sandbox.h"

#ifdef __DEBUG__
#include <cstdio>
#endif

namespace Intereviewer{
#ifndef INT16_MAX
#define INT16_MAX (32767)
#endif /* INT16_MAX */
    const int ERROR_WHILE_LOOKING_FOR_USER = -7;
    const int ERROR_WHILE_REVIEWER_COMPILE = -6;
    const int ERROR_WHILE_CREATE_PIPE = -5;
    const int ERROR_WHILE_USING_SYSTEM_API = -4;
    const int ERROR_WHILE_INIT_DATA_STRUCT = -3;
    const int ERROR_WHILE_LOAD_FILE = -2;
    const int ERROR_WHILE_FORK_PROCESS = 1;
    const int SUCCESS_RETURN = 0;
    
    typedef int Status;
    
    typedef std::string String;
    typedef std::vector<String> StrVector;
    
    const int MAX_PLAYER = 50;
    const int MAX_FILE_PATH = 1024;
    const int MAX_STR_LENGTH = 1024;
    const int MAX_PROGRAM_OUTPUT = 4096;
    const int MAX_INT_LENGTH = 40;
    const int MAX_INT64 = 32767;
    const int MAX_ARG_NUMBER = 1024;
    const int MAX_POLICY = 500;
    const int AUTO_MEMORY_LIMIT = 1024*1024*32; //32MB
    const int AUTO_TIME_LIMIT = 3000;           //3sec
    const int HANDSHAKE_TIMEOUT = 5000;         //5sec

    const String config_file_path("/home/iniyk/IntereviewerStage/Judger.ini");

    void seperate(const String &str, StrVector &res, char sep = ' ');
    String int2string(int x);
    int limitstring2resource(const String &limit_str);
    bool alphaornumber(char x);
    unsigned long ts2ms(struct timespec ts);
    int16_t sc2idx(const syscall_t &scinfo);
    int16_t abi32(int scno);

    String loadAllFromFile(String);
    String Inttostring(int);
    int stringToInt(String);
    bool trans2bool(String);
    const String currentDateTime();
    const String currentDate();
    StrVector split(const String &, char, bool = true);
    String get_filename_main(const String &str);
    int decode_message(const char* str, String &reserve_word, String &message);
    int decode_message(const char* str, String &reserve_word, String &message,
                                        String &target_player, String &play_ground);
    String get_line_from_pipe(int fd);
    int get_line_from_pipe(int fd, char* buffer);
    bool pid_running(pid_t pid);
    void pid_killer(pid_t pid);
    
    void LOG(String msg);

    typedef Dispatcher::Logger DLogger;
    typedef Dispatcher::Exception DException;
    //typedef Dispatcher::SocketHandler DSocketHandler;

    //#define LOG log
    //#define LOGGER DLogger::Getinstance()

    typedef action_t* (*rule_t)(const sandbox_t*, const event_t*, action_t*);
    typedef struct {
       sandbox_t sbox;
       policy_t default_policy;
       rule_t sc_table[MAX_INT64 + 1];
    } minisbox_t;
    typedef enum {
        P_ELAPSED = 0, P_CPU = 1, P_MEMORY = 2,
    } probe_t;
    //Sandbox Function
    res_t probe(const sandbox_t* psbox, probe_t key);
    void policy(const policy_t*, const event_t*, action_t*);
    action_t* _KILL_RF(const sandbox_t*, const event_t*, action_t*);
    action_t* _CONT(const sandbox_t*, const event_t*, action_t*);
}

#endif	/* GLOBALHELPERS_H */

