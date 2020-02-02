#include <sstream>

std::string toto() {
    std::ostringstream ss;
    ss << "Hello, World n" << 42 << ','
    << std::endl << "you're the " << 3.14
    << " magic number" << std::endl;
    // C++ way until std::fmt in C++20
    char buffer[512];
    sscanf(buffer, "Hello, World n%d,\n"
    "you're the %f magic number\n",
    42, 3.14); // C way
    return ss.str();
}