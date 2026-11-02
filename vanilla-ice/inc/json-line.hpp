#pragma once

#include <list>
#include <string>

namespace json_line {

// Returns JSON-line in event format for `ice`
std::string event(const std::string& op, const std::string& path);

}  // namespace json_line
