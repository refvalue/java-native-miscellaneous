#pragma once

#include <compare>
#include <concepts>
#include <functional>
#include <type_traits>

#include <g6/hashing.hpp>

#include <jni.h>

namespace glasssix::jni {
    struct cache_key {
        std::int32_t class_part;
        std::int32_t field_part;
        std::int32_t method_part;

        auto operator<=>(const cache_key&) const noexcept = default;
    };
} // namespace glasssix::jni

namespace glasssix::jni {
    template <auto Enum>
        requires std::is_enum_v<decltype(Enum)>
    inline constexpr auto arg_enum_v = static_cast<std::underlying_type_t<decltype(Enum)>>(Enum);

    template <typename T>
    constexpr auto make_cache_key(std::int32_t key) noexcept {
        if constexpr (std::same_as<T, jclass>) {
            return cache_key{key};
        } else if constexpr (std::same_as<T, jfieldID>) {
            return cache_key{0, key};
        } else if constexpr (std::same_as<T, jmethodID>) {
            return cache_key{0, 0, key};
        } else {
            return cache_key{};
        }
    }
} // namespace glasssix::jni

namespace std {
    template <>
    struct hash<glasssix::jni::cache_key> {
        auto operator()(const glasssix::jni::cache_key& item) const noexcept {
            return glasssix::hash_arbitrary(item.class_part, item.field_part, item.method_part);
        }
    };
} // namespace std
