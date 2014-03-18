/*
 * Class Name : Compiler
 * Func :
 *   Compile user's program in the way which could be watched by another thread.
 *   Supplied language : C, CPP, Java, Python
 *   Other's could add other language simplyly register by Register(), and add a .cmp file.
 */
#ifndef _JUDGER_COMPILER_
#define _JUDGER_COMPILER_

#include "GlobalHelpers.h"
#include "config.h"

namespace Intereviwer{
    class Compiler{
    public:
        Compiler();
        /**
         * reset this object
         */
        void reset();
        /**
         * set up limits such as time limit & memeory limit by config
         * @param  config setup config
         * @return        0 normal return\n
         *                1 ini file formatting error\n
         *                2 ini file path invalid
         */
        int setupLimits(const Config &config);
        /**
         * register a new compiler by cmp file
         * @param  cmp_path cmp file path
         * @return          0 normal return\n
         *                  1 cmp file formatting error\n
         *                  2 cmp file path invalid\n
         *                  3 language has already been registered
         */
        int Register(const String &cmp_path);
        /**
         * compile target file
         * @param  lang_name        language name
         * @param  target_file_path target file path
         * @param  output_exec_file output execute file path
         * @return                  0 normal return\n
         *                          1 compile error, error message will be saven & could get it by Error() Func\n
         *                          2 target file invalid\n
         *                          3 language has not been registered\n
         *                          4 compiler execute failed
         */
        int Compile(const String &lang_name, const String &target_file_path, const String &output_exec_file);
        /**
         * get last compile error message
         * @return if Compile() hasn't been used since last init() it will return ""\n
         *         if no error occured last Compile() it will return ""\n
         *         else will get the error message
         */
        String Error();
    private:
        typedef std::pair<String, String> CompileOpt;
        typedef std::vector<CompileOpt> CmpOptArr;

        struct LangSetting{
            String lang_name;
            String compiler_cmd;
            String output_file_opt;
            CmpOptArr coa;
            LangSetting() {
                coa.clear();
                compiler_cmd = "";
                lang_name = "";
                output_file_opt = "";""
            }
            LangSetting(const String &name, const String &cmd, const String &ofo) {
                lang_name = name;
                compiler_cmd = cmd;
                output_file_opt = ofo;
                coa.clear();
            }
            void init(const String &name, const String &cmd, const String &ofo) {
                lang_name = name;
                compiler_cmd = cmd;
                output_file_opt = ofo;
                coa.clear();
            }
            void regOption(const String &para, const String &var="") {
                CompileOpt co;
                co = make_pair(para, var);
                coa.push_back(co);
            }
        };

        typedef std::vector<LangSetting> LangSettingArr;
        typedef std::map<String, int> LangDirector;

        /*------------------data-----------------*/
        LangSettingArr lsa;
        LangDirector ld;
        LimitMap lm;
        String error_message;
        /*-----------private function------------*/
        /**
         * Read an cmp file & regist a new compiler by it
         * @param  director id of lsa underlabel
         * @param  cmp_path cmp file path
         * @return          0 normal return\n
         *                  1 cmp file formatting error\n
         *                  2 cmp file path invalid
         */
        int SetupByCmpFile(int director, const String &cmp_path);
        void
    };

}

#endif
