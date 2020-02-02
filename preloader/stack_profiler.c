#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define ALT_STACK_SIZE 65535

static uint8_t * stack_top = 0;
static uint8_t * stack_bottom = 0;
static bool stack_painting = false;
static size_t stack_size = 0;
static void * painter_signal_stack = 0;
static stack_t painter_signal_stack_info;
static struct sigaction painter_actions;

static void sigsegv_handler(int sig) {
    (void)sig;
    stack_painting = false;
    stack_bottom++;
    stack_size = (size_t)stack_top - (size_t)stack_bottom;
    printf("Painted %lu of stack memory from %p to %p\n", stack_size, &stack_size, stack_bottom);
    exit(sig);
}

void stack_painter() {
    volatile size_t dummy = (size_t)&dummy;
    stack_bottom = &dummy - sizeof(dummy);
    stack_top = stack_bottom;
    stack_painting = true;
    painter_signal_stack = valloc(ALT_STACK_SIZE);
    painter_signal_stack_info.ss_sp = painter_signal_stack;
    painter_signal_stack_info.ss_flags = 0;
    painter_signal_stack_info.ss_size = ALT_STACK_SIZE;
    memset(&painter_actions, 0, sizeof(painter_actions));
    painter_actions.sa_flags = SA_SIGINFO | SA_ONSTACK;
    painter_actions.sa_sigaction = &sigsegv_handler;
    sigaction(SIGSEGV, &painter_actions, NULL);
    sigaltstack(&painter_signal_stack_info, NULL);
    while (stack_painting) {
        stack_bottom--;
        *stack_bottom = 0b10101010;
    }
    signal(SIGSEGV, SIG_DFL);
}

int main(void) {
    stack_painter();
    return 0;
}