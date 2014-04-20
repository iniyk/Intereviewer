/* 
 * File:   Judger.h
 * Author: root
 *
 * Created on 2014年4月20日, 上午11:23
 */

#ifndef JUDGER_H
#define	JUDGER_H

#include "ini.hpp"
#include "GlobalHelpers.h"

namespace Intereviewer{
    class Judger {
    public:
        Judger();
        Judger(const Judger& orig);
        virtual ~Judger();

        void Setup(INI::Parser &_config);
        Status StartUp(const StrVector &_player_list, const StrVector &_lang_list,
                        const String &_reviewer, const String &_lang_reviewer, const String _play_ground,
                        int _time_limit_all, int _memory_limit, int _time_limit_per);
    private:
        INI::Parser* config;
        StrVector player_list, lang_list;
        String reviewer, lang_reviewer;
        String play_ground;
        int time_limit_all, memory_limit, time_limit_per;
        int reviewer_time_limit, reviewer_memory_limit;
        
        struct Pipe{
            int fd_rd[2], fd_wt[2];
        };
        
        Pipe player_pipes[MAX_PLAYER];
        pid_t player_pids[MAX_PLAYER];
        pid_t reviewer_pid;
        Pipe reviewer_pipe;
        
        String rank[MAX_PLAYER];
        String result[MAX_PLAYER];
        int has_returned;
        
        Status start_up_player(int id);
        Status start_up_reviewer();
        Status do_player_register();
        Status send_message(const String &player, const String &reserve_word, const String &message);
        Status send_message(int fd_wt, const String &reserve_word, const String &message);
        int get_player_id(const String &player);
        Status end_of_a_game();
        Status do_request_player_line(const String &player);
        Status send_message_to_reviewer(const String &reserve_word, const String &message);
        
        
    };
}

#endif	/* JUDGER_H */

