#include "../kernel-lib/system.h"
#include "../mem/header/memory.h"
#include "../libc/include/stdbase.h" 

static sys_process_t * current_process;
static sys_process_t * ready_queue;

void shed_init(void) {
    current_process = NULL;
    ready_queue = NULL;
}

int process_create(void (* entry)(), void * stack) {
    sys_process_t * process = (sys_process_t *)0x100000;
    process->context.rip = (uint64_t)entry;
    process->context.rsp = (uint64_t)stack + 4096;
    process->context.rflags = 0x202;
    process->state = PROC_READY;
    process->pid = 1;
    process->next = NULL;

    if (!ready_queue) {
        ready_queue = process;
    } 
    else {
        sys_process_t * tmp = ready_queue;
        while (tmp->next) tmp = tmp->next;
        tmp->next = process;
    }

    return process->pid;
}

void set_process_environment(
    sys_process_t * process,
    page_dir_t * dir
) {
    if (process == NULL || dir == NULL) {
        prints("[ERROR]: set_process_environment: invalid arguments\n", RED);
        return;
    }

    process->theard.page_dir = dir;
    current_dir = dir;
}

void switch_context(
    proc_context_t * old, 
    proc_context_t * new
) {
    __asm__ volatile (
        "pushfq\n"
        "pushq %%rbp\n"
        "pushq %%rdi\n"
        "pushq %%rsi\n"
        "pushq %%rdx\n"
        "pushq %%rcx\n"
        "pushq %%rbx\n"
        "pushq %%rax\n"
        
        "movq %%rsp, (%0)\n"
        "movq (%1), %%rsp\n"
        
        "popq %%rax\n"
        "popq %%rbx\n"
        "popq %%rcx\n"
        "popq %%rdx\n"
        "popq %%rsi\n"
        "popq %%rdi\n"
        "popq %%rbp\n"
        "popfq\n"
        : : "r"(old), "r"(new) : "memory"
    );
}

void shedule(void) {
    if (!current_process || current_process->state != PROC_RUNNING) {
        if (ready_queue) {
            sys_process_t * next = ready_queue;
            ready_queue = ready_queue->next;
            next->state = PROC_RUNNING;
            switch_context(&current_process->context, &next->context);
            current_process = next;
        }
    }
}

void init_proc_tree(void) {
    return;
}

sys_process_t * spawn_init(void) {
    sys_process_t * init = malloc(sizeof(sys_process_t));
    return init;
}

sys_process_t * spawn_KIdle(void) {
    sys_process_t *idle = malloc(sizeof(sys_process_t));
    memset(idle, 0x00, sizeof(sys_process_t));
    return idle;
}

