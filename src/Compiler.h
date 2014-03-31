#ifndef _JUDGER_COMPILER_
#define _JUDGER_COMPILER_

#include "GlobalHelpers.h"
#include "ini.hpp"

namespace Intereviewer{
	struct Language
	{
		String lang_name;
		String compiler_command;
		String compiler_argv_before;
		String compiler_argv_after;
		String source_ext;
		String exec_ext;
		bool auto_output;
		String output_arg;
	};

	typedef std::vector<Language> LangVector;

	class Compiler
	{
	public:
		Compiler();
		virtual ~Compiler();
		
		int Setup(INI::Parser &config);
		int Compile(const String &target_path, const String &run_id);
	private:
		int compile_time_limit;
		int compile_file_size_limit;
		String output_folder;
		String cinfo_folder;
		
		LangVector lv;

		int regist_language(INI::Parser &config);
		int regist_one_lang(INI::Parser &config, const String &lang);
		int get_lang_id(const String &target_path);
		int run_compile(const String &command);
		String get_file_name(const String &target_path);
		String get_compile_command(const String &target_path, const String &run_id);
	};
}

#endif