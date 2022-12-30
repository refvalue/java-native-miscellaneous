#pragma once

#include "ciphertext.hpp"

#include <cstddef>
#include <memory>
#include <span>

#include <g6/abi/string.hpp>
#include <g6/abi/vector.hpp>
#include <g6/compat.hpp>
#include <g6/meta.hpp>

namespace glasssix {
    class symmetric_homomorphic_encryptor {
    public:
        GX_API(JAVAUTIL) symmetric_homomorphic_encryptor(std::span<const std::byte> secret_key);
        GX_API(JAVAUTIL) symmetric_homomorphic_encryptor(symmetric_homomorphic_encryptor&&) noexcept;
        GX_API(JAVAUTIL) ~symmetric_homomorphic_encryptor();
        GX_API(JAVAUTIL) symmetric_homomorphic_encryptor& operator=(symmetric_homomorphic_encryptor&&) noexcept;
        GX_API(JAVAUTIL) static abi::vector<std::byte> generate_secret_key();
        GX_API(JAVAUTIL) ciphertext process(std::span<const std::byte> buffer) const;

        template <byte_like_contiguous_range Range>
        ciphertext process(Range&& range) const {
            return process(as_const_byte_span(range));
        }

    private:
        class impl;

        std::unique_ptr<impl> impl_;
    };
} // namespace glasssix
