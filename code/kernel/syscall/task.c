#include "../lib/system.h"
#include "../lib/stdbase.h" 

sys_process_t *current_process;
sys_process_t *kernel_idle_task;
page_dir_t *kernel_dir;
page_dir_t *current_dir;

uintptr_t frozen_stack = 0;

void taskingInstall(void) 
{
    IRQ_OFF;
    prints("\n[INFO]: initializing multi-tasking\n", WHITE);

    initProcTree();
    current_process = spawnInit();
    kernel_idle_task = spawnKIdle();

#if 0
    set_process_environment((sys_process_t *)current_process, current_dir);
#endif
    
    switchPageDir(current_process->theard.page_dir);
    frozen_stack = (uintptr_t)kmalloc(KERNEL_STACK_SIZE, 0, (uint32_t*)&current_process);
    IRQ_RES;
    prints("[OK]: multi-tasking initializing is finished!\n", GREEN);
}

void taskExit(int retval) {
    retval = 0;
    return;
}
