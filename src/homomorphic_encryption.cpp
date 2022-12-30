#include "homomorphic_encryption.hpp"

namespace glasssix {
    const seal::SEALContext& get_default_seal_context() {
        static constexpr std::size_t poly_modulus_degree{8192};
        static const seal::SEALContext seal_context{[] {
            seal::EncryptionParameters params{seal::scheme_type::bfv};

            params.set_poly_modulus_degree(poly_modulus_degree);
            params.set_coeff_modulus(seal::CoeffModulus::BFVDefault(poly_modulus_degree));
            params.set_plain_modulus(seal::PlainModulus::Batching(poly_modulus_degree, 20));

            return params;
        }()};

        return seal_context;
    }

    namespace {
        seal::SecretKey load_secret_key(std::span<const std::byte> secret_key) {
            seal::SecretKey result;

            result.load(get_default_seal_context(), reinterpret_cast<const seal::seal_byte*>(secret_key.data()),
                secret_key.size());

            return result;
        }
    } // namespace

    const seal::Evaluator& get_default_seal_evaluator() {
        thread_local const seal::Evaluator evaluator{get_default_seal_context()};

        return evaluator;
    }

    seal::SecretKey load_seal_secret_key(std::span<const std::byte> secret_key) {
        seal::SecretKey result;

        result.load(
            get_default_seal_context(), reinterpret_cast<const seal::seal_byte*>(secret_key.data()), secret_key.size());

        return result;
    }

    abi::vector<std::byte> generate_seal_secret_key(const seal::SEALContext& context) {
        seal::KeyGenerator key_generator{get_default_seal_context()};
        abi::vector<std::byte> buffer(key_generator.secret_key().save_size());

        key_generator.secret_key().save(reinterpret_cast<seal::seal_byte*>(buffer.data()), buffer.size());

        return buffer;
    }
} // namespace glasssix
