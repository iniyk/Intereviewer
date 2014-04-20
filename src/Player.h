/* 
 * File:   Player.h
 * Author: root
 *
 * Created on 2014年4月8日, 上午10:38
 */

#ifndef PLAYER_H
#define	PLAYER_H

#include <unistd.h>
#include <time.h>

#include "GlobalHelpers.h"
#include "Compiler.h"
#include "BoxRunner.h"

namespace Intereviewer{
    class Player {
    public:
        Player();
        Player(const Player& orig);
        virtual ~Player();
        
         void Setup(INI::Parser &_config);
         Status StartUp(const String &playername, const String &playground, 
                               const String &source_file, bool reviewer,
                               int _time_limit_all, int _memory_limit, int _time_limit_per);
    private:
        INI::Parser* config;
        Compiler compiler;
        BoxRunner boxrunner;
        
        bool reviewer;
        String play_ground, player_name;
        String lang_name, source_file;
        char buffer_rd[MAX_STR_LENGTH], buffer_wt[MAX_STR_LENGTH];
        int pipe_rd[2], pipe_wt[2];
        
        StrVector player_list;
        pid_t send_proc;
        
        String reviewer_name;
        pid_t read_proc;
        int time_limit_all, memory_limit, time_limit_per;
        
        Status compile();
        Status reviewer_code();
        Status normal_player_code();
        bool checkAlive();
    };
}

#endif	/* PLAYER_H */

