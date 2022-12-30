#include "symmetric_homomorphic_encryptor.hpp"

#include "homomorphic_encryption.hpp"

#include <algorithm>
#include <cstddef>
#include <stdexcept>

#include <g6/char8_t_remediation.hpp>
#include <g6/cryptography.hpp>

#include <seal/seal.h>

namespace glasssix {
    class symmetric_homomorphic_encryptor::impl {
    public:
        impl(std::span<const std::byte> secret_key)
            : encryptor_{get_default_seal_context(), load_seal_secret_key(secret_key)} {}

        static abi::vector<std::byte> generate_secret_key() {
            return generate_seal_secret_key(get_default_seal_context());
        }

        ciphertext process(std::span<const std::byte> buffer) const {
            if (buffer.empty()) {
                throw std::invalid_argument{U8("The buffer must be non-empty.")};
            }

            abi::vector<std::uint64_t> input_values(buffer.size());

            std::ranges::transform(
                buffer, input_values.begin(), [](std::byte inner) { return std::to_integer<std::uint64_t>(inner); });

            seal::Plaintext plain{input_values};
            seal::Ciphertext encrypted;

            encryptor_.encrypt_symmetric(plain, encrypted);

            return ciphertext{std::move(encrypted)};
        }

    private:
        seal::Encryptor encryptor_;
    };

    symmetric_homomorphic_encryptor::symmetric_homomorphic_encryptor(std::span<const std::byte> secret_key)
        : impl_{std::make_unique<impl>(secret_key)} {}

    symmetric_homomorphic_encryptor::symmetric_homomorphic_encryptor(
        symmetric_homomorphic_encryptor&&) noexcept = default;

    symmetric_homomorphic_encryptor::~symmetric_homomorphic_encryptor() = default;

    symmetric_homomorphic_encryptor& symmetric_homomorphic_encryptor::operator=(
        symmetric_homomorphic_encryptor&&) noexcept = default;

    abi::vector<std::byte> symmetric_homomorphic_encryptor::generate_secret_key() {
        return impl::generate_secret_key();
    }

    ciphertext symmetric_homomorphic_encryptor::process(std::span<const std::byte> buffer) const {
        return impl_->process(buffer);
    }

} // namespace glasssix
