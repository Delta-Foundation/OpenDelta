/* 
 * наверно будем использовать свою мелкую реализацию  
 * стандартной библиотеки, если это не понадобится 
 * то поменяем на дефолтные библиотеки типо #include <stdio.h>
 */
#include "../src/lib/system.h"
#include "../src/lib/stdbase.h"
#include "../src/lib/types.h"

void start_pid1(void) {
    pid_t pid = 1;
    prints("[SFINIT]: [initializing PID1]\n", CYAN);
    initProc("SFINIT", pid);
}

void start_min_dltsh(void) {
    pid_t dltsh_pid = 2;
    prints("[SFINIT]: [initializing dltsh, pid: %d]\n", WHITE, dltsh_pid);
    initProc("Minimal dltsh(lang)", dltsh_pid);
}
