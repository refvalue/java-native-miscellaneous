#pragma once

#include "ciphertext.hpp"

#include <cstddef>
#include <memory>
#include <span>

#include <g6/abi/string.hpp>
#include <g6/abi/vector.hpp>
#include <g6/compat.hpp>

namespace glasssix {
    class symmetric_homomorphic_decryptor {
    public:
        GX_API(JAVAUTIL) symmetric_homomorphic_decryptor(std::span<const std::byte> secret_key);
        GX_API(JAVAUTIL) symmetric_homomorphic_decryptor(symmetric_homomorphic_decryptor&&) noexcept;
        GX_API(JAVAUTIL) ~symmetric_homomorphic_decryptor();
        GX_API(JAVAUTIL) symmetric_homomorphic_decryptor& operator=(symmetric_homomorphic_decryptor&&) noexcept;
        GX_API(JAVAUTIL) abi::vector<std::byte> process(const ciphertext& encrypted) const;
        GX_API(JAVAUTIL) abi::string process_as_string(const ciphertext& encrypted) const;

    private:
        class impl;

        std::unique_ptr<impl> impl_;
    };
} // namespace glasssix
