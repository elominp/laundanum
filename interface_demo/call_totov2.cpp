#include "Toto-v2.hpp"

int main(void) {
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
    toto->iShoudlntBeAddedThere(0);
    IToto * old_toto = toto;
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
    toto->iShoudlntBeAddedThere(0);
    delete old_toto;
    return 0;
}