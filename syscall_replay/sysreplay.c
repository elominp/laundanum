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

typedef int (*syscall_replayer)(pid_t pid, struct user_regs_struct *);

static syscall_replayer syscall_replayers[NB_SYSCALLS];
static int read_fds[1024] = {0};

int replay_read(pid_t pid, struct user_regs_struct * user_regs) {
    if (ARG1 > 0 && !read_fds[ARG1]) {
        //fds[ARG2] = open("read/0", O_CREAT | O_TRUNC | O_WRONLY, S_IRWXU | S_IRWXG | S_IRWXO);
        return -1;
    }
    else if (ARG1 > 2 && read_fds[ARG1] < 0) {
        return -1;
    }
    else if ((long long)RET < -1) {
        size_t previous_count = 0;
        read(read_fds[ARG1], &previous_count, sizeof(previous_count)); // requested count
        // if previous_count != count this time, may trace a warning or sth
    }
    else {
        read(read_fds[ARG1], &RET, sizeof(RET)); // number of read bytes
        if (RET > 0) {
            size_t word;
            for (size_t i = 0; i < RET; i += sizeof(word)) {
                read(read_fds[ARG1], &word, sizeof(word));
                ptrace(PTRACE_POKEDATA, pid, ARG2 + i, word);
            }
        }
        ptrace(PTRACE_SETREGS, pid, user_regs, user_regs);
    }
    return 0;
}

int deinit_read_replayer(void) {
    for (size_t i = 0; i < NB_SYSCALLS; i++) {
        if (read_fds[i] != 0) {
            close(read_fds[i]);
        }
    }
}

int deinit_syscall_replayers(void) {
    deinit_read_replayer();
    return 0;
}

int init_read_replayer(void) {
    read_fds[0] = open("read/0", O_RDONLY);
    syscall_replayers[SYS_read] = &replay_read;
    return 0;
}

int init_syscall_replayers(void) {
    for (size_t i = 0; i < 3; i++) {
        close(i);
    }
    memset(syscall_replayers, 0, sizeof(syscall_replayer) * NB_SYSCALLS);
    init_read_replayer();
    return 0;
}

int log_syscall(pid_t pid, struct user_regs_struct * user_regs) {
    return (ARG0 >= NB_SYSCALLS || syscall_replayers[ARG0] == NULL) ? -1 :
        syscall_replayers[ARG0](pid, user_regs);
}

int replay_process(pid_t pid) {
    int status = 0;

    init_syscall_replayers();
    while (waitpid(pid, &status, 0) && ! WIFEXITED(status)) {
        struct user_regs_struct user_regs;
        memset(&user_regs, 0, sizeof(user_regs));
        ptrace(PTRACE_GETREGS, pid, NULL, &user_regs);
        log_syscall(pid, &user_regs);
        ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
    }
    deinit_syscall_replayers();
    return status;
}

int main(int argc, char ** argv) {
    pid_t child = fork();
    if (!child) {
        int dev_null = open("/dev/null", O_RDONLY);
        dup2(dev_null, 0);
        close(dev_null);
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        execvp(argv[1], argv + 1);
    }
    else {
        return replay_process(child);
    }
    return 0;
}