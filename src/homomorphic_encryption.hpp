#pragma once

#include <cstddef>
#include <span>

#include <g6/abi/vector.hpp>

#include <seal/seal.h>

namespace glasssix {
    const seal::SEALContext& get_default_seal_context();
    const seal::Evaluator& get_default_seal_evaluator();
    seal::SecretKey load_seal_secret_key(std::span<const std::byte> secret_key);
    abi::vector<std::byte> generate_seal_secret_key(const seal::SEALContext& context = get_default_seal_context());
} // namespace glasssix
