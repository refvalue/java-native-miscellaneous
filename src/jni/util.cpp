#include "util.hpp"

#include "reflector.hpp"
#include "util.hpp"

#include <algorithm>
#include <cstddef>
#include <vector>

#include <g6/encoding.hpp>
#include <g6/scope.hpp>

namespace glasssix::jni {
    std::string to_string(jstring str) {
        auto env = reflector::instance().get_thread_env();

        return env && str ? std::string{env->GetStringUTFChars(str, nullptr),
                   static_cast<std::size_t>(env->GetStringUTFLength(str))}
                          : std::string{};
    }

    jstring make_string(std::string_view str) {
        if (auto env = reflector::instance().get_thread_env()) {
            auto wide_str = to_native_string(str);
            std::vector<jchar> buffer{wide_str.begin(), wide_str.end()};

            return env->NewString(buffer.data(), static_cast<jsize>(buffer.size()));
        }

        return nullptr;
    }

    jbyteArray make_byte_array(std::span<const std::uint8_t> buffer) {
        if (auto env = reflector::instance().get_thread_env()) {
            auto array    = env->NewByteArray(static_cast<jsize>(buffer.size()));
            auto elements = env->GetByteArrayElements(array, nullptr);
            scope_exit elements_scope{[&] { env->ReleaseByteArrayElements(array, elements, 0); }};

            std::copy(buffer.data(), buffer.data() + buffer.size(), elements);

            return array;
        }

        return nullptr;
    }

    jfloatArray make_float_array(std::span<const float> buffer) {
        if (auto env = reflector::instance().get_thread_env()) {
            auto array    = env->NewFloatArray(static_cast<jsize>(buffer.size()));
            auto elements = env->GetFloatArrayElements(array, nullptr);
            scope_exit elements_scope{[&] { env->ReleaseFloatArrayElements(array, elements, 0); }};

            std::copy(buffer.data(), buffer.data() + buffer.size(), elements);

            return array;
        }

        return nullptr;
    }

    jint throw_exception(jclass exception_type, const char* message) {
        if (auto env = reflector::instance().get_thread_env()) {
            return env->ThrowNew(exception_type, message);
        }

        return -1;
    }
} // namespace glasssix::jni
