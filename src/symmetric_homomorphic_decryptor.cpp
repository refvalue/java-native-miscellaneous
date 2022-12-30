#include "symmetric_homomorphic_decryptor.hpp"

#include "homomorphic_encryption.hpp"

#include <algorithm>
#include <cstddef>
#include <stdexcept>

#include <g6/char8_t_remediation.hpp>
#include <g6/cryptography.hpp>

#include <seal/seal.h>

namespace glasssix {
    class symmetric_homomorphic_decryptor::impl {
    public:
        impl(std::span<const std::byte> secret_key)
            : decryptor_{get_default_seal_context(), load_seal_secret_key(secret_key)} {}

        static abi::vector<std::byte> generate_secret_key() {
            return generate_seal_secret_key(get_default_seal_context());
        }

        abi::vector<std::byte> process(const ciphertext& encrypted) {
            abi::vector<std::byte> result;

            process_impl(encrypted, result);

            return result;
        }

        abi::string process_as_string(const ciphertext& encrypted) {
            abi::string result;

            process_impl(encrypted, result);

            return result;
        }

    private:
        template <typename Container>
        void process_impl(const ciphertext& encrypted, Container& result) {
            seal::Plaintext plain;

            decryptor_.decrypt(encrypted.get_impl(), plain);
            result.resize(plain.coeff_count());
            
            std::transform(plain.data(), plain.data() + plain.coeff_count(), result.begin(),
                [](std::uint64_t inner) { return static_cast<typename Container::value_type>(inner); });
        }

        seal::Decryptor decryptor_;
    };

    symmetric_homomorphic_decryptor::symmetric_homomorphic_decryptor(std::span<const std::byte> secret_key)
        : impl_{std::make_unique<impl>(secret_key)} {}

    symmetric_homomorphic_decryptor::symmetric_homomorphic_decryptor(
        symmetric_homomorphic_decryptor&&) noexcept = default;

    symmetric_homomorphic_decryptor::~symmetric_homomorphic_decryptor() = default;

    symmetric_homomorphic_decryptor& symmetric_homomorphic_decryptor::operator=(
        symmetric_homomorphic_decryptor&&) noexcept = default;

    abi::vector<std::byte> symmetric_homomorphic_decryptor::process(const ciphertext& encrypted) const {
        return impl_->process(encrypted);
    }

    abi::string symmetric_homomorphic_decryptor::process_as_string(const ciphertext& encrypted) const {
        return impl_->process_as_string(encrypted);
    }

} // namespace glasssix
