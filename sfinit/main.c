#include "./lib/start_pid.h"
#include "../kernel/lib/system.h"
#include "../kernel/lib/stdbase.h"
#include "../kernel/lib/types.h"

int main(void) {
    prints("[SFINIT START]\n", CYAN);
    start_pid1();
}
