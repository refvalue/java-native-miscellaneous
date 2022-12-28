#pragma once

#include "global_ref.hpp"

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>

#include <jni.h>

namespace glasssix::jni {
    namespace detail {
        template <typename T>
        struct is_enum_tuple : std::false_type {};

        template <typename T, typename... Args>
            requires std::is_enum_v<T> || std::integral<T>
        struct is_enum_tuple<std::tuple<T, Args...>> : std::true_type {};

        template <typename T>
        concept enum_tuple = is_enum_tuple<std::decay_t<T>>::value;

        template <enum_tuple Tuple>
        constexpr auto from_enum_tuple(const Tuple& source) {
            return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
                return std::tuple{static_cast<std::int32_t>(std::get<0>(source)), std::get<(Is + 1)>(source)...};
            }
            (std::make_index_sequence<std::tuple_size_v<Tuple> - 1>{});
        }
    } // namespace detail

    struct java_vm_context {
        JavaVM* vm;
        std::int32_t version;
    };

    class reflector {
    public:
        using tuple_type = std::tuple<std::int32_t, std::string_view, std::string_view>;

        reflector(reflector&&) noexcept = delete;
        ~reflector();
        reflector& operator=(reflector&&) noexcept = delete;
        static const reflector& instance();
        std::shared_ptr<java_vm_context> init(JavaVM* vm) const;
        JNIEnv* get_thread_env() const;
        std::shared_ptr<java_vm_context> context() const;
        global_ref_ex<jclass> get_class_cache(std::int32_t key) const;
        jfieldID get_field_cache(std::int32_t key) const;
        jmethodID get_method_cache(std::int32_t key) const;
        global_ref_ex<jclass> add_class_cache(std::int32_t key, std::string_view name) const;
        jfieldID add_field_cache(std::int32_t class_key, const tuple_type& field) const;
        jmethodID add_method_cache(std::int32_t class_key, const tuple_type& method) const;
        jmethodID add_static_method_cache(std::int32_t class_key, const tuple_type& method) const;

        template <typename T, detail::enum_tuple Tuple>
            requires std::is_enum_v<T>
        jfieldID add_field_cache(T class_key, const Tuple& field) const {
            return add_field_cache(static_cast<std::int32_t>(class_key), detail::from_enum_tuple(field));
        }

        template <typename T, detail::enum_tuple Tuple>
            requires std::is_enum_v<T>
        jmethodID add_method_cache(T class_key, const Tuple& method) const {
            return add_method_cache(static_cast<std::int32_t>(class_key), detail::from_enum_tuple(method));
        }

        template <typename T, detail::enum_tuple Tuple>
            requires std::is_enum_v<T>
        jmethodID add_static_method_cache(T class_key, const Tuple& method) const {
            return add_static_method_cache(static_cast<std::int32_t>(class_key), detail::from_enum_tuple(method));
        }

        template <typename T, detail::enum_tuple... Args>
            requires std::is_enum_v<T> || std::integral<T>
        auto add_field_caches(T class_key, Args&&... fields) const {
            return std::tuple{add_field_cache(class_key, std::forward<Args>(fields))...};
        }

        template <typename T, detail::enum_tuple... Args>
            requires std::is_enum_v<T> || std::integral<T>
        auto add_method_caches(T class_key, Args&&... methods) const {
            return std::tuple{add_method_cache(class_key, std::forward<Args>(methods))...};
        }

        template <typename T, detail::enum_tuple... Args>
            requires std::is_enum_v<T> || std::integral<T>
        auto add_static_method_caches(T class_key, Args&&... methods) const {
            return std::tuple{add_static_method_cache(class_key, std::forward<Args>(methods))...};
        }

    private:
        reflector();

        class impl;

        std::unique_ptr<impl> impl_;
    };
} // namespace glasssix::jni
