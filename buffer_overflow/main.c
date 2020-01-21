#include <stdio.h>

#define KN_FIELD1 5
#define KN_FIELD2 255

typedef struct MyStruct {
    char field1[KN_FIELD1];
    char field2[KN_FIELD2];
} MyStruct;

int main(void) {
    MyStruct foo;
    strcpy(foo.field1, "Hello");
    strcpy(foo.field2, "Hello, World!");
    printf("foo.field1 = %s\nfoo.field2 = %s\n", foo.field1, foo.field2);
    return 0;
}