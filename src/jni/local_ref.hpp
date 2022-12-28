#pragma once

#include "util.hpp"

#include <utility>

#include <jni.h>

namespace glasssix::jni {
    /// <summary>
    /// Represents the global reference for a java object.
    /// </summary>
    class local_ref {
    public:
        local_ref() noexcept;
        explicit local_ref(jobject obj);
        local_ref(jobject obj, bool takeOverOnly);
        local_ref(const local_ref& other);
        local_ref(local_ref&& other) noexcept;
        virtual ~local_ref();
        explicit operator bool() const noexcept;
        local_ref& operator=(const local_ref& right);
        local_ref& operator=(local_ref&& right) noexcept;
        jobject get() const noexcept;

    private:
        jobject ref_;
    };

    template <derived_from_jobject JObject>
    class local_ref_ex : public local_ref {
    public:
        local_ref_ex() noexcept = default;

        explicit local_ref_ex(JObject obj) : local_ref{obj} {}

        local_ref_ex(JObject obj, bool takeOverOnly) : local_ref{obj, takeOverOnly} {}

        local_ref_ex(const local_ref_ex& other) : local_ref{other} {}

        local_ref_ex(local_ref_ex&& other) noexcept : local_ref{std::move(other)} {}

        virtual ~local_ref_ex() = default;

        local_ref_ex& operator=(const local_ref_ex& right) {
            static_cast<local_ref&>(*this) = right;

            return *this;
        }

        local_ref_ex& operator=(local_ref_ex&& right) noexcept {
            static_cast<local_ref&>(*this) = std::move(right);

            return *this;
        }

        JObject get() const noexcept {
            return static_cast<JObject>(static_cast<const local_ref&>(*this).get());
        }
    };
} // namespace glasssix::jni
