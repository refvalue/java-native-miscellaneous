#pragma once

#include "jni/global_ref.hpp"

#include <jni.h>

namespace glasssix {
    struct jni_classes {
        struct exception {
            inline static jni::global_ref_ex<jclass> self;
        };

        struct process {
            inline static jni::global_ref_ex<jclass> self;
            inline static jmethodID start_sync;
        };

        struct i_stdout_callback {
            inline static jni::global_ref_ex<jclass> self;
            inline static jmethodID run;
        };
    };

} // namespace glasssix::face
