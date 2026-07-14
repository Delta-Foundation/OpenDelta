#include "../lib/system.h"
#include "../mem/header/memory.h"
#include "../lib/stdbase.h" 

void initProcTree(void) {
    return;
}

void initProc(const char* pname, pid_t pid) {
    printf("[pinfo]: [started %s process with %d PID]", pname, pid, LCYAN);
    return (void)pid;
    return (void)pname; 
}

sys_process_t *spawnInit(void) {
    sys_process_t *init = malloc(sizeof(sys_process_t));
    return init;
}

sys_process_t *spawnKIdle(void) {
    sys_process_t *idle = malloc(sizeof(sys_process_t));
    memset(idle, 0x00, sizeof(sys_process_t));
    return idle;
}
