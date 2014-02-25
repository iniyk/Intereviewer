#include "boxrunner.h"

using namespace Intereviwer;

void BoxRunner::local_log_func(const String &str) {

}

void BoxRunner::init(LogFunc lf = &silent_log_func, const Config &config) {
    memset(args, 0, sizeof(args));
    memory_limit = AUTO_MEMORY_LIMIT;
    time_limit = AUTO_TIME_LIMIT;
    language = 'cpp';
    LOG = lf;

    string2intarr(config.)
}

res_t BoxRunner::probe(const sandbox_t* psbox, probe_t key) {
    switch (key) {
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

void BoxRunner::SetLanguage(const String &lang) {
    language = lang;
}

void BoxRunner::SetTimeLimit(int val) {
    time_limit = val;
}

void BoxRunner::SetMemoryLimit(int val) {
    memory_limit = val;
}

int BoxRunner::Run(const String &target_path, int fd_rd, int fd_wt) {
    strcpy(args[0], target_path.c_str());

    if (sandbox_init(&msb.sbox, args) != 0) {
#ifdef __DEBUG__
        LOG("[ERROR] error at boxrunner Run() -- sandbox_init error");
#endif
        return -1;
    }

    SetPolicy
}

int BoxRunner::status() {
    int ret;

    pid_t tmp = waitpid(sand_pid, &ret, WNOHANG);

    if (tmp == 0) return 0;

    return WEXITSTATUS(status);
}

int BoxRunner::kill() {
    return (int)kill(0, SIGKILL);
}

void BoxRunner::setup_policy() {
    for (int i=0; i<=INT16_MAX; ++i) {
        msb.sc_table[sc] = _CONT;
    }
    bool *ban_list = &banlist_other;
    if (language == 'java') {

    }
    for (int i)
}

void BoxRunner::SetPolicy(int16_t ban[], int size)/////this Function is not complete
{
    assert(pmsb);

    /* initialize table of system call rules */
    int sc;
    for (sc = 0; sc <= INT16_MAX; sc++)
    {
        pmsb->sc_table[sc] = _CONT;
    }
    for (int i=0; i<size; ++i) {
        pmsb->sc_table[ban[i]] = _KILL_RF;
    }
    /* override the default policy of the sandbox */
    pmsb->default_policy = pmsb->sbox.ctrl.policy;
    pmsb->default_policy.entry = (pmsb->default_policy.entry) ?: \
        (void*)sandbox_default_policy;
    pmsb->sbox.ctrl.policy = (policy_t){(void*)policy, (long)pmsb};
}

void BoxRunner::policy(const policy_t* pp, const event_t* pe, action_t* pa)
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

action_t* BoxRunner::_CONT(const sandbox_t* psbox, const event_t* pe, action_t* pa)
{
    *pa = (action_t){S_ACTION_CONT}; /* continue */
    return pa;
}

action_t* BoxRunner::_KILL_RF(const sandbox_t* psbox, const event_t* pe, action_t* pa)
{
    *pa = (action_t){S_ACTION_KILL, {{S_RESULT_RF}}}; /* restricted func. */
    return pa;
}
