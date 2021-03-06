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

#include <map>
#include <string>
#include <vector>
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

#include <netinet/in.h>
#include <arpa/inet.h>

#include "Exception.h"

#include "sandbox.h"

namespace Intereviwer{
    typedef std::string String;
    typedef std::vector<String> StrVector;

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

    const String config_file_path("/etc/Judger/Judger.conf");

    void seperate(const String &str, StrVector &res, char sep = ' ');
    String int2string(int x);
    int limitstring2resource(const String &limit_str);
    inline bool alphaornumber(char x);
    unsigned long ts2ms(struct timespec ts);
    int16_t sc2idx(syscall_t scinfo);
    int16_t abi32(int scno);

    String loadAllFromFile(String);
    String intToString(int);
    int stringToInt(String);
    const String currentDateTime();
    const String currentDate();
    StrVector split(const String &, char, bool = true);

    //typedef Dispatcher::Logger DLogger;
    //typedef Dispatcher::Exception DException;
    //typedef Dispatcher::SocketHandler DSocketHandler;

    #define LOG DLogger::Getinstance()->log
    #define LOGGER DLogger::Getinstance()
}

#endif	/* GLOBALHELPERS_H */

