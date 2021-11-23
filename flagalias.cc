#include "flagalias.h"

namespace gflags {
namespace ext {

/* trivial conversion */
template <>
std::string lexical_cast(const char* str) {
    return str;
}

/* conversions that exist in stl */
template <>
float lexical_cast(const char* str) {
    return std::strtof(str, nullptr);
}
template <>
long lexical_cast(const char* str) {
    return std::strtol(str, nullptr, 0);
}
template <>
long long lexical_cast(const char* str) {
    return std::strtoll(str, nullptr, 0);
}
template <>
unsigned long lexical_cast(const char* str) {
    return std::strtoul(str, nullptr, 0);
}
template <>
unsigned long long lexical_cast(const char* str) {
    return std::strtoull(str, nullptr, 0);
}

/* conversions that need to be truncated */
template <>
short lexical_cast(const char* str) {
    return static_cast<short>(lexical_cast<long>(str));
}
template <>
int lexical_cast(const char* str) {
    return static_cast<int>(lexical_cast<long>(str));
}
template <>
unsigned short lexical_cast(const char* str) {
    return static_cast<unsigned short>(lexical_cast<unsigned long>(str));
}
template <>
unsigned int lexical_cast(const char* str) {
    return static_cast<unsigned int>(lexical_cast<unsigned long>(str));
}

} // namespace ext
} // namespace gflags
