#include "compiler.h"

using namespace Judger;

int Compile::SetupByCmpFile(int director, const String &cmp_path) {
    FILE *cmp_file;
    cmp_file = fopen(cmp_file.c_str(), "r");
    
    if (cmp_file == NULL) return 2;
    LangSetting &lst = lsa[director];

    String lang_name, compiler_cmd, output_file_opt;
    char chrBuff[MAX_STR_LENGTH];
    //Read lang name
    if (fscanf(cmp_file, "%s", chrBuff) == EOF) {
        return 1;
    }
    lang_name = chrBuff;
    //Read compiler command
    if (fscanf(cmp_file, "%s", chrBuff) == EOF) {
        return 1;
    }
    compiler_cmd = chrBuff;
    //Read output file option
    if (fscanf(cmp_file, "%s", chrBuff) == EOF) {
        return 1;
    }
    output_file_opt = chrBuff;

    lst.init(lang_name, compiler_cmd);
    //Read other options
    while (!feof(cmp_file)) {
        int k;
        fscanf(cmp_file, "%d", &k);
        if (k<=0) {
            return 1;
        }
        String para, opt;
        opt = "";
        if (fscanf(cmp_file, "%s", chrBuff) == EOF) {
            return 1;   
        }
        para = chrBuff;
        for (int i=1; i<k; ++i) {
            if (fscanf(cmp_file, "%s", chrBuff) == EOF) {
                return 1;
            }
            opt += chrBuff;
        }
        lst.regOption(para, opt);
    }

    close(cmp_file);
    return 0;
}

Compiler::Compiler() {
    lsa.clear();
    ld.clear();
    lm.clear();
    error_message = "";
}

void Compiler::reset() {
    lsa.clear();
    ld.clear();
    lm.clear();
    error_message = "";
}

int Compiler::Register(const String &cmp_file) {
   int dir = lsa.size();
   LangSetting ls_empty;
   lsa.push_back(ls_empty);
   int ret = SetupByCmpFile(dir, cmp_file);
   if (ret != 0) {
        lsa.pop_back();
        return ret;
   }
   if (ld.count(lsa[dir].lang_name) == 0) {
       ld[lsa[dir].lang_name] = dir;
       return 0;
   } else {
       lsa.pop_back();
       return 3;
   }
}

int Compiler::Compile(const String lang_name, const String &target_file_path, const String &output_exec_file) {
    int lang_dir = ld[lang_name];
    String compile_command_string = "";
    compile_command_string += lsa[lang_dir].compiler_cmd;
    for (int i=0; i<(int)lsa[lang_dir].coa.size(); ++i) {
        compile_command_string += " " + lsa[lang_dir].coa[i].first;
        if (lsa[lang_dir].coa[i].second != "") {
            compile_command_string += " " + las[lang_dir].coa[i].second;
        }
    }
    compile_command_string += " " + output_file_opt + " " + output_file_opt;
    compile_command_string += " " + target_file_path;
    // compile_command_string setup finish
    
    pid_t pid = fork();
    if (pid == -1) return 4;
    if (pid == 0) {
        //child process
        
    } else {
        
    }
}

int Compiler::setupLimits(const Config &config) {
    FILE *ini_file = fopen(ini_file_path.c_str(), "r");
    if (ini_file == NULL) {
        return 2;
    }
    char chrBuff[MAX_STR_LENGTH];
    while (fscanf(ini_file, "%s", chrBuff) != EOF) {
        String limit_name = chrBuff;
        int limit_numer;
        if (fscanf(ini_file, "%d", &limit_numer) == EOF) {
            return 1;
        }
        lm[limit_name] = limit_numer;
    }
    return 0;
}

String Compiler::Error() {
    return error_message;
}
 
