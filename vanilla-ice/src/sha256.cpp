#include "sha256.hpp"

#include <openssl/sha.h>

#include <cstring>
#include <filesystem>
#include <format>
#include <fstream>

namespace fs = std::filesystem;

namespace sha256 {

std::string sum(const std::string& filepath) {
  if (not fs::exists(filepath)) {
    throw std::runtime_error(
        std::format("File is not exist: `{}`.\n", filepath));
  }

  std::ifstream ifs(filepath, std::ios::in | std::ios::binary);
  if (not ifs.good()) {
    throw std::runtime_error(std::format("Cannot open file `{}`: errno `{}`.\n",
                                         filepath, std::strerror(errno)));
  }

  constexpr const size_t buf_len{4096};
  char buf[buf_len];

  constexpr const size_t hash_len{SHA256_DIGEST_LENGTH};
  uint8_t hash[hash_len]{};

  SHA256_CTX ctx;
  SHA256_Init(&ctx);

  while (ifs.good()) {
    ifs.read(buf, buf_len);
    SHA256_Update(&ctx, buf, ifs.gcount());
  }

  SHA256_Final(hash, &ctx);
  ifs.close();

  std::ostringstream oss;
  for (size_t i = 0; i < hash_len; ++i) {
    oss << std::format("{:02x}", hash[i]);
  }

  return oss.str();
}

}  // namespace sha256
