#ifndef OPTIONAL_HPP
#define OPTIONAL_HPP

#include <typeinfo>
#include <exception>
#include <stdexcept>

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

#endif // OPTIONAL_HPP