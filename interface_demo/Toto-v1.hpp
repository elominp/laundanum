#ifndef TOTO_HPP
#define TOTO_HPP

#include <stdlib.h>
#include <stdint.h>

#define TOTO_VERSION 1

class IToto {
public:
    virtual void func1(size_t nargs, ...) = 0;
    virtual void func2(size_t nargs, ...) = 0;
    virtual void func3(size_t nargs, ...) = 0;
    virtual void func4(size_t nargs, ...) = 0;
    virtual void func5(size_t nargs, ...) = 0;
    virtual void func6(size_t nargs, ...) = 0;
    virtual void func7(size_t nargs, ...) = 0;
    virtual void func8(size_t nargs, ...) = 0;
    virtual void func9(size_t nargs, ...) = 0;
    virtual void func10(size_t nargs, ...) = 0;
    virtual void func11(size_t nargs, ...) = 0;
};

class Toto : public IToto {
public:
    void func1(size_t nargs, ...);
    void func2(size_t nargs, ...);
    void func3(size_t nargs, ...);
    void func4(size_t nargs, ...);
    void func5(size_t nargs, ...);
    void func6(size_t nargs, ...);
    void func7(size_t nargs, ...);
    void func8(size_t nargs, ...);
    void func9(size_t nargs, ...);
    void func10(size_t nargs, ...);
    void func11(size_t nargs, ...);
};

IToto * getNewToto();

#endif // TOTO_HPP