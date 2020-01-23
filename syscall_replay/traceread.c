#ifndef _GNU_SOURCE
 #define _GNU_SOURCE
#endif

#include <dlfcn.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>

static int fds[1024];

ssize_t read(int fd, void * buf, size_t count) {
    static bool initialized = false;
    static ssize_t (*orig_read)(int, void *, size_t) = NULL;

    if (!initialized) {
        orig_read = dlsym(RTLD_NEXT, "read");
    }
    fds[0] = open("read/ld0", O_CREAT | O_APPEND | O_WRONLY, 0644);
    write(fds[fd], &count, sizeof(count));
    ssize_t return_value = orig_read(fd, buf, count);
    write(fds[0], &return_value, sizeof(return_value));
    if (return_value > 0) {
        write(fds[fd], buf, return_value);
    }
    close(fds[0]);
    return return_value;
}