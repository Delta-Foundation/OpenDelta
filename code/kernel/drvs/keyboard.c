#include "../kernel-lib/keyboard.h"
#include "../kernel-lib/isr.h"
#include "../kernel-lib/screen.h"
#include "../kernel-lib/pic.h"

#include "../libc/include/ctype.h"
#include "../libc/include/types.h"
#include "../libc/include/stdbase.h"

static boolean shift_down = FALSE;
static boolean caps_lock = FALSE;

static char keyboard_buffer[KEYBOARD_BUFFER_SIZE];
static volatile int keyboard_head = 0;
static volatile int keyboard_tail = 0;

static const char scancode_to_ascii[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', '\t',
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0,
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.'
};

static const char scancode_to_ascii_shifted[128] = {
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b', '\t',
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', 0,
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0,
    '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
    '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.'
};

static inline unsigned long irq_save_flags(void) {
    unsigned long flags;
    __asm__ volatile ("pushf; pop %0; cli" : "=g"(flags) :: "memory");
    return flags;
}

static inline void irq_restore_flags(unsigned long flags) {
    __asm__ volatile ("push %0; popf" :: "g"(flags) : "memory", "cc");
}

char get_ascii_char(uint8_t scancode) {
    if (scancode >= 128) {
        return 0;
    }

    if (shift_down) {
        return scancode_to_ascii_shifted[scancode];
    }

    char c = scancode_to_ascii[scancode];

    if (caps_lock && c >= 'a' && c <= 'z') {
        c = (char)(c - 'a' + 'A');
    }

    return c;
}

void keyboard_buffer_push(char c) {
    unsigned long flags = irq_save_flags();
    int next = (keyboard_head + 1) % KEYBOARD_BUFFER_SIZE;

    if (next != keyboard_tail) {
        keyboard_buffer[keyboard_head] = c;
        keyboard_head = next;
    }

    irq_restore_flags(flags);
}

int keyboard_getchar(void) {
    unsigned long flags = irq_save_flags();

    if (keyboard_head == keyboard_tail) {
        irq_restore_flags(flags);
        return -1;
    }

    int c = (unsigned char)keyboard_buffer[keyboard_tail];
    keyboard_tail = (keyboard_tail + 1) % KEYBOARD_BUFFER_SIZE;

    irq_restore_flags(flags);
    return c;
}

static void keyboard_process_scancode(void) {
    uint8_t code = inb(KEYBOARD_PORT);
    boolean released = (code & 0x80) != 0;
    uint8_t key = code & 0x7F;

    if (key == KEY_LSHIFT || key == KEY_RSHIFT) {
        shift_down = !released;
        return;
    }

    if (key == KEY_CAPSLOCK && !released) {
        caps_lock = !caps_lock;
        return;
    }

    if (!released) {
        char ch = get_ascii_char(key);
        if (ch != 0) {
            keyboard_buffer_push(ch);
        }
    }
}

/* Called from the common IRQ stub.  The PIC EOI is sent by irq_handler. */
static void keyboard_irq_handler(regs_t * regs) {
    (void)regs;
    keyboard_process_scancode();
}

static void keyboard_wait_write(void) {
    for (int i = 0; i < 100000; i++) {
        if ((inb(KEYBOARD_STATUS_PORT) & 0x02) == 0) {
            return;
        }
    }
}

static void keyboard_wait_read(void) {
    for (int i = 0; i < 100000; i++) {
        if (inb(KEYBOARD_STATUS_PORT) & 0x01) {
            return;
        }
    }
}

static void keyboard_flush(void) {
    while (inb(KEYBOARD_STATUS_PORT) & 0x01) {
        (void)inb(KEYBOARD_DATA_PORT);
    }
}

void keyboard_install(void) {
    register_interrupt_handler(IRQ1, keyboard_irq_handler);

    /* Drain anything that may have been left in the controller. */
    keyboard_flush();

    /* Make sure the keyboard interface and scanning are enabled. */
    keyboard_wait_write();
    outb(KEYBOARD_STATUS_PORT, 0xAE);

    keyboard_wait_write();
    outb(KEYBOARD_DATA_PORT, 0xF4);

    keyboard_wait_read();
    (void)inb(KEYBOARD_DATA_PORT);

    keyboard_flush();
}

void keyboard_handler(void) {
    keyboard_process_scancode();
}

int console_getchar(void) {
    for (;;) {
        int c = keyboard_getchar();
        if (c >= 0) {
            return c;
        }

        __asm__ volatile ("hlt");
    }
}

int console_readline(char * buf, int max) {
    if (buf == NULL || max <= 0) {
        return 0;
    }

    int len = 0;

    for (;;) {
        int c = console_getchar();

        if (c == '\n' || c == '\r') {
            console_put_char('\n');
            buf[len] = '\0';
            return len;
        }

        if (c == '\b') {
            if (len > 0) {
                len--;
                console_put_char('\b');
            }
            continue;
        }

        if (c == 27) {
            /* Escape: clear the current line. */
            while (len > 0) {
                len--;
                console_put_char('\b');
            }
            continue;
        }

        if (c < 32 || c > 126) {
            continue;
        }

        if (len + 1 < max) {
            buf[len++] = (char)c;
            console_put_char((char)c);
        }
    }
}
