/* 
 * File:   Judger.cpp
 * Author: root
 * 
 * Created on 2014年4月20日, 上午11:23
 */

#include "Judger.h"

namespace Intereviewer{
    Judger::Judger() {
    }

    Judger::Judger(const Judger& orig) {
    }

    Judger::~Judger() {
    }
    
    void Judger::Setup(INI::Parser &_config) {
        config = &_config;
        reviewer_time_limit = stringToInt(config->top()("Judger")["reviewer_time_limit"]);
        reviewer_memory_limit = stringToInt(config->top()("Judger")["reviewer_memory_limit"]);
        player_list.clear();
        lang_list.clear();
        has_returned = 0;
    }
    
    Status Judger::do_request_player_line(const String &player) {
        char buffer_rd[MAX_STR_LENGTH];
        int id = get_player_id(player);
        String reserve_word, message, player_rd, playground;
        
        send_message(player, "request_one_line", "");
        
        get_line_from_pipe(player_pipes[id].fd_rd[0], buffer_rd);
        decode_message(buffer_rd, reserve_word, message, player_rd, playground);
        if (player.compare("") == 0) {
            if (reserve_word.compare("result_return")) {
                result[id] = message;
                send_message(player, "end_of_a_game", "");
                send_message_to_reviewer("quit_game", "");
            }
        } else {
            send_message_to_reviewer(reserve_word, message);
        }
        
        return SUCCESS_RETURN;
    }
    
    Status Judger::StartUp(const StrVector &_player_list, const StrVector &_lang_list,
                        const String &_reviewer, const String &_lang_reviewer, const String _play_ground,
                        int _time_limit_all, int _memory_limit, int _time_limit_per) {
        char buffer_rd[MAX_STR_LENGTH];
        
        for (int i=0; i<(int)_player_list.size(); ++i) {
            player_list.push_back(_player_list[i]);
        }
        for (int i=0; i<(int)_lang_list.size(); ++i) {
            lang_list.push_back(_lang_list[i]);
        }
        reviewer = _reviewer;
        lang_reviewer = _lang_reviewer;
        play_ground = _play_ground;
        time_limit_all = _time_limit_all;
        memory_limit = _memory_limit;
        time_limit_per = _time_limit_per;
        
        start_up_reviewer();
        for (int i=0; i<(int)player_list.size(); ++i) {
            result[i] = "OK";
            start_up_player(i);
        }
        
        usleep(500000);
        
        do_player_register();
        
        while (true) {
            get_line_from_pipe(reviewer_pipe.fd_rd[0], buffer_rd);
            String player, playground;
            String reserve_word, message;
            decode_message(buffer_rd, reserve_word, message, player, playground);
            if (player.compare("") == 0) {
                if (reserve_word.compare("result_return")) {
                    int rk;
                    sscanf(message.c_str(), "%d %s", &rk, buffer_rd);
                    rank[rk] = String(buffer_rd);
                } else if (reserve_word.compare("end_of_a_game")) {
                    end_of_a_game();
                }
            } else {
                if (reserve_word.compare("request_one_line") == 0) {
                    do_request_player_line(player);
                } else {
                    send_message(player, reserve_word, message);
                }
            }
        }
        
        return SUCCESS_RETURN;
    }
    
    Status Judger::start_up_player(int id) {
        String command = config->top()("Judger")["player_exec_path"];
        
#ifdef __DEBUG__
        printf("COMMAND : %s\n", command.c_str());
#endif
        
        int status = pipe(player_pipes[id].fd_rd);
        if (status == -1) return ERROR_WHILE_CREATE_PIPE;
        status = pipe(player_pipes[id].fd_wt);
        if (status == -1) return ERROR_WHILE_CREATE_PIPE;
        
        pid_t pid_now = fork();
        if (pid_now == -1) {
            return ERROR_WHILE_FORK_PROCESS;
        } else if (pid_now == 0) {
            dup2(player_pipes[id].fd_wt[0], STDIN_FILENO);
            dup2(player_pipes[id].fd_rd[1], STDOUT_FILENO);
            String source_file_name = config->top()("Judger")["auto_player_source_file_name"];
            
            bool flag = trans2bool(config->top()("Judger")["using_player_name_as_source_file_name"]);
            if (flag) {
                source_file_name = player_list[id];
            }
            
            source_file_name += String(".") + config->top()("LanguageRegister")(lang_reviewer)["lang_ext"];
            execle(command.c_str(), source_file_name.c_str(), player_list[id].c_str(), play_ground.c_str(),
                    "false", Inttostring(time_limit_all).c_str(), Inttostring(memory_limit).c_str(),
                    Inttostring(time_limit_per).c_str(), NULL);
            exit(0);
        } else {
            player_pids[id] = pid_now;
            return SUCCESS_RETURN;
        }
        
        return SUCCESS_RETURN;
    }
    
