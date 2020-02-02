#ifndef BASEVARIANT_HPP
#define BASEVARIANT_HPP

#include <iostream>
#include <typeinfo>
#include <stdarg.h>
#include <string.h>

class BaseVariant {
public:
    BaseVariant(size_t n, ...) :
    m_size(n),
    m_types(new const char *[n + 1]),
    m_current_type(0),
    m_value(0) {
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
    BaseVariant(const BaseVariant & other) :
    m_size(other.m_size),
    m_types(new const char *[m_size + 1]),
    m_current_type(other.m_current_type),
    m_value(other.m_value) {
        memcpy(m_types, other.m_types, sizeof(const char *) * m_size);
    }
    BaseVariant & operator=(const BaseVariant & other) {
        if (this != &other) {
            m_size = other.m_size;
            delete[] m_types;
            m_types = new const char *[m_size + 1];
            memcpy(m_types, other.m_types, sizeof(const char *) * m_size);
            m_current_type = other.m_current_type;
            m_value = other.m_value;
        }
        return *this;
    }
    virtual ~BaseVariant() { delete[] m_types; }

    std::ostream & operator>>(std::ostream & os) const {
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
    const char *    m_current_type;
    void *          m_value;
};

std::ostream & operator<<(std::ostream & os, const BaseVariant & variant) {
    return variant >> os;
}

#endif // BASEVARIANT_HPP