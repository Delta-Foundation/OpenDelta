#include "../kernel-lib/system.h"
#include "../libc/include/stdbase.h" 
#include "../mem/header/memory.h"

static sys_process_t * current_process;
static sys_process_t * kernel_idle_task;
static page_dir_t * kernel_dir;
static page_dir_t * current_dir;

uintptr_t frozen_stack = 0;

void tasking_install(void) 
{
    IRQ_OFF;
    prints("\n[INFO]: initializing multi-tasking\n", WHITE);

    init_proc_tree();
    current_process = spawn_init();
    kernel_idle_task = spawn_KIdle();

#if 0
    set_process_environment((sys_process_t *)current_process, current_dir);
#endif
    switch_page_dir(current_process->theard.page_dir);
    frozen_stack = (uintptr_t)kmalloc(KERNEL_STACK_SIZE, 0, (uint32_t*)&current_process);
    IRQ_RES;
    prints("[OK]: multi-tasking initializing is finished!\n", GREEN);
}

void task_exit(int retval) {
    retval = 0;
    return;
}
