#pragma once

#include <string>

namespace sha256 {

// Returns hex string representation of SHA-256 hashsum for file at given path
std::string sum(const std::string& filepath);

}  // namespace sha256
