/**
 * COMMAND HEAD LIST: 
 *     RequestNextStep
 *     GameOver
 *     InputLine
 */
#include "common.h"
#include "config.h"
#include "boxrunner.h"
#include "connector.h"

using namespace Judger;

void Player::reset(char *argv[]) {
    config.init(config_file_path);
    setup_boxrunner();
    
    exec_path = argv[1];
    sscanf(argv[2], "%d", &if_interaction);
    sscanf(argv[3], "%d", &if_overlord);
    sscanf(argv[4], "%d", &time_limit);
    sscanf(argv[5], "%d", &memory_limit);
    
    if (if_interaction && (!if_overlord)) {
        sscanf(argv[6], "%d", &step_time_limit);
    }
}

void Player::LOG(const String &str) {
    time_t rawtime;
    struct tm * timeinfo;

    time(&rawtime);
    timeinfo = localtime (&rawtime);
    
    FILE *log_file = fopen(player_log_file_path.c_str(), "a");
    while (log_file == NULL) log_file = fopen(player_log_file_path.c_str(), "a");
    fprintf(log_file, "%s | Player %s : %s\n", asctime(timeinfo), player_name.c_str(), str.c_str());
    fclose(log_file);
}

void Player::setup_boxrunner() {
    config.selectSection("boxrunner");
    String args = config.getConfig("args");
    StrVector args_arr;
    seperate(args, args_arr);
    for (int i=0; i<(int)args_arr.size(); ++i) {
        boxrunner.addArg(args_arr[i]);
    }
    boxrunner.setTimeLimit(time_limit);
    boxrunner.setTimeLimit(memory_limit);
}

int Player::player_startup() {
    int box_input[2], box_output[2];
    pipe(box_input);
    pipe(box_output);
    int box_pid = boxrunner.Run(exec_path, box_input[0], box_output[1]);
    if (box_pid < 0) {
        if (box_pid == -1) LOG("[ERROR] sandbox error");
        if (box_pid == -2) LOG("[ERROR] target file invalid");
        return 1;
    }
    
    //last_clock = clock();
    
    while (1) {
        scanf("%s", buffer);
        String opt = buffer;
        if (opt == "InputLine") {
            gets(program_buffer);
            int len = strlen(program_buffer);
            program_buffer[len] = '\n';
            program_buffer[len+1] = 0;
            write(box_input[1], program_buffer, strlen(program_buffer));
            //last_clock = clock();
        } else if (opt == "GameOver") {
            
            return 0;
        } else if (opt == "RequestNextStep") {
            usleep(step_time_limit);
            
            String user_output = "";
            int now_len = read(box_output[0], program_buffer, sizeof(program_buffer));
            while (now_len != 0) {
                program_buffer[now_len] = 0;
                user_output += program_buffer;
                now_len = read(box_output[0], program_buffer, sizeof(program_buffer));
            }
            
            if (user_output == "") {
                printf("UserTimeLimitError\n");
                
                
                return 0;
            }
            
            StrVector user_output_arr;
            seperate(user_output, user_output_arr, '\n');
            for (int i=0; i<(int)user_output_arr.size(); ++i) {
                printf("OutputLine %s\n", user_output_arr[i]);
            }
        }
    }
    return 0;
}

Player player;

int main(int argc, char *argv[]) {
    player.reset(argv);
    player.Run();
    return 0;
}

