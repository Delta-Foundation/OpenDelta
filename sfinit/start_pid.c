/* 
 * наверно будем использовать свою мелкую реализацию  
 * стандартной библиотеки, если это не понадобится 
 * то поменяем на дефолтные библиотеки типо #include <stdio.h>
 */
#include "../kernel/lib/system.h"
#include "../kernel/lib/stdbase.h"
#include "../kernel/lib/types.h"

void start_pid1(void) 
{
    pid_t pid = 1;
    prints("[SFINIT]: [initializing PID1]\n", CYAN);
    spawnInit();
    initProcTree();
    initProc(pid);
}
