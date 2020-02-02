#include <string.h>
#include <string>
#include <iostream>
#if __cplusplus >= 201400L
#include <memory>
using Toto = std::unique_ptr<char *>;
#else
typedef char * Toto;
#endif

void overwrite(Toto & a) {
#if __cplusplus >= 201400L
    char * b = *a;
    a = std::make_unique<char*>(strdup("I overwrote it!"));
    // delete b; // OK! already freed
#else
    // delete a; // Oops! memory leak if forgot
    a = strdup("I overwrote it!");
#endif
}

// absent from standard library standard :(
static char * str2lower(const char * s) {
    char * ret = strdup(s);
    for (int i = 0; i < strlen(s); i++) {
        if (ret[i] >= 'A' && ret[i] <= 'Z') ret[i] += 32;
    }
    return ret;
}

void init_my_toto(Toto & a) {
#if __cplusplus >= 201400L
    a = std::make_unique<char*>(str2lower("Hello, World!"));
#else
    a = str2lower("Hello, World!");
#endif
}

int main(void) {
#if __cplusplus >= 201400L
    Toto a = std::make_unique<char *>(strdup(""));
    std::cout << *a << std::endl;
    init_my_toto(a);
    std::cout << *a << std::endl;
    overwrite(a);
    std::cout << *a << std::endl;
#else
    Toto a = "";
    std::cout << a << std::endl;
    init_my_toto(a);
    std::cout << a << std::endl;
    overwrite(a);
    std::cout << a << std::endl;
#endif
    return 0;
}