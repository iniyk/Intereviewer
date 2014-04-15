/* 
 * File:   Player.h
 * Author: root
 *
 * Created on 2014年4月8日, 上午10:38
 */

#ifndef PLAYER_H
#define	PLAYER_H

#include <unistd.h>

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
                               const String &work_folder, const String &source_file, bool reviewer,
                               int time_limit_all, int memory_limit, int time_limit_per);
    private:
        bool reviewer;
        String play_ground, player_name;
        String lang_name;
        StrVector player_list;
        String reviewer_name;
        INI::Parser* config;
        pid_t send_proc;
        char buffer_rd[MAX_STR_LENGTH], buffer_wt[MAX_STR_LENGTH];
        
        Status reply_request_line(const String &asker);
        Status shut_down(const String &asker, const String &last_word);
        
        Status request_line(const String &target);
        Status end_game(const String &judger);
    };
}

#endif	/* PLAYER_H */

