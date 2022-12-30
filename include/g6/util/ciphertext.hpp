#pragma once

#include <cstddef>
#include <memory>
#include <span>
#include <string_view>

#include <g6/abi/string.hpp>
#include <g6/abi/vector.hpp>
#include <g6/compat.hpp>

namespace seal {
    class Ciphertext;
}

namespace glasssix {
    class ciphertext {
    public:
        GX_API(JAVAUTIL) ciphertext(seal::Ciphertext&& impl);
        GX_API(JAVAUTIL) ciphertext(std::span<const std::byte> bytes);
        GX_API(JAVAUTIL) ciphertext(std::string_view base64);
        GX_API(JAVAUTIL) ciphertext(ciphertext&&) noexcept;
        GX_API(JAVAUTIL) ciphertext& operator=(ciphertext&&) noexcept;
        GX_API(JAVAUTIL) ~ciphertext();
        GX_API(JAVAUTIL) bool contains(const ciphertext& other) const;
        GX_API(JAVAUTIL) abi::vector<std::byte> as_bytes() const;
        GX_API(JAVAUTIL) abi::string as_base64() const;
        GX_API(JAVAUTIL) const seal::Ciphertext& get_impl() const noexcept;

    private:
        std::unique_ptr<seal::Ciphertext> impl_;
    };
} // namespace glasssix
