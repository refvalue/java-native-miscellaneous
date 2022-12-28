#pragma once

#include "util.hpp"

#include <utility>

#include <jni.h>

namespace glasssix::jni {
    /// <summary>
    /// Represents the global reference for a java object.
    /// </summary>
    class global_ref {
    public:
        global_ref() noexcept;
        explicit global_ref(jobject obj);
        global_ref(jobject obj, bool takeOverOnly);
        global_ref(const global_ref& other);
        global_ref(global_ref&& other) noexcept;
        virtual ~global_ref();
        explicit operator bool() const noexcept;
        global_ref& operator=(const global_ref& right);
        global_ref& operator=(global_ref&& right) noexcept;
        jobject get() const noexcept;

    private:
        jobject ref_;
    };

    template <derived_from_jobject JObject>
    class global_ref_ex : public global_ref {
    public:
        global_ref_ex() noexcept = default;

        explicit global_ref_ex(JObject obj) : global_ref{obj} {}

        global_ref_ex(JObject obj, bool takeOverOnly) : global_ref{obj, takeOverOnly} {}

        global_ref_ex(const global_ref_ex& other) : global_ref{other} {}

        global_ref_ex(global_ref_ex&& other) noexcept : global_ref{std::move(other)} {}

        virtual ~global_ref_ex() = default;

        global_ref_ex& operator=(const global_ref_ex& right) {
            static_cast<global_ref&>(*this) = right;

            return *this;
        }

        global_ref_ex& operator=(global_ref_ex&& right) noexcept {
            static_cast<global_ref&>(*this) = std::move(right);

            return *this;
        }

        JObject get() const noexcept {
            return static_cast<JObject>(static_cast<const global_ref&>(*this).get());
        }
    };
} // namespace glasssix::jni
