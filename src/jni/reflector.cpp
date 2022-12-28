#include "reflector.hpp"

#include "cache_key.hpp"
#include "global_ref.hpp"
#include "local_ref.hpp"

#include <array>
#include <atomic>
#include <unordered_map>
#include <variant>

namespace glasssix::jni {
    namespace {
        /// Some JNI functions may contain parameters that are void** or JNIEnv** across different JNI versions.
        /// We provides an adapter here to support auto-casting.
        /// </summary>
        struct env_adapter {
            JNIEnv* env;

            JNIEnv* get() const noexcept {
                return env;
            }

            operator JNIEnv**() noexcept {
                return &env;
            }

            operator void**() noexcept {
                return reinterpret_cast<void**>(&env);
            }
        };
    } // namespace

    class reflector::impl {
    public:
        impl() {}

        std::shared_ptr<java_vm_context> init(JavaVM* vm) {
            static constexpr std::array available_versions{
                JNI_VERSION_1_6, JNI_VERSION_1_4, JNI_VERSION_1_2, JNI_VERSION_1_1};

            env_adapter adapter;

            if (auto iter = std::find_if(available_versions.begin(), available_versions.end(),
                    [&](std::int32_t inner) { return vm->GetEnv(adapter, inner) == JNI_OK; });
                iter != available_versions.end()) {
                auto result = std::make_shared<java_vm_context>(java_vm_context{vm, *iter});

                context_.store(result, std::memory_order::release);

                return result;
            }

            return nullptr;
        }

        JNIEnv* get_thread_env() const {
            auto context = context_.load(std::memory_order::acquire);

            if (!context) {
                return nullptr;
            }

            thread_local bool attached_in_native_code = false;

            // Creates a thread-local JNIEnv* using a trick.
            thread_local auto creator = [context] {
                env_adapter env{nullptr};

                // Attaches the current thread if necessary.
                if (context && context->vm->GetEnv(env, context->version) == JNI_EDETACHED) {
                    return context->vm->AttachCurrentThread(env, nullptr) == JNI_OK
                             ? (attached_in_native_code = true, env.get())
                             : nullptr;
                }

                return env.get();
            };

            thread_local auto freeing_handler = [context](JNIEnv*) {
                if (context && attached_in_native_code) {
                    context->vm->DetachCurrentThread();
                }
            };

            thread_local std::unique_ptr<JNIEnv, decltype(freeing_handler)> result{creator(), freeing_handler};

            return result.get();
        }

        std::shared_ptr<java_vm_context> context() const {
            return context_.load(std::memory_order::acquire);
        }

        global_ref_ex<jclass> get_class_cache(std::int32_t key) const {
            return get_item_cache_internal<global_ref_ex<jclass>, jclass>(key);
        }

        jfieldID get_field_cache(std::int32_t key) const {
            return get_item_cache_internal<jfieldID>(key);
        }

        jmethodID get_method_cache(std::int32_t key) const {
            return get_item_cache_internal<jmethodID>(key);
        }

        global_ref_ex<jclass> add_class_cache(std::int32_t key, std::string_view name) {
            auto env = get_thread_env();

            if (env == nullptr) {
                return global_ref_ex<jclass>{nullptr};
            }

            // Finds the class.
            if (local_ref_ex<jclass> clazz{env->FindClass(name.data()), true}) {
                return std::get<global_ref_ex<jclass>>(
                    cache_.insert_or_assign(make_cache_key<jclass>(key), global_ref_ex<jclass>{clazz.get()})
                        .first->second);
            }

            return global_ref_ex<jclass>{nullptr};
        }

        jfieldID add_field_cache(std::int32_t class_key, const tuple_type& field) {
            if (auto env = get_thread_env()) {
                return add_item_cache_internal<jfieldID>(env, class_key, env->functions->GetFieldID, field);
            }

            return nullptr;
        }

        jmethodID add_method_cache(std::int32_t class_key, const tuple_type& method) {
            if (auto env = get_thread_env()) {
                return add_item_cache_internal<jmethodID>(env, class_key, env->functions->GetMethodID, method);
            }

            return nullptr;
        }

        jmethodID add_static_method_cache(std::int32_t class_key, const tuple_type& method) {
            if (auto env = get_thread_env()) {
                return add_item_cache_internal<jmethodID>(env, class_key, env->functions->GetStaticMethodID, method);
            }

            return nullptr;
        }

    private:
        template <std::constructible_from<std::nullptr_t> T, typename Category = T>
        T get_item_cache_internal(std::int32_t key) const {
            auto iter = cache_.find(make_cache_key<Category>(key));

            return iter != cache_.end() ? std::get<T>(iter->second) : T{nullptr};
        }

        template <std::constructible_from<std::nullptr_t> T>
        T add_item_cache_internal(JNIEnv* env, std::int32_t class_key,
            T(JNICALL* handler)(JNIEnv*, jclass, const char*, const char*), const tuple_type& item) {
            if (auto iter = cache_.find(make_cache_key<jclass>(class_key)); iter != cache_.end()) {
                auto clazz                    = std::get<global_ref_ex<jclass>>(iter->second);
                auto&& [key, name, signature] = item;

                if (auto id = handler(env, clazz.get(), name.data(), signature.data())) {
                    return std::get<T>(cache_.insert_or_assign(make_cache_key<T>(key), id).first->second);
                }
            }

            return T{nullptr};
        }

        std::atomic<std::shared_ptr<java_vm_context>> context_;
        std::unordered_map<cache_key, std::variant<global_ref_ex<jclass>, jfieldID, jmethodID>> cache_;
    };

    reflector::reflector() : impl_{std::make_unique<impl>()} {}

    reflector::~reflector() {}

    const reflector& reflector::instance() {
        static const reflector instance;

        return instance;
    }

    std::shared_ptr<java_vm_context> reflector::init(JavaVM* vm) const {
        return impl_->init(vm);
    }

    JNIEnv* reflector::get_thread_env() const {
        return impl_->get_thread_env();
    }

    std::shared_ptr<java_vm_context> reflector::context() const {
        return impl_->context();
    }

    global_ref_ex<jclass> reflector::get_class_cache(std::int32_t key) const {
        return impl_->get_class_cache(key);
    }

    jfieldID reflector::get_field_cache(std::int32_t key) const {
        return impl_->get_field_cache(key);
    }

    jmethodID reflector::get_method_cache(std::int32_t key) const {
        return impl_->get_method_cache(key);
    }

    global_ref_ex<jclass> reflector::add_class_cache(std::int32_t key, std::string_view name) const {
        return impl_->add_class_cache(key, name);
    }

    jfieldID reflector::add_field_cache(std::int32_t class_key, const tuple_type& field) const {
        return impl_->add_field_cache(class_key, field);
    }

    jmethodID reflector::add_method_cache(std::int32_t class_key, const tuple_type& method) const {
        return impl_->add_method_cache(class_key, method);
    }

    jmethodID reflector::add_static_method_cache(std::int32_t class_key, const tuple_type& method) const {
        return impl_->add_static_method_cache(class_key, method);
    }
} // namespace glasssix::jni
