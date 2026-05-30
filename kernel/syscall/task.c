#include "../lib/system.h"
#include "../lib/stdbase.h" 

sys_process_t *current_process;
sys_process_t *kernel_idle_task;
page_dir_t *kernel_dir;
page_dir_t *current_dir;

uintptr_t frozen_stack = 0;

void tackingInstall(void) 
{
    IRQ_OFF;

    prints("\n[INFO]: initializing multi-tasking\n", WHITE);

    initProcTree();
    current_process = spawnInit();
    kernel_idle_task = spawnKIdle();

#if 0
#endif

}

void taskExit(int retval) {
    retval = 0;
    return;
}
