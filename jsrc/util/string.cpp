
#include "string.hpp"

namespace util {
auto
remove_all_occurrences(std::string &str, std::string_view sub_str) -> void {
    size_t n = 0;
    while ((n = str.find(sub_str, n)) != std::string::npos) str.erase(n, sub_str.size());
}
}  // namespace util
