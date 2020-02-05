#include <iostream>
#include "Toto-v1.hpp"

int main(void) {
    std::cout << "Call member functions of Toto object returned by getNewToto" << std::endl;
    IToto * toto = getNewToto();
    toto->func1(0);
    toto->func2(0);
    toto->func3(0);
    toto->func4(0);
    toto->func5(0);
    toto->func6(0);
    toto->func7(0);
    toto->func8(0);
    toto->func9(0);
    toto->func10(0);
    toto->func11(0);
    IToto * old_toto = toto;
    std::cout << "Call member functions of Toto instanciated on the stack" << std::endl;
    Toto mytoto;
    toto = &mytoto;
    toto->func1(0);
    toto->func2(0);
    toto->func3(0);
    toto->func4(0);
    toto->func5(0);
    toto->func6(0);
    toto->func7(0);
    toto->func8(0);
    toto->func9(0);
    toto->func10(0);
    toto->func11(0);
    delete old_toto;
    return 0;
}