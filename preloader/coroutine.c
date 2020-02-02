#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#ifndef SIZE_COSTACK
#define SIZE_COSTACK    4096
#endif

#ifndef NB_COSTACKS
#define NB_COSTACKS     32
#endif

#ifndef ALLOC
#define ALLOC           salloc
#endif

#define INIT_COROUTINES uint8_t * memstacks = ALLOC(NB_COSTACKS * SIZE_COSTACK); \
  for (size_t i = 0; i < NB_COSTACKS; i++) { \
      costacks[i] = memstacks + (i * SIZE_COSTACK); \
  }

static void *   costacks[NB_COSTACKS];
static uint8_t  costacks_usage[NB_COSTACKS / sizeof(uint8_t) + 1];

static void * salloc(size_t size) {
    void * ptr = mmap(NULL, size, PROT_EXEC | PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
    return (ptr != MAP_FAILED) ? ptr : NULL;
}

static void * find_first_free_costack() {
    uint8_t * costack;
    for (size_t i = 0; i < NB_COSTACKS; i += 8) {
        for (size_t j = 0; j < 8; j++) {
            if (!(costacks_usage[i] & (1 << j))) {
                costack = costacks[i + j];
                return costack + SIZE_COSTACK;
            }
        }
    }
    return NULL;
}

size_t costart(size_t (* func)()) {
    static size_t (* cb)();
    volatile void * costack = find_first_free_costack();
    cb = func;
    if (costack) {
        volatile static size_t sp, bp, ret;
        asm volatile("\tmov %%rsp,%0\n\t" : "=r"(sp) : : "memory");
        asm volatile("\tmov %%rbp,%0\n\t" : "=r"(bp) : : "memory");
        //printf("costack = %p\nprevious sp = %p\n", costack, sp); // Won't work on unoptimized builds ???
        asm volatile("\tmov %0,%%rsp\n\t" : : "r"(costack) : "memory");
        asm volatile("\tmov %0,%%rbp\n\t" : : "r"(costack) : "memory");
        ret = cb();
        asm volatile("\tmov %0,%%rsp\n\t" : : "r"(sp) : "memory");
        asm volatile("\tmov %0,%%rbp\n\t" : : "r"(bp) : "memory");
        //asm volatile("\tmov %%rsp,%0\n\t" : "=r"(sp) : : "memory");
        return ret;
    }
    return 0;
}

int hello() {
    printf("Hello, World!\n");
    printf("Foo!\n");
    return 0;
}

int main() {
    INIT_COROUTINES
    printf("costack is aligned with a delta of %ld\n", (size_t)memstacks % 16);
    costart(hello);
    return 0;
}
