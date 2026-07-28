#include "../lib/keyboard.h"
#include "../lib/ctype.h"
#include "../lib/types.h"
#include "../lib/pic.h"
#include "../lib/stdbase.h"

static boolean shift_down = FALSE;
static boolean caps_lock = FALSE;

static char keyboard_buffer[KEYBOARD_BUFFER_SIZE];
static volatile int keyboard_head = 0;
static volatile int keyboard_tail = 0;

static char scancode_to_ascii[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', '\t',
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0,
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.'
};

static char scancode_to_ascii_shifted[128] = {
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b', '\t',
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', 0,
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0,
    '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
    '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.'
};

static inline unsigned long irq_save_flags(void) {
    unsigned long flags;
    __asm__ volatile ( "pushf; pop %0; cli" : "=g"(flags) :: "memory");
    return flags;
}

static inline void irq_restore_flags(unsigned long flags) {
    __asm__ volatile ( "push %0; popf" :: "g"(flags) : "memory", "cc");
}

char get_ascii_char(uint8_t scancode) {
    if (isalpha(scancode)) {
        boolean upper = shift_down ^ caps_lock;
        char base = scancode_to_ascii[(uint8_t)scancode];
        return upper ? toupper(base) : base;
    }

    if (shift_down) {
        return scancode_to_ascii_shifted[(uint8_t)scancode];
    }

    else {
        return scancode_to_ascii[(uint8_t)scancode];
    }
}

void keyboard_buffer_push(char c) {
    unsigned long flags = irq_save_flags();
    int next = (keyboard_head + 1) % KEYBOARD_BUFFER_SIZE;
    
    if (next != keyboard_tail) {
        keyboard_buffer[keyboard_head] = c;
        keyboard_head = next;
    }

    else {}

    irq_restore_flags(flags);
}

char keyboard_getchar(void) {
    unsigned long flags = irq_save_flags();
    
    if (keyboard_head == keyboard_tail) {
        irq_restore_flags(flags);
        return -1;
    }

    char c = (char)keyboard_buffer[keyboard_tail];
    keyboard_tail = (keyboard_tail + 1) % KEYBOARD_BUFFER_SIZE;
    
    irq_restore_flags(flags);
    return c;
}

void keyboard_handler(void)
{
    uint8_t code = inb(KEYBOARD_PORT);

    boolean released = code & 0x80;
    uint8_t key = code & 0x7F;

    if (key == KEY_LSHIFT || key == KEY_RSHIFT) {
        shift_down = !released;
        send_end_of_int(1);
        return;
    }

    if (key == KEY_CAPSLOCK && !released) {
        caps_lock = !caps_lock;
        send_end_of_int(1);
        return;
    }

    if (!released) {
        char ch = get_ascii_char(key);
        if (ch) {
            keyboard_buffer_push(ch);
        }
    }

    send_end_of_int(1);
}