    Status Judger::start_up_reviewer() {
        String command = config->top()("Judger")["player_exec_path"];
        
#ifdef __DEBUG__
        printf("COMMAND : %s\n", command.c_str());
#endif
        
        int status = pipe(reviewer_pipe.fd_rd);
        if (status == -1) return ERROR_WHILE_CREATE_PIPE;
        status = pipe(reviewer_pipe.fd_wt);
        if (status == -1) return ERROR_WHILE_CREATE_PIPE;
        
        pid_t pid_now = fork();
        if (pid_now == -1) {
            return ERROR_WHILE_FORK_PROCESS;
        } else if (pid_now == 0) {
            dup2(reviewer_pipe.fd_wt[0], STDIN_FILENO);
            dup2(reviewer_pipe.fd_rd[1], STDOUT_FILENO);
            String source_file_name = config->top()("Judger")["auto_reviewer_source_file_name"];
            source_file_name += String(".") + config->top()("LanguageRegister")(lang_reviewer)["lang_ext"];
            execle(command.c_str(), source_file_name.c_str(), reviewer.c_str(), play_ground.c_str(),
                    "true", Inttostring(reviewer_time_limit).c_str(), Inttostring(reviewer_memory_limit).c_str(),
                    Inttostring(reviewer_time_limit).c_str(), NULL);
            exit(0);
        } else {
            reviewer_pid = pid_now;
            return SUCCESS_RETURN;
        }
        
        return SUCCESS_RETURN;
    }
    
    Status Judger::do_player_register() {
        String players = "";
        for (StrVector::iterator iter=player_list.begin(); iter!=player_list.end(); ++iter) {
            if (iter != player_list.begin()) players += String(" ") + (*iter);
            else players += (*iter);
        }
        send_message(reviewer_pipe.fd_wt[1], "player_register", players);
        return SUCCESS_RETURN;
    }
    
    int Judger::get_player_id(const String &player) {
        for (StrVector::iterator iter=player_list.begin(); iter!=player_list.end(); ++iter) {
            if (iter->compare(player) == 0) {
                return (int)(iter - player_list.begin());
            }
        }
        return -1;
    }
    
    Status Judger::send_message(const String &player, const String &reserve_word, const String &message) {
        int id = get_player_id(player);
        if (id == -1) return ERROR_WHILE_LOOKING_FOR_USER;
        int fd_wt = player_pipes[id].fd_wt[1];
        return send_message(fd_wt, reserve_word, message);
    }
    
    Status Judger::send_message_to_reviewer(const String &reserve_word, const String &message) {
        return send_message(reviewer_pipe.fd_wt[1], reserve_word, message);
    }
    
    Status Judger::send_message(int fd_wt, const String &reserve_word, const String &message) {
        char buffer_wt[MAX_STR_LENGTH];
        
        sprintf(buffer_wt, "[%s]%s\n", reserve_word.c_str(), message.c_str());
        write(fd_wt, buffer_wt, strlen(buffer_wt) * sizeof(char));
        return SUCCESS_RETURN;
    }
    
    Status Judger::end_of_a_game() {
        for (int i=0; i<(int)player_list.size(); ++i) {
            if (result[i].compare("OK") == 0) {
                send_message(player_list[i], "end_of_a_game", "");
            }
            usleep(50000);
            pid_killer(player_pids[i]);
        }
        send_message_to_reviewer("end_of_a_game", "");
        usleep(50000);
        pid_killer(reviewer_pid);
    }
}
