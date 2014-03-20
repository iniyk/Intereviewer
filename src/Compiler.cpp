#include "Compiler.h"

namespace Intereviwer{
	int Compiler::Setup(Config &config) {
		lv.clear();

		int status = config.selectSection("Compiler");
		if (status == 0) {
			compile_time_limit = stringToInt(config.getConfig("compile_time_limit"));
			compile_file_size_limit = stringToInt(config.getConfig("compiler_file_size_limit"));
			output_folder = config.getConfig("output_folder");
			cinfo_folder = config.getConfig("cinfo_folder");

		} else {
			LOG("error : config missing secion Compiler.");
			return -2;
		}

		int ret = regist_language(config);

		return ret;
	}

	int Compiler::regist_language(Config &config) {
		int status = config.selectSection("LanguageRegister");
		if (status == 0) {
			String lang_list = config.getConfig("lang_list");
			StrVector lang_list_array = split(lang_list, ' ', true);
			int sz = (int)lang_list_array.size();
			for (int i=0; i<sz; ++i) {
				regist_one_lang(config, lang_list_array[i]);
			}
		} else {
			LOG("error : config language list don't exist.");
			return -1;
		}
		return 0;
	}

	int Compiler::regist_one_lang(Config &config, const String &lang) {
		int status = config.selectSection(lang);
		if (status == 0) {
			int sz = (int)lv.size();
			for (int i=0; i<sz; ++i) {
				if (lv[i].lang_name == lang) {
					LOG("error : config lang has been rigisted.");
					return -2;
				}
			}

			Language tmp;
			tmp.lang_name = lang;
			tmp.compiler_command = config.getConfig("compiler");
			tmp.compiler_argv_before = config.getConfig("compile_argv_before");
			tmp.compiler_argv_after = config.getConfig("compile_argv_after");
			tmp.source_ext = config.getConfig("lang_ext");
			tmp.exec_ext = config.getConfig("output_ext");
			tmp.auto_output = trans2bool(config.getConfig("auto_output"));
			tmp.output_arg = config.getConfig("output_command");

			lv.push_back(tmp);
		} else {
			LOG("error : config lang section don't exist.");
			return -1;
		}
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

		LOG("error : trying to compiler a unregister language.");

		return -1;
	}

	int Compiler::run_compile(const String &command) {
		struct rlimit compile_limit;

		pid_t cpid = fork();
		if (cpid == -1) {
			LOG("error : error while fork a process to compile source file.");
		} else if (cpid == 0) {
		 	usleep(50000);
	        setrlimit(RLIMIT_CPU,&compile_limit);
	        LOG("message : Compile Command: "+command);
	        int res=system(command.c_str());
	        exit(0);
		} else {
	        LOG("message : Compile Child Process: "+Inttostring(cpid));
	        LOG("message : Compile time limit: "+Inttostring(compile_time_limit));
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
	            if (cnt%20==0) LOG("message : Compiling Used: "+Inttostring(tused));
	            if (waitpid(cpid,&cstat,WNOHANG)==0) {
	                if (tused>compile_time_limit) {
	                    LOG("message : Time too much!");
	                    LOG("message : kill `pstree -p "+Inttostring(cpid)+" | sed 's/(/\\n(/g' | grep '(' | sed 's/(\\(.*\\)).*/\\1/' | tr \"\\n\" \" \"`");
	                    system(("kill `pstree -p "+Inttostring(cpid)+" | sed 's/(/\\n(/g' | grep '(' | sed 's/(\\(.*\\)).*/\\1/' | tr \"\\n\" \" \"`").c_str());
	                    waitpid(cpid,&cstat,0);
	                    return -2;
	                }
	            }
	            else if (WIFSIGNALED(cstat)&&WTERMSIG(cstat)!=0) {
	                LOG("message : Something is wrong.");
	                LOG("message : kill `pstree -p "+Inttostring(cpid)+" | sed 's/(/\\n(/g' | grep '(' | sed 's/(\\(.*\\)).*/\\1/' | tr \"\\n\" \" \"`");
	                system(("kill `pstree -p "+Inttostring(cpid)+" | sed 's/(/\\n(/g' | grep '(' | sed 's/(\\(.*\\)).*/\\1/' | tr \"\\n\" \" \"`").c_str());
	                waitpid(cpid,&cstat,0);
	                return -1;
	            }
	            if (WIFEXITED(cstat)) {
	                waitpid(cpid,&cstat,0);
	                LOG("message : Compiled");
	                break;
	            }
	        }
	        system(("kill `pstree -p "+Inttostring(cpid)+" | sed 's/(/\\n(/g' | grep '(' | sed 's/(\\(.*\\)).*/\\1/' | tr \"\\n\" \" \"`").c_str());
		}
		return 0;
	}

	String Compiler::get_file_name(const String &target_path) {
		String file_name = "";
		int dot, slash;
		for (int i=0; i<(int)target_path.length(); ++i) {
			if (target_path[i]=='.') {
				dot = i;
			} else if (target_path[i]=='/') {
				slash = i;
			}
		}
		for (int i=slash+1; i<dot; ++i) {
			char buffer[MAX_FILE_PATH];
			int len = target_path.copy(buffer, slash+2, dot - slash - 1);
			buffer[len] = 0;
			file_name = buffer;
		}
		return file_name;
	}

	String Compiler::get_compile_command(const String &target_path, const String &run_id) {
		String command = "";
		int lang_id = get_lang_id(target_path);
		String file_name = get_file_name(target_path);

		command += lv[lang_id].compiler_command;
		command += " ";
		command += lv[lang_id].compiler_argv_before;
		command += " ";

		command += target_path;
		command += " ";

		String output_path = output_folder + file_name + lv[lang_id].exec_ext;
		if (!lv[lang_id].auto_output) {
			command += lv[lang_id].output_arg + " " + output_path;
			command += " ";
		}
		command += lv[lang_id].compiler_argv_after;
		command += " ";
		command += "2>"+cinfo_folder+run_id+".log";
		return command;
	}

	int Compiler::Compile(const String &target_path, const String &run_id) {
		String command = get_compile_command(target_path, run_id);
		int ret = run_compile(command);
		return ret;
	}

}