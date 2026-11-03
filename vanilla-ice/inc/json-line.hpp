#pragma once

#include <cstdint>
#include <list>
#include <string>

namespace json_line {

// Returns JSON-line in event format for `ice`
std::string event(const std::string& op, const std::string& path);

// Returns JSON-line in alert format for `gena`
std::string activity_alert(uint32_t ppid, const std::list<uint32_t>& pids);

}  // namespace json_line
