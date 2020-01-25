#ifndef PLOG_HPP
#define PLOG_HPP

#include <iostream>
#include <typeinfo>
#include <exception>
#include <stdexcept>
#include <stdarg.h>
#include <string.h>

namespace plog {
template <typename T>
class Optional {
public:
    class NullException : std::exception {
    public:
        virtual const char * what() { return "Null pointer accessed"; }
    };

    Optional() : m_value(0) {}
    Optional(T * value) : m_value(value) {}
    Optional(T & value) { m_value = &value; }
    Optional(const Optional & other) : m_value(other.m_value) {}
    Optional & operator=(const T & other) {
        if (this != &other) {
            m_value = other.m_value;
        }
        return *this;
    }
    const T * operator->() const { return m_value; }
    T * operator->() { return m_value; }
    const T & operator*() const { return *m_value; }
    T & operator*() { return *m_value; }
    operator bool() const { return m_value != 0; }
    bool operator==(const Optional & other) const {
        if (m_value && other.m_value) {
            return *m_value == *other.m_value;
        } else if (!m_value && !other.m_value) {
            return true;
        } else {
            return false;
        }
    }
    bool operator!=(const Optional & other) const { return !operator==(other); }
    bool operator<(const Optional & other) const {
        if (m_value && other.m_value) {
            return *m_value < *other.m_value;
        } else if (!m_value && other.m_value) {
            return true;
        } else {
            return false;
        }
    }
    bool operator<=(const Optional & other) const {
        return operator<(other) || operator==(other);
    }
    bool operator>(const Optional & other) const {
        if (m_value && other.m_value) {
            return *m_value < *other.m_value;
        } else if (!m_value && other.m_value) {
            return false;
        } else {
            return true;
        }
    }
    bool operator>=(const Optional & other) const {
        return operator>(other) || operator==(other);
    }
    virtual ~Optional() {}

    bool has_value() const { return operator bool(); }
    T & value() {
        if (m_value) {
            return *m_value;
        } else {
            throw new NullException();
        }
    }
    const T & value() const {
        if (m_value) {
            return *m_value;
        } else {
            throw new NullException();
        }
    }
    template <typename U> T & value_or(U &default_value) {
        return (m_value) ? m_value : default_value;
    }
    template <typename U> const T & value_or(const U & default_value) const {
        return (m_value) ? m_value : default_value;
    }
    void swap(Optional & other) {
        T * tmp = m_value;
        m_value = other.m_value;
        other.m_value = tmp;
    }
    void reset() {
        m_value = 0;
    }

private:
    T * m_value;
};

class BaseVariant {
public:
    BaseVariant(size_t n, ...) : m_size(n), m_types(new const char *[n + 1]) {
        va_list args;
        size_t i;
        const std::type_info * type_info = 0;

        va_start(args, n);
        for (i = 0; i < n; i++) {
            type_info = va_arg(args, const std::type_info *);
            m_types[i] = type_info->name();
        }
        va_end(args);
        m_types[i] = 0;
    }
    BaseVariant(const BaseVariant & other) : m_size(other.m_size), m_types(new const char *[n + 1]) {
        memcpy(m_types, other.m_types, sizeof(const char *) * m_size);
    }
    BaseVariant & operator=(const BaseVariant & other) {
        if (this != &other) {
            
        }
        return *this;
    }

    std::ostream & operator<<(std::ostream & os) const {
        size_t i = 0;
        while (m_types[i] != 0) {
            if (i > 0) {
                os << ";";
            }
            os << m_types[i++];
        }
        return os;
    }
private:
    size_t          m_size;
    const char **   m_types;
};
}

std::ostream & operator<<(std::ostream & os, const plog::BaseVariant & variant) {
    return variant << os;
}

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