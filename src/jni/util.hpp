#pragma once

#include <concepts>
#include <cstddef>
#include <span>
#include <string>
#include <string_view>
#include <type_traits>

#include <jni.h>

namespace glasssix::jni {
    std::string to_string(jstring str);

    constexpr jboolean from_boolean(bool value) noexcept {
        return value ? JNI_TRUE : JNI_FALSE;
    }

    constexpr bool to_boolean(jboolean value) noexcept {
        return value;
    }

    template <typename T>
    concept derived_from_jobject =
        std::is_pointer_v<T> && std::is_base_of_v<std::remove_pointer_t<jobject>, std::remove_pointer_t<T>>;

    jstring make_string(std::string_view str);
    jbyteArray make_byte_array(std::span<const std::uint8_t> buffer);
    jfloatArray make_float_array(std::span<const float> buffer);

    jint throw_exception(jclass exception_type, const char* message);
} // namespace glasssix::jni
