/* 
 * File:   Player.cpp
 * Author: root
 * 
 * Created on 2014年4月8日, 上午10:38
 */

#include "Player.h"
#include "ini.hpp"

namespace Intereviewer{
    Player::Player() {
    }

    Player::Player(const Player& orig) {
    }

    Player::~Player() {
    }
    
    void Player::Setup(INI::Parser &_config) {
        config = &_config;
    }
    
    Status Player::compile() {
        compiler.Setup(*config);
        //printf("Source File : %s\n", source_file.c_str());
        //printf("Working Folder : %s\n", getcwd(NULL, NULL));
        Status ret = compiler.Compile(source_file);
        
        lang_name = compiler.get_lang_name(source_file);
        return ret;
    }
    
    bool Player::checkAlive() {
        return boxrunner.GetStatus();
    }
    
    Status Player::reviewer_code() {
        String reserve_word, message;
        Status status;
        gets(buffer_rd);

        decode_message(buffer_rd, reserve_word, message);
        assert(reserve_word.compare("players_register") == 0);
        player_list = split(message, ' ', true);

        for (int i=0; i<(int)player_list.size(); ++i) {
            printf("%s @ %s : [reviewer_register]%s\n", player_list[i].c_str(), play_ground.c_str(),
                    player_name.c_str());
        }

        send_proc = fork();
        if (send_proc == -1) {
            printf("@ %s : [judge_error_occurs] child process create error\n", play_ground.c_str());
            return ERROR_WHILE_FORK_PROCESS;
        } else if (send_proc == 0) {
            close(pipe_wt[1]);
            while (true) {
                //int sz = (int)read(pipe_rd[0], buffer_rd, sizeof(buffer_rd));
                int sz = get_line_from_pipe(pipe_rd[0], buffer_rd);
                buffer_rd[sz] = 0;
                printf("%s", buffer_rd);
            }
        } else {
            close(pipe_rd[0]);
            while (true) {
                gets(buffer_rd);
                decode_message(buffer_rd, reserve_word, message);
                if (reserve_word.compare("end_of_a_game") == 0) {
                    boxrunner.EndUp();
                    kill(send_proc, SIGTERM);
                    sleep(1000000);
                    kill(send_proc, SIGKILL);
                    return SUCCESS_RETURN;
                }
                int sz = strlen(buffer_rd);
                buffer_rd[sz++] = '\n';
                buffer_rd[sz] = 0;
                status = (int)write(pipe_wt[1], buffer_rd, sz);
            }
        }
        return SUCCESS_RETURN;
        //end of reviewer
    }
    
    Status Player::normal_player_code() {
        String reserve_word, message;
        gets(buffer_rd);
        decode_message(buffer_rd, reserve_word, message);
        assert(reserve_word.compare("reviewer_register") == 0);
        reviewer_name = message;

        while (true) {
            gets(buffer_rd);
            decode_message(buffer_rd, reserve_word, message);
            
            //if (false) {
            if (this->checkAlive() == false) {
                printf("COMMAND : %s\n", buffer_rd);
                if (reserve_word.compare("end_of_a_game") == 0) {
                    boxrunner.EndUp();
                    return SUCCESS_RETURN;
                } else {
                    usleep(100000);
                    FILE* result_file = fopen("result.txt", "r");
                    if (result_file == NULL) {
                        printf("@ %s : [judge_error_occurs] no result file found\n", play_ground.c_str());
                        return ERROR_WHILE_LOAD_FILE;
                    } else {
                        fscanf(result_file, "%s", buffer_rd);
                        printf("@ %s : [result_return]%s\n", play_ground.c_str(), buffer_rd);
                    }
                }
            } else {
                if (reserve_word.compare("end_of_a_game") == 0) {
                    boxrunner.EndUp();
                    return SUCCESS_RETURN;
                } else if (reserve_word.compare("request_one_line") == 0) {
                    read_proc = fork();
                    if (read_proc == -1) {
                        printf("@ %s : [judge_error_occurs] child process create error\n", play_ground.c_str());
                        return ERROR_WHILE_FORK_PROCESS;
                    } else if (read_proc == 0) {
                        clock_t start_clock = clock();
                        //int sz = read(pipe_rd[0], buffer_rd, sizeof(buffer_rd));
                        int sz = get_line_from_pipe(pipe_rd[0], buffer_rd);
                        clock_t end_clock = clock();
                        int ms = ( (end_clock - start_clock) * 1000.0) / CLOCKS_PER_SEC;
                        if (ms > time_limit_per) {
                            printf("@ %s : [result_return]TL\n", play_ground.c_str());
                            exit(0);
                        }
                        if (sz >= 0) {
                            buffer_rd[sz] = 0;
                            printf("%s @ %s : [reply_request_line]%s\n", reviewer_name.c_str(), play_ground.c_str(),
                                    buffer_rd);
                        } else {
                            printf("@ %s : [judge_error_occurs] pipe read error\n", play_ground.c_str());
                            return ERROR_WHILE_LOAD_FILE;
                        }
                        exit(0);
                    } else {
                        usleep(2 * time_limit_per * 1000);
                        int status;
                        pid_t ret =  waitpid(read_proc, &status, WNOHANG);
                        if (ret == 0) {
                            kill(read_proc, SIGKILL);
                            printf("@ %s : [result_return]TL\n", play_ground.c_str());
                        }
                    }
                } else if (reserve_word.compare("send_one_line") == 0) {
                    int sz = 0;
                    for (int i=0; i<(int)message.length(); ++i) {
                        buffer_wt[sz++] = message[i];
                    }
                    buffer_wt[sz++] = '\n';
                    buffer_wt[sz] = 0;
                    //printf("Buffer : %s\n", buffer_wt);
                    write(pipe_wt[1], buffer_wt, sz);
                } else {
                    printf("@ %s : [judge_error_occurs]no such reserve word\n", play_ground.c_str());
                }
            }
        }
        //end of normal player
    }
    
    Status Player::StartUp(const String &playername, const String &playground, 
                               const String &_source_file, bool _reviewer,
                                int _time_limit_all, int _memory_limit, int _time_limit_per) {
        
        String working_folder = config->top()("Player")["working_folder"];
        chdir(working_folder.c_str());
        chdir(playground.c_str());
        chdir(playername.c_str());
        //printf("Working Folder : %s|\n", working_folder.c_str());
        
        reviewer = _reviewer;
        player_name = playername;
        play_ground = playground;
        time_limit_all = _time_limit_all;
        memory_limit = _memory_limit;
        time_limit_per = _time_limit_per;
        source_file = _source_file;
        
        int status;
        status = compile();

        if (status != 0) {
            printf("@ %s : [result_return]CE\n", play_ground.c_str());
            return status;
        }
        
        status = 0;
        status = pipe(pipe_rd);
        status += pipe(pipe_wt);
        if (status != 0) {
            printf("@ %s : [judge_error_occurs] pipe create error\n", play_ground.c_str());
            return ERROR_WHILE_CREATE_PIPE;
        }
        
        boxrunner.Setup(*config);
        String main_name = get_filename_main(source_file);
        boxrunner.StartUp(main_name, pipe_wt, pipe_rd, lang_name, time_limit_all, memory_limit);
        
        if (reviewer) {
            return reviewer_code();
        } else {
            return normal_player_code();
        }
    }
}

using namespace Intereviewer;

//test
//./intereviewer main.cpp 13212 player01 atuoplayground false 2000 10240000 2000
//Argv[1] is the name of target source
//Argv[2] is the player name
//Argv[3] is the playground string
//Argv[4] is boolean for whether the player is a reviewer
//Argv[5] time limit all
//Argv[6] memory limit all
//Argv[6] time limit per
int main(int args, const char* argv[]) {
    Player player;
    INI::Parser config(config_file_path.c_str());
    //printf("Config Path : %s\n", config_file_path.c_str());
    player.Setup(config);
    String target_source = argv[1];
    //String work_folder = argv[2];
    String player_name = argv[2];
    String play_ground = argv[3];
    String reviewer_str = argv[4];
    bool reviewer = false;
    if (reviewer_str.compare("true") == 0) {
        reviewer = true;
    }
    String tmp = argv[5];
    int time_limit_all = stringToInt(tmp);
    tmp = argv[6];
    int memory_limit = stringToInt(tmp);
    tmp = argv[7];
    int time_limit_per = stringToInt(tmp);
    
    //printf("%d %d %d\n", time_limit_all, memory_limit, time_limit_per);
    
    player.StartUp(player_name, play_ground, target_source, reviewer, 
            time_limit_all, memory_limit, time_limit_per);
    
 
    return 0;
}

/*
[reviewer_register]reviewer
[send_one_line]hello
[request_one_line]
 */