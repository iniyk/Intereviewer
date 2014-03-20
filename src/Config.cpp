#include "Config.h"

namespace Intereviwer{

Config::Config() {
    data.clear();
    section_now = "";
    Section auto_section(section_now);
    data.push_back(auto_section);
    now_section_id = 0;
}

int Config::init(const String &file_path) {
    char buffer[MAX_STR_LENGTH], temp[MAX_STR_LENGTH];

    data.clear();
    section_now = "";
    Section auto_section(section_now);
    data.push_back(auto_section);
    now_section_id = 0;

    FILE *ini_file = fopen(file_path.c_str(), "r");
    if (ini_file == NULL) return 2;

    while (fgets(buffer, MAX_STR_LENGTH, ini_file) != NULL) {
        int len = 0, tot = strlen(buffer);
        for (int i=0; i<tot; ++i) {
            if (buffer[i]==' ' || buffer[i] == '\r' || buffer[i] == '\n') {
                ;
            } else if (buffer[i] == ';' || buffer[i] == '#') {
                break;
            } else {
                buffer[len++] = buffer[i];
            }
        }
        buffer[len] = 0;
        if (buffer[0] == '[') {
            //Section init
            ///Format check
            for (int i=1; i<len-1; ++i) {
                if (buffer[i] == '[' || buffer[i] == ']')  return 1;
            }
            if (buffer[len-1] != ']') return 1;
            ///
            buffer[len-1] = 0;
            sscanf(&buffer[1], "%s", temp);
            String section_name = temp;
            bool exist = false;
            for (int i=0; i<(int)data.size(); ++i) {
                if (section_name == data[i].section_name) {
                    now_section_id = i;
                    section_now = section_name;
                    exist = true;
                    break;
                }
            }
            if (exist) continue;
            Section new_section(temp);
            data.push_back(new_section);
            now_section_id = (int)data.size() - 1;
            section_now = section_name;
        } else {
            //Entry init
            int equal = -1;
            for (int i=0; i<len; ++i) {
                if (buffer[i] == '=') {
                    if (equal >= 0) return 1;
                    equal = i;
                    if (equal == 0 || equal == len-1) return 1;
                } else if (alphaornumber(buffer[i])) {
                    ;
                } else {
                    return 1;
                }
            }
            if (equal==-1) return 1;

            buffer[equal] = 0;
            sscanf(buffer, "%s", temp);
            String par = temp;
            sscanf(&buffer[equal+1], "%s", temp);
            String val = temp;
            data[now_section_id].Set(par, val);
        }
    }

    fclose(ini_file);
    return 0;
}

int Config::selectSection(const String &section) {
    for (int i=0; i<(int)data.size(); ++i) {
        if (data[i].section_name == section) {
            section_now = data[i].section_name;
            now_section_id = i;
            return 0;
        }
    }
    return 1;
}

String Config::getConfig(const String &par) {
    return data[now_section_id].entry[par];
}
}