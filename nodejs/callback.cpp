#include <iostream>
#if __cplusplus >= 201103L
#include <functional>
void call(std::function<void ()> toto) {
    toto();
}
#else
void call(void (*toto)()) {
    toto();
}
void titi() { std::cout << "Hello, C" << std::endl; }
#endif
int main(void) {
#if __cplusplus >= 201103L
    call([]() { std::cout << "Hello, C++11!" << std::endl; });
#else
    call(&titi);
#endif
}