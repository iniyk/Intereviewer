#ifndef _JUDGER_CONFIG_
#define _JUDGER_CONFIG_

#include "common.h"

namespace Intereviwer{
    class Config{
    public:
        Config();
        /**
         * @param  file_path ini file path
         * @return           0 Normal return, with no error occur
         *                   1 ini file FORMAT ERROR
         *                   2 ini file path invalid
         */
        int init(const String &file_path);
        /**
         * select a Section
         * @param  section section wanner to select
         * @return         0 Normal return, with no error occur
         *                 1 No such section
         */
        int selectSection(const String &section);
        /**
         * get config from now section
         * @param  par param name
         * @return     config entry value
         */
        String getConfig(const String &par);
    private:
        typedef std::map<String, String> Entrys;

        struct Section{
            String section_name;
            Entrys entry;
            Section() {
                entry.clear();
            }
            Section(const String &name) {
                section_name = name;
                entry.clear();
            }
            void Set(const String &par, const String &val) {
                entry[par] = val;
            }
            String Get(const String &par) {
                if (entry.count(par)) {
                    return entry[par];
                }
                return "";
            }
        };

        typedef std::vector<Section> Data;
        Data data;
        String section_now;
        int now_section_id;
    };
}

#endif
