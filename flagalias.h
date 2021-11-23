#ifndef EXTEND_FLAG_ALIAS_H
#define EXTEND_FLAG_ALIAS_H

#include "flags.h"

#include <sstream>
#include <string>

namespace gflags {
namespace ext {

template <class T>
T lexical_cast(const char* str) {
    static std::istringstream ss; /* reusing has severe (positive) impact on performance */
    T                         value;
    ss.str(str);
    ss >> value;
    ss.clear();
    return value;
}

/* trivial conversion */
template <>
std::string lexical_cast(const char* str);

/* conversions that exist in stl */
template <>
float lexical_cast(const char* str);
template <>
long lexical_cast(const char* str);
template <>
long long lexical_cast(const char* str);
template <>
unsigned long lexical_cast(const char* str);
template <>
unsigned long long lexical_cast(const char* str);

/* conversions that need to be truncated */
template <>
short lexical_cast(const char* str);
template <>
int lexical_cast(const char* str);
template <>
unsigned short lexical_cast(const char* str);
template <>
unsigned int lexical_cast(const char* str);

/**
 * @brief Helper accessor/assignment alias class to support deprecated flags.
 *
 * This templated class wraps Flag::updateValue and Flag::getValue to 'alias'
 * a deprecated flag name as the updated name. The helper macro FLAG_ALIAS
 * will create a global variable instances of this wrapper using the same
 * Gflags naming scheme to prevent collisions and support existing callsites.
 */
template <typename T>
class FlagAlias {
public:
    FlagAlias& operator=(T const& v) {
        Flag::updateValue(name_, std::to_string(v));
        return *this;
    }

    /*explicit*/ operator T() const {
        return lexical_cast<T>(Flag::getValue(name_));
    }

    FlagAlias(const std::string& /*alias*/, const std::string& /*type*/, std::string name, T* /*storage*/)
        : name_(std::move(name)) {}

private:
    /// Friendly flag name.
    std::string name_;
};
} // namespace ext

} // namespace gflags

/**
 * @brief Create an alias to a command line flag.
 *
 * Like OSQUERY_FLAG, do not use this in the osquery codebase. Use the derived
 * macros that abstract the tail of boolean arguments.
 */
#define _FLAG_ALIAS(t, a, n, s, e)                                                                                     \
    gflags::ext::FlagAlias<t> FLAGS_##a(#a, #t, #n, &FLAGS_##n);                                                       \
    namespace flags {                                                                                                  \
    static GFLAGS_NAMESPACE::FlagRegisterer oflag_##a(#a, #a, #a, &FLAGS_##n, &FLAGS_##n);                             \
    const int                               flag_alias_##a = gflags::ext::Flag::createAlias(#a, {#n, s, e, 0, 1});     \
    }

/// See FLAG, FLAG_ALIAS aliases a flag name to an existing FLAG.
#define FLAG_ALIAS(t, a, n) _FLAG_ALIAS(t, a, n, 0, 0)

/// See FLAG_ALIAS, SHELL_FLAG_ALIAS%es are only available in osqueryi.
#define SHELL_FLAG_ALIAS(t, a, n) _FLAG_ALIAS(t, a, n, 1, 0)

/// See FLAG_ALIAS, EXTENSION_FLAG_ALIAS%es are only available to extensions.
#define EXTENSION_FLAG_ALIAS(a, n) _FLAG_ALIAS(std::string, a, n, 0, 1)

#endif
