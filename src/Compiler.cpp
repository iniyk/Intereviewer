#include "Compiler.h"

namespace Intereviewer{
        Compiler::Compiler() {
            lv.clear();
        }
        
        Compiler::~Compiler() {
            lv.clear();
        }
        
        Status Compiler::Setup(INI::Parser &config) {
                lv.clear();

                compile_time_limit = stringToInt(config.top()("Player")("Compiler")["compile_time_limit"]);
                compile_file_size_limit = stringToInt(config.top()("Player")("Compiler")["compiler_file_size_limit"]);
                //output_folder = config.top()("Player")("Compiler")["output_folder"];
                //cinfo_folder =config.top()("Player")("Compiler")["cinfo_folder"];


                int ret = regist_language(config);

                return ret;
        }

        Status Compiler::regist_language(INI::Parser &config) {
                String lang_list = config.top()("Player")("LanguageRegister")["lang_list"];
                StrVector lang_list_array = split(lang_list, ' ', true);
                int sz = (int)lang_list_array.size();
                for (int i=0; i<sz; ++i) {
                        regist_one_lang(config, lang_list_array[i]);
                }
                return 0;
        }
        
        Status Compiler::regist_one_lang(INI::Parser &config, const String &lang) {
                int sz = (int)lv.size();
                for (int i=0; i<sz; ++i) {
                        if (lv[i].lang_name == lang) {
                                LOG("error : config lang has been rigisted.");
                                return -2;
                        }
                }

                Language tmp;
                tmp.lang_name = lang;
                tmp.compiler_command = config.top()("Player")("LanguageRegister")(lang)["compiler"];
                tmp.compiler_argv_before = config.top()("Player")("LanguageRegister")(lang)["compile_argv_before"];
                tmp.compiler_argv_after = config.top()("Player")("LanguageRegister")(lang)["compile_argv_after"];
                tmp.source_ext = config.top()("Player")("LanguageRegister")(lang)["lang_ext"];
                tmp.exec_ext = config.top()("Player")("LanguageRegister")(lang)["output_ext"];
                tmp.auto_output = trans2bool(config.top()("Player")("LanguageRegister")(lang)["auto_output"]);
                tmp.output_arg = config.top()("Player")("LanguageRegister")(lang)["output_command"];

                lv.push_back(tmp);
                return 0;
        }

        int Compiler::get_lang_id(const String &target_path) {
                StrVector tmp = split(target_path, '.', true);
                int sz = (int) tmp.size();
                String s_ext = tmp[sz-1];
                sz = (int) lv.size();
                for (int i=0; i<sz; ++i) {
                        if (lv[i].source_ext == s_ext) {
                                return i;
                        }
                }

            //LOG("error : trying to compiler a unregister language.");

                return -1;
        }

        Status Compiler::run_compile(const String &command) {
                struct rlimit compile_limit;

                pid_t cpid = fork();
                if (cpid == -1) {
                        return ERROR_WHILE_FORK_PROCESS;
                } else if (cpid == 0) {
                        usleep(50000);
                        setrlimit(RLIMIT_CPU,&compile_limit);
                        FILE* ret = popen(command.c_str(), "r");
                        pclose(ret);
                        exit(0);
                } else {
                        String kill_pid = String("pstree -p ")+Inttostring(cpid)+" | sed 's/(/\\n(/g' | grep '(' | sed 's/(\\(.*\\)).*/\\1/' | tr \"\\n\" \" \"";
                        FILE* pid_getter = popen(kill_pid.c_str(), "r");
                        fscanf(pid_getter, "%s", buffer_rd);
                        fclose(pid_getter);
                        kill_pid = buffer_rd;
                        //printf("Kill PID : %s\n",kill_pid.c_str());
                        String kill_command = String("kill ")+kill_pid;
                        //printf("Kill Command : %s\n",kill_command.c_str());
                        //LOG("message : Compile Child Process: "+Inttostring(cpid));
                        //LOG("message : Compile time limit: "+Inttostring(compile_time_limit));
                        int cstat,tused;
                        struct timeval case_startv,case_nowv;
                        struct timezone case_startz,case_nowz;
                        gettimeofday(&case_startv,&case_startz);
                        int cnt=-1;
                        while (1) {
                                usleep(50000);
                                cnt++;
                                gettimeofday(&case_nowv,&case_nowz);
                                tused=case_nowv.tv_sec-case_startv.tv_sec;
                                //if (cnt%20==0) LOG("message : Compiling Used: "+Inttostring(tused));
                                if (waitpid(cpid,&cstat,WNOHANG)==0) {
                                         if (tused>compile_time_limit) {
                                                //LOG("message : Time too much!");
                                                //LOG("message : kill `pstree -p "+Inttostring(cpid)+" | sed 's/(/\\n(/g' | grep '(' | sed 's/(\\(.*\\)).*/\\1/' | tr \"\\n\" \" \"`");
                                                FILE* ret = popen(kill_command.c_str(), "r");
                                                pclose(ret);
                                                waitpid(cpid,&cstat,0);
                                                return ERROR_WHILE_REVIEWER_COMPILE;
                                        }
                                } else if (WIFSIGNALED(cstat)&&WTERMSIG(cstat)!=0) {
                                        //LOG("message : Something is wrong.");
                                        //LOG("message : kill `pstree -p "+Inttostring(cpid)+" | sed 's/(/\\n(/g' | grep '(' | sed 's/(\\(.*\\)).*/\\1/' | tr \"\\n\" \" \"`");
                                        FILE* ret = popen(kill_command.c_str(), "r");
                                        pclose(ret);
                                        waitpid(cpid,&cstat,0);
                                        return ERROR_WHILE_REVIEWER_COMPILE;
                                }
                                if (WIFEXITED(cstat)) {
                                        waitpid(cpid,&cstat,0);
                                        //LOG("message : Compiled");
                                        break;
                                }
                        }
                        if (pid_running((int)stringToInt(kill_pid))) {
                            FILE* ret = popen(kill_command.c_str(), "r");
                            pclose(ret);
                        }
                }
                return SUCCESS_RETURN;
        }

	String Compiler::get_file_name(const String &target_path) {
		String file_name = "";
		int dot, slash = -1;
		for (int i=0; i<(int)target_path.length(); ++i) {
			if (target_path[i]=='.') {
				dot = i;
			} else if (target_path[i]=='/') {
				slash = i;
			}
		}
		for (int i=slash+1; i<dot; ++i) {
			char buffer[MAX_FILE_PATH];
			int len = target_path.copy(buffer, dot - slash -1, slash+1);
			buffer[len] = 0;
			file_name = buffer;
		}
		return file_name;
	}

	String Compiler::get_compile_command(const String &target_path) {
		String command = "";
		int lang_id = get_lang_id(target_path);
		String file_name = get_file_name(target_path);

		command += lv[lang_id].compiler_command;
		command += " ";
		command += lv[lang_id].compiler_argv_before;
		command += " ";

		command += target_path;
		command += " ";
		String output_path = file_name + "." +  lv[lang_id].exec_ext;
		if (!lv[lang_id].auto_output) {
			command += lv[lang_id].output_arg + " " + output_path;
			command += " ";
		}
		command += lv[lang_id].compiler_argv_after;
		command += " ";
		command += "2>compile.log";
		return command;
	}

	int Compiler::Compile(const String &target_path) {
		String command = get_compile_command(target_path);
		int ret = run_compile(command);
		return ret;
	}
        
            String Compiler::get_lang_name(const String &target_path) {
                int id = get_lang_id(target_path);
                return lv[id].lang_name;
            }

}