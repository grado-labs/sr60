#pragma once

#include <cstdint>
#include <filesystem>
#include <string>

#include "bounded.hpp"

namespace sucker {

void read_range(uint32_t producer_id, const std::filesystem::path& path,
                std::uintmax_t begin, std::uintmax_t end,
                bounded::safe_priority_queue<std::string, uint32_t>& queue);

}  // namespace sucker
