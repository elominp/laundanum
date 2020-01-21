#include <stdio.h>

int main(void) {
    char buffer[16777216] = "Hello, World!";
    printf("%s\n", buffer);
    return 0;
}