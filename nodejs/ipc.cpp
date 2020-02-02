#include <unistd.h>

int main(void) {
    char buffer[32768];
    pid_t pid;
    int master2worker[2];
    int worker2master[2];
    pipe(master2worker);
    pipe(worker2master);
    if ((pid = fork()) == 0) {
        // worker
        close(master2worker[1]);
        close(worker2master[0]);
        write(worker2master[1], "Hello, I'm worker!", 19);
        while (1) {
            read(master2worker[0], buffer, 32768);
            // do something
        }
    } else {
        // master
        close(worker2master[1]);
        close(master2worker[0]);
        write(master2worker[1], "Hello, I'm master!", 19);
        while (1) {
            read(worker2master[0], buffer, 32768);
            // do something
        }
    }
    return 0;
}