#include "entry.hpp"

#include "jni/cache_key.hpp"
#include "jni/reflector.hpp"
#include "jni/util.hpp"

#include <jni.h>

using namespace glasssix::jni;

namespace {
    auto&& current_reflector = reflector::instance();
} // namespace

using namespace glasssix;

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void*) {
    auto context = current_reflector.init(vm);

    if (!context) {
        return JNI_ERR;
    }

    using exception         = jni_classes::exception;
    using process           = jni_classes::process;
    using i_stdout_callback = jni_classes::i_stdout_callback;

    std::int32_t class_key{};
    std::int32_t method_key{};

    exception::self = current_reflector.add_class_cache(class_key, "java/lang/Exception");

    class_key++;
    method_key = 0;

    process::self                 = current_reflector.add_class_cache(class_key, "com/glasssix/Process");
    std::tie(process::start_sync) = current_reflector.add_static_method_caches(
        class_key, std::tuple{method_key++, "startSync", "(Ljava/lang/String;Lcom/glasssix/IStdoutCallback;)I"});

    class_key++;
    method_key = 0;

    i_stdout_callback::self = current_reflector.add_class_cache(class_key, "com/glasssix/IStdoutCallback");
    std::tie(i_stdout_callback::run) =
        current_reflector.add_method_caches(class_key, std::tuple{method_key++, "run", "(Ljava/lang/String;)V"});

    class_key++;
    method_key = 0;

    return context->version;
}
