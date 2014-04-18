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
		
		Status Setup(INI::Parser &config);
		Status Compile(const String &target_path);
                         String get_lang_name(const String &target_path);
	private:
		int compile_time_limit;
		int compile_file_size_limit;
		String output_folder;
		String cinfo_folder;
                          char buffer_rd[MAX_STR_LENGTH];
		
		LangVector lv;

		Status regist_language(INI::Parser &config);
		Status regist_one_lang(INI::Parser &config, const String &lang);
		int get_lang_id(const String &target_path);
		Status run_compile(const String &command);
		String get_file_name(const String &target_path);
		String get_compile_command(const String &target_path);
	};
}

#endif