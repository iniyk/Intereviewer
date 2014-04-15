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
    
    int Player::StartUp(const String &playername, const String &playground, 
                               const String &work_folder, const String &source_file, bool _reviewer,
                                int time_limit_all, int memory_limit, int time_limit_per) {
        String working_folder = config->top()("Player")["working_folder"];
        chdir(working_folder.c_str());
        chdir(work_folder);
        
        reviewer = _reviewer;
        player_name = playername;
        play_ground = playground;

        Compiler compiler;
        compiler.Setup(*config);

        int status;

        status = compiler.Compile(source_file);
        if (status != 0) {
            printf("@ %s : [error_occurs] compile error", play_ground.c_str());
            return status;
        }
        lang_name = compiler.get_lang_name(source_file);
        
        int pipe_rd[2], pipe_wt[2];
        status = pipe(pipe_rd);
        status += pipe(pipe_wt);
        if (status != 0) {
            printf("@ %s : [error_occurs] pipe create error", play_ground.c_str());
            return ERROR_WHILE_CREATE_PIPE;
        }
        
        BoxRunner boxrunner;
        boxrunner.Setup(*config);

        String main_name = get_filename_main(source_file);

        boxrunner.StartUp(main_name, pipe_wt, pipe_rd, lang_name, time_limit_all, memory_limit);

        String reserve_word;
        String message;

        if (reviewer) {
            gets(buffer_rd);
            decode_message(buffer_rd, reserve_word, message);
            assert(reserve_word.compare("players_register") == 0);
            player_list = split(message, ' ', true);

            for (int i=0; i<(int)player_list.size(); ++i) {
                printf("%s @ %s : [reviewer_register]%s", player_list[i].c_str(), play_ground.c_str(),
                        player_name.c_str());
            }
            
            send_proc = fork();
            if (send_proc == -1) {
                printf("@ %s : [error_occurs] child process create error", play_ground.c_str());
                return ERROR_WHILE_FORK_PROCESS;
            } else if (send_proc == 0) {
                close(pipe_wt[1]);
                while (true) {
                    int sz = (int)read(pipe_rd[0], buffer_rd, sizeof(buffer_rd));
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
        //end of reviewer
        } else {
            
        //end of normal player
        }
    }
}

using namespace Intereviewer;

//Argv[1] is the name of target source
//Argv[2] is the working folder contains the target
//Argv[3] is the player name
//Argv[4] is the playground string
int main(int args, const char* argv[]) {
    Player player;
    INI::Parser config(Intereviewer::config_file_path);
    player.Setup(config);
 
    return 0;
}

