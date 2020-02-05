#include <iostream>
#include "Toto-v1.hpp"

void Toto::func1(size_t nargs, ...) {
    std::cout << __func__ << ": " << nargs << std::endl;
}

void Toto::func2(size_t nargs, ...) {
    std::cout << __func__ << ": " << nargs << std::endl;
}

void Toto::func3(size_t nargs, ...) {
    std::cout << __func__ << ": " << nargs << std::endl;
}

void Toto::func4(size_t nargs, ...) {
    std::cout << __func__ << ": " << nargs << std::endl;
}

void Toto::func5(size_t nargs, ...) {
    std::cout << __func__ << ": " << nargs << std::endl;
}

void Toto::func6(size_t nargs, ...) {
    std::cout << __func__ << ": " << nargs << std::endl;
}

void Toto::func7(size_t nargs, ...) {
    std::cout << __func__ << ": " << nargs << std::endl;
}

void Toto::func8(size_t nargs, ...) {
    std::cout << __func__ << ": " << nargs << std::endl;
}

void Toto::func9(size_t nargs, ...) {
    std::cout << __func__ << ": " << nargs << std::endl;
}

void Toto::func10(size_t nargs, ...) {
    std::cout << __func__ << ": " << nargs << std::endl;
}

void Toto::func11(size_t nargs, ...) {
    std::cout << __func__ << ": " << nargs << std::endl;
}

IToto * getNewToto() {
    return new Toto();
}