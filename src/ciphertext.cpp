#include "ciphertext.hpp"

#include "homomorphic_encryption.hpp"

#include <g6/cryptography.hpp>

#include <seal/seal.h>

namespace glasssix {
    ciphertext::ciphertext(seal::Ciphertext&& impl) : impl_{std::make_unique<seal::Ciphertext>(std::move(impl))} {}

    ciphertext::ciphertext(std::span<const std::byte> bytes) : impl_{std::make_unique<seal::Ciphertext>()} {
        impl_->load(get_default_seal_context(), reinterpret_cast<const seal::seal_byte*>(bytes.data()), bytes.size());
    }

    ciphertext::ciphertext(std::string_view base64) : ciphertext{base64_decode(base64)} {}

    ciphertext::ciphertext(ciphertext&&) noexcept = default;

    ciphertext::~ciphertext() = default;

    ciphertext& ciphertext::operator=(ciphertext&&) noexcept = default;

    bool ciphertext::contains(const ciphertext& other) const {
        seal::Ciphertext result;
        auto&& evaluator = get_default_seal_evaluator();
        
        evaluator.sub(*impl_, *other.impl_, result);
        
        return true;
    }

    abi::vector<std::byte> ciphertext::as_bytes() const {
        auto size = impl_->save_size();
        abi::vector<std::byte> result(size);

        impl_->save(reinterpret_cast<seal::seal_byte*>(result.data()), result.size());
        
        return result;
    }

    abi::string ciphertext::as_base64() const {
        return base64_encode(as_bytes());
    }

    const seal::Ciphertext& ciphertext::get_impl() const noexcept {
        return *impl_;
    }

} // namespace glasssix
