#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <sys/stat.h>
#include <syscall.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#if __x86_64__
 #define RET  user_regs->rax
 #define ARG0 user_regs->orig_rax
 #define ARG1 user_regs->rdi
 #define ARG2 user_regs->rsi
 #define ARG3 user_regs->rdx
#elif __i386__
 #define ARG1 user_regs->orig_eax
#else
 #error "Architecture not supported"
#endif

#define NB_SYSCALLS 329

typedef int (*syscall_tracer)(pid_t pid, const struct user_regs_struct *);

static syscall_tracer syscall_tracers[NB_SYSCALLS];
static int read_fds[1024] = {0};

int trace_read(pid_t pid, const struct user_regs_struct * user_regs) {
    if (ARG1 > 0 && !read_fds[ARG1]) {
        //fds[ARG2] = open("read/0", O_CREAT | O_TRUNC | O_WRONLY, S_IRWXU | S_IRWXG | S_IRWXO);
        return -1;
    }
    else if (ARG1 > 2 && read_fds[ARG1] < 0) {
        return -1;
    }
    else if ((long long)RET < -1) {
        write(read_fds[ARG1], &ARG3, sizeof(ARG3)); // requested count
    }
    else {
        write(read_fds[ARG1], &RET, sizeof(RET)); // number of read bytes
        if (RET > 0) {
            size_t word;
            for (size_t i = 0; i < RET; i += sizeof(word)) {
                word = ptrace(PTRACE_PEEKDATA, pid, ARG2 + i, NULL);
                write(read_fds[ARG1], &word, sizeof(word));
            }
        }
    }
    return 0;
}

int deinit_read_tracer(void) {
    for (size_t i = 0; i < NB_SYSCALLS; i++) {
        if (read_fds[i] != 0) {
            close(read_fds[i]);
        }
    }
}

int deinit_syscall_tracers(void) {
    deinit_read_tracer();
    return 0;
}

int init_read_tracer(void) {
    read_fds[0] = open("read/0", O_CREAT | O_APPEND| O_WRONLY, 0644);
    syscall_tracers[SYS_read] = &trace_read;
    return 0;
}

int init_syscall_tracers(void) {
    for (size_t i = 0; i < 3; i++) {
        close(i);
    }
    memset(syscall_tracers, 0, sizeof(syscall_tracer) * NB_SYSCALLS);
    init_read_tracer();
    return 0;
}

int log_syscall(pid_t pid, const struct user_regs_struct * user_regs) {
    return (ARG0 >= NB_SYSCALLS || syscall_tracers[ARG0] == NULL) ? -1 :
        syscall_tracers[ARG0](pid, user_regs);
}

int trace_process(pid_t pid) {
    int status = 0;

    init_syscall_tracers();
    while (waitpid(pid, &status, 0) && ! WIFEXITED(status)) {
        struct user_regs_struct user_regs;
        memset(&user_regs, 0, sizeof(user_regs));
        ptrace(PTRACE_GETREGS, pid, NULL, &user_regs);
        log_syscall(pid, &user_regs);
        ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
    }
    deinit_syscall_tracers();
    return status;
}

int main(int argc, char ** argv) {
    pid_t child = fork();
    if (!child) {
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        execvp(argv[1], argv + 1);
    }
    else {
        return trace_process(child);
    }
    return 0;
}