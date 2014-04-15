#include "GlobalHelpers.h"

namespace Intereviewer{

/**
 * Convert an integer to a String
 * @param i     The integer
 * @return      The converted String
 */
String Inttostring(int i) {
    char str[15];
    sprintf(str, "%d", i);
    return (String)str;
}

/**
 * Convert a String to an integer
 * WARNING: did not check if it's valid
 * @param str   The String
 * @return      The converted interger
 */
int stringToInt(String str) {
    return atoi(str.c_str());
}


/**
 * Get current date/time, format is YYYY-MM-DD HH:mm:ss
 * https://stackoverflow.com/questions/997946/how-to-get-current-time-and-date-in-c
 * @return Current datetime in YYYY-MM-DD HH:mm:ss
 */
const String currentDateTime() {
    time_t     now = time(NULL);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tstruct);

    return buf;
}

/**
 * Get current date
 * https://stackoverflow.com/questions/997946/how-to-get-current-time-and-date-in-c
 * @return Current date in YYYY-MM-DD
 */
const String currentDate() {
    time_t     now = time(NULL);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d", &tstruct);

    return buf;
}

/**
 * Split the String into pieces by the delimeter
 * taken from https://stackoverflow.com/a/236803
 * @param str                   The original String
 * @param delim                 Delimeter
 * @param removeAppendedNull    Where to remove the appended empty strings
 * @return Splitted String
 */
StrVector split(const String &str, char delim, bool removeAppendedNull) {
    StrVector elems;
    std::stringstream ss(str);
    String item;
    while (getline(ss, item, delim)) {
        elems.push_back(item);
    }
    if (removeAppendedNull) {
        while (!elems.empty() && elems.back().empty()) {
            elems.pop_back();
        }
    }
    return elems;
}

/**
 * Load the whole text file content to a String
 * @param filename      File to load
 * @return File content
 */
String loadAllFromFile(String filename) {
    int tried = 0;
    String res = "", tmps;
    std::fstream fin(filename.c_str(), std::fstream::in);

    while (fin.fail() && tried++ < 10) {
        fin.open(filename.c_str(), std::fstream::in);
    }

    if (fin.fail()) {
        throw DException("File not found");
    }

    while (getline(fin,tmps)) {
        if (res != "") res += "\n";
        res += tmps;
        if (fin.eof()) break;
    }
    fin.close();
    return res;
}

void seperate(const String &str, StrVector &res, char sep) {
    int len = str.length();
    char buffer[MAX_STR_LENGTH];
    int now = 0;

    res.clear();

    for (int i=0; i<len; ++i) {
        if (str[i]==sep || (sep=='\n' && str[i]=='\r')) {
            if (now>0) {
                buffer[now] = 0;
                res.push_back(String(buffer));
                now = 0;
            }
        } else {
            buffer[now++] = str[i];
        }
    }
}

bool trans2bool(String s) {
    if (s == "0" || s=="false") {
        return false;
    }
    return true;
}

String int2string(int x) {
    char tmp[MAX_INT_LENGTH];
    sprintf(tmp, "%d", x);
    String ret = tmp;
    return ret;
}

int limitstring2resource(const String &limit_str) {
    if (limit_str == "TIME") {
        return RLIMIT_CPU;
    }
    if (limit_str == "MEMORY") {
        return RLIMIT_DATA;
    }
    if (limit_str == "FILE") {
        return RLIMIT_NOFILE;
    }
    if (limit_str == "PROCESS") {
        return RLIMIT_NPROC;
    }
    if (limit_str == "STACK") {
        return RLIMIT_STACK;
    }
}

String get_filename_main(const String &str) {
        String file_name = "";
        int dot, slash = -1;
        for (int i=0; i<(int)str.length(); ++i) {
                if (str[i]=='.') {
                        dot = i;
                } else if (str[i]=='/') {
                        slash = i;
                }
        }
#ifdef __DEBUG__
        printf("Slash %d  dot %d\n", slash, dot);
#endif
        for (int i=slash+1; i<dot; ++i) {
                char buffer[MAX_FILE_PATH];
                int len = str.copy(buffer, dot - slash -1, slash+1);
                buffer[len] = 0;
                file_name = buffer;
        }
        return file_name;
}

bool alphaornumber(char x) {
    if (x>='a' && x<='z') return true;
    if (x>='A' && x<='Z') return true;
    if (x>='0' && x<='9') return true;
    if (x=='_' || x=='-') return true;
    return false;
}

unsigned long ts2ms(struct timespec ts) {
    return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

/* convert syscall_t to 15bit sc_table index */
int16_t sc2idx(const syscall_t &scinfo) {
#ifdef __x86_64__
    assert((scinfo.scno >= 0) && (scinfo.scno < 1024) &&
        (scinfo.mode >= 0) && (scinfo.mode < 32));
    return (int16_t)(scinfo.scno | (scinfo.mode << 10));
#else /* __i386__ */
    assert((scinfo >= 0) && (scinfo < 1024));
    return (int16_t)(scinfo);
#endif /* __x86_64__ */
}

/* tag syscall number with linux32 abi mask */
int16_t abi32(int scno) {
    assert((scno >= 0) && (scno < 1024));
#ifdef __x86_64__
    return (int16_t)(scno | (1 << 10));
#else /* __i386__ */
    return (int16_t)(scno);
#endif /* __x86_64__ */
}


void policy(const policy_t* pp, const event_t* pe, action_t* pa)
{
    assert(pp && pe && pa);
    const minisbox_t* pmsb = (const minisbox_t*)pp->data;
    /* handle SYSCALL/SYSRET events with local rules */
    if ((pe->type == S_EVENT_SYSCALL) || (pe->type == S_EVENT_SYSRET))
    {
        const syscall_t scinfo = *(const syscall_t*)&(pe->data._SYSCALL.scinfo);
        pmsb->sc_table[sc2idx(scinfo)](&pmsb->sbox, pe, pa);
        return;
    }
    /* bypass other events to the default poicy */
    ((policy_entry_t)pmsb->default_policy.entry)(&pmsb->default_policy, pe, pa);
}

action_t* _CONT(const sandbox_t* psbox, const event_t* pe, action_t* pa)
{
    *pa = (action_t){S_ACTION_CONT}; /* continue */
    return pa;
}

action_t* _KILL_RF(const sandbox_t* psbox, const event_t* pe, action_t* pa)
{
    *pa = (action_t){S_ACTION_KILL, {{S_RESULT_RF}}}; /* restricted func. */
    return pa;
}

void LOG(String msg) {
        FILE* log_file = fopen ("/home/iniyk/IntereiwerStage/log/log.log", "w");
        if (log_file != NULL) {
                String output = currentDateTime () +" "+ msg;
                fprintf(log_file, "%s\n", output.c_str ());
        }
}

res_t probe(const sandbox_t* psbox, probe_t key)
{
    switch (key)
    {
    case P_ELAPSED:
        return ts2ms(psbox->stat.elapsed);
    case P_CPU:
        return ts2ms(psbox->stat.cpu_info.clock);
    case P_MEMORY:
        return psbox->stat.mem_info.vsize_peak / 1024;
    default:
        break;
    }
    return 0;
}

int decode_message(char* str, String &reserve_word, String &message) {
    int len = strlen(str);
    char tmp[MAX_STR_LENGTH];
    int cnt = 0, flag = 0;
    
    for (int i=0; i<len; ++i) {
        if (str[i] == ']') {
            flag = 0;
            tmp[cnt] = 0;
            reserve_word = String(tmp);
            message = String(&str[i+1]);
            return 0;
        }
        if (flag==1) {
            tmp[cnt++] = str[i];
        }
        if (str[i] == '[') {
            flag = 1;
        }
    }
    return -1;
}
int decode_message(char* str, String &reserve_word, String &message,
                                        String &target_player, String &play_ground) {
    char tmp_player_name[MAX_STR_LENGTH];
    char tmp_play_ground[MAX_STR_LENGTH];
    sscanf("%s %*s %s", tmp_player_name, tmp_play_ground);
    target_player = String(tmp_player_name);
    play_ground = StrVector(tmp_play_ground);
    int len = strlen(str);
    for (int i=0; i<len; ++i) {
        if (str[i]==':') {
            return (&str[i+1], reserve_word, message);
        }
    }
    return -1;
}

}