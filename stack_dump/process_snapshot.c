#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

#define PAGE_SIZE       4096
#define STACK_SIZE      16 * PAGE_SIZE // 64 KB stack is enough
#define PATH_LENGTH     512
#define LINE_MAX_LENGTH 1024

#ifdef __i386__
# define SP "ESP"
# define BP "EBP"
#elif defined(__x86_64__)
# define SP "RSP"
# define BP "RBP"
#else
# error "CPU architecture not supported"
#endif

static uint8_t *    snapshot_stack = NULL;
static void *       orig_sp = NULL;
static void *       orig_bp = NULL;

typedef struct memmap_s {
    void *  bottom;
    void *  top;
    char    rights[5];
    size_t  offset;
    uint8_t device_major;
    uint8_t device_minor;
    size_t  inode;
    char *  pathname; // pointer on buffer to fill
} memmap_t;

static int init_altstack(void) {
    int fd = open("/dev/zero", O_RDWR);
    if (fd < 0) {
        fprintf(stderr, "Failed to open /dev/zero, unable to get a new fd\n");
        return -1;
    }
    // fd = -1 to avoid opening /dev/zero and STACK_GROWSDOWN doesn't work on my linux kernel :'(
    if ((snapshot_stack = mmap(NULL, STACK_SIZE, PROT_EXEC | PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0)) == MAP_FAILED) {
        close(fd);
        fprintf(stderr, "Failed to allocate memory for the new stack, errno = %s\n", strerror(errno));
        return -1;
    }
    close(fd);
    snapshot_stack += STACK_SIZE;
    printf("Allocated new stack memory at %p\n", snapshot_stack);
    return 0;
}

static int get_next_memmap(FILE * proc_fd, memmap_t * dest) {
    char * buffer = NULL;
    size_t n = 0;
    
    if (getline(&buffer, &n, proc_fd) <= 0 || buffer == NULL) {
        return -1;
    }
    int parsed_bytes = sscanf(buffer, "%lx-%lx %s %lx %02x:%02x %lu %s",
        &dest->bottom,
        &dest->top,
        dest->rights,
        &dest->offset,
        &dest->device_major,
        &dest->device_minor,
        &dest->inode,
        dest->pathname
    );
    free(buffer);
    return 0;
}

static memmap_t * get_stack(void) {
    static bool stack_found = false;
    static memmap_t stack_map;

    if (!stack_found) {
        char proc_maps_path[PATH_LENGTH];
        FILE * proc_fd;
        pid_t my_pid = getpid();

        snprintf(proc_maps_path, PATH_LENGTH - 1, "/proc/%d/maps", my_pid);
        proc_maps_path[PATH_LENGTH - 1] = '\0';
        printf("opening maps file %s\n", proc_maps_path);
        if ((proc_fd = fopen(proc_maps_path, "rb")) == NULL) {
            fprintf(stderr, "Unable to open process memory maps\n");
            return NULL;
        }
        stack_map.pathname = proc_maps_path;
        while (!get_next_memmap(proc_fd, &stack_map)) {
            printf("map pathname found = %s\n", stack_map.pathname);
            if (!strncmp(stack_map.pathname, "[stack]", 8)) {
                fclose(proc_fd);
                return &stack_map;
            }
        }
        fclose(proc_fd);
        return NULL;
    }
    return (stack_found) ? &stack_map : NULL;
}

static int do_snapshot(void) {
    printf("Hello, World!\n");
    memmap_t * stack = get_stack();
    if (stack != NULL) { // dump stack
        write(1, stack->bottom, (size_t)stack->top - (size_t)stack->bottom);
    }
    return 0;
}

typedef int (*altstack_cb)(void);

static int process_on_altstack(altstack_cb cb) {
    static int ret;
    static altstack_cb cb_to_call;

    if (snapshot_stack != NULL || !init_altstack()) {
        cb_to_call = cb;
        printf("Switch to alternative stack\n");
        // Backup original stack pointers
        asm volatile ("\tmov %%"SP", %0\n" : "=r"(orig_sp) ::);
        asm volatile ("\tmov %%"BP", %0\n" : "=r"(orig_bp) ::);
        // Switch to new stack
        asm volatile ("\tmov %0, %%"SP"\n" :: "r"(snapshot_stack) :);
        asm volatile ("\tmov %0, %%"BP"\n" :: "r"(snapshot_stack) :);
        printf("Switched stack\n");
        ret = cb_to_call();
        printf("Alt stack callback executed, restoring stack pointers\n");
        // Go back to previous stack
        asm volatile ("\tmov %0, %%"SP"\n" :: "r"(orig_sp) :);
        asm volatile ("\tmov %0, %%"BP"\n" :: "r"(orig_bp) :);
        printf("Restored stack pointers, continuing process\n");
        return ret;
    } else {
        return -1;
    }
}

static int snapshot(void) {
    printf("snapshoting...\n");
    int ret = process_on_altstack(&do_snapshot);
    printf("snapshoting done and returned %d\n", ret);
    return ret;
}

static int do_destroy_main_stack(void) {
    memmap_t * stack = get_stack();
    if (stack != NULL) {
        printf("Stack found, beginning unmapping\n");
        if (munmap(stack->bottom, (size_t)stack->top - (size_t)stack->bottom) == -1) {
            printf("Failed to unmap stack, errno = %s\n", strerror(errno));
            return -1;
        }
        printf("Stack successfully unmapped\n");
    }
    else {
        printf("404 stack not found\n");
        return -1;
    }
    return 0;
}

static int destroy_stack(void) {
    printf("destroying stack...\n");
    int ret = process_on_altstack(&do_destroy_main_stack);
    printf("stack destroyed\n"); // will never reach until here
    return ret;
}

int main(void) {
    snapshot();
    destroy_stack();
    return 0;
}