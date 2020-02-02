#ifndef PLOG_HPP
#define PLOG_HPP

#include <iostream>

template <typename T = std::ostream>
class ALogger {
public:
    ALogger() : T() {}
    ALogger(const ALogger & other) : T(other) {}
    ALogger & operator=(const ALogger & other) { return *this; }
    virtual ~ALogger() {}

    template <typename U> ALogger & debug(const U & msg) { return *this; }
    template <typename U> ALogger & info(const U & msg) { return *this; }
    template <typename U> ALogger & warning(const U & msg) { return *this; }
    template <typename U> ALogger & error(const U & msg) { return *this; }
};

namespace log {

}

#endif // PLOG_HPP