#include <sys/resource.h>
#include <stdio.h>

int main() {
    struct rlimit stack_limit;
    getrlimit(RLIMIT_STACK, &stack_limit);
    printf("Stack limit max = %ld\ncur = %ld\n", stack_limit.rlim_max, stack_limit.rlim_cur);
    stack_limit.rlim_cur = stack_limit.rlim_max;
    setrlimit(RLIMIT_STACK, &stack_limit);
    printf("Stack limit max = %ld\ncur = %ld\n", stack_limit.rlim_max, stack_limit.rlim_cur);
    stack_limit.rlim_cur = 8388608;
    setrlimit(RLIMIT_STACK, &stack_limit);
    printf("Stack limit max = %ld\ncur = %ld\n", stack_limit.rlim_max, stack_limit.rlim_cur);
    return 0;
}