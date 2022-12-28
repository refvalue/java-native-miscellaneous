#include "entry.hpp"
#include "jni/local_ref.hpp"
#include "jni/util.hpp"
#include "process.hpp"

#include <jni.h>

using namespace glasssix;
using namespace glasssix::jni;

extern "C" {
/*
 * Class:     com_glasssix_Process
 * Method:    startSync
 * Signature: (Ljava/lang/String;Lcom/glasssix/IStdoutCallback;)I
 */
JNIEXPORT jint JNICALL Java_com_glasssix_Process_startSync(
    JNIEnv* env, jclass clazz, jstring cmdline, jobject callback) try {
    return start_process(to_string(cmdline), [&](std::string_view content) {
        env->CallVoidMethod(
            callback, jni_classes::i_stdout_callback::run, local_ref_ex<jstring>{make_string(content), true}.get());
    });

} catch (const std::exception& ex) {
    return throw_exception(jni_classes::exception::self.get(), ex.what());
}
}
