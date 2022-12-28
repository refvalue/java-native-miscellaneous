#include "process.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <locale>
#include <memory>
#include <span>
#include <stdexcept>
#include <system_error>
#include <vector>

#include <g6/encoding.hpp>
#include <g6/functional.hpp>
#include <g6/scope.hpp>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#ifndef NOGDI
#define NOGDI
#endif
#include <Windows.h>
#endif

namespace glasssix {
    namespace {
        struct force_init {
            force_init() {
                std::locale::global(std::locale{"en_US.UTF8"});
            }
        } init;

        using handle_ptr = std::unique_ptr<void, decltype([](HANDLE inner) { CloseHandle(inner); })>;

        void check_last_error(BOOL value) {
            if (!value) {
                throw std::system_error{static_cast<std::int32_t>(GetLastError()), std::system_category()};
            }
        }

        auto make_startup_info_reserved2(std::span<const HANDLE> handles) {
            static constexpr std::byte flags_fopen{0x01};
            static constexpr std::byte flags_fdev{0x40};
            static constexpr auto file_info = flags_fopen | flags_fdev;

            std::vector<std::byte> buffer(
                sizeof(std::uint32_t) + handles.size() * (sizeof(std::uint8_t) + sizeof(HANDLE)));
            auto iter = buffer.data();
            auto size = static_cast<std::uint32_t>(handles.size());

            iter = std::copy(reinterpret_cast<const std::byte*>(&size),
                reinterpret_cast<const std::byte*>(&size) + sizeof(size), iter);

            iter = std::fill_n(iter, handles.size(), file_info);

            auto handles_bytes = std::as_bytes(handles);

            iter = std::copy(handles_bytes.begin(), handles_bytes.end(), iter);

            return buffer;
        }

        auto make_proc_thread_attribute_list(std::span<const HANDLE> handles) {
            std::size_t size{};

            // https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-initializeprocthreadattributelist
            // This initial call will return an error by design. This is expected behavior.
            static_cast<void>(InitializeProcThreadAttributeList(nullptr, 1, 0, &size));

            std::unique_ptr<std::remove_pointer_t<LPPROC_THREAD_ATTRIBUTE_LIST>,
                decltype([](LPPROC_THREAD_ATTRIBUTE_LIST inner) {
                    DeleteProcThreadAttributeList(inner);
                    std::default_delete<std::byte[]>{}(reinterpret_cast<std::byte*>(inner));
                })>
                list{reinterpret_cast<LPPROC_THREAD_ATTRIBUTE_LIST>(new std::byte[size])};

            check_last_error(InitializeProcThreadAttributeList(list.get(), 1, 0, &size));
            check_last_error(UpdateProcThreadAttribute(list.get(), 0, PROC_THREAD_ATTRIBUTE_HANDLE_LIST,
                const_cast<HANDLE*>(handles.data()), handles.size_bytes(), nullptr, nullptr));

            return list;
        }

        auto create_anonymous_pipe(bool inherit) {
            HANDLE read{};
            HANDLE write{};
            SECURITY_ATTRIBUTES sa_attr{.nLength{sizeof(SECURITY_ATTRIBUTES)}, .bInheritHandle{inherit}};

            check_last_error(CreatePipe(&read, &write, &sa_attr, 0));

            return std::array{handle_ptr{read}, handle_ptr{write}};
        }
    } // namespace

    std::int32_t start_process(
        std::string_view cmd_line, const std::function<void(std::string_view content)>& stdout_callback) {
        auto&& [stdin_read, stdin_write]   = create_anonymous_pipe(true);
        auto&& [stdout_read, stdout_write] = create_anonymous_pipe(true);

        check_last_error(SetHandleInformation(stdin_write.get(), HANDLE_FLAG_INHERIT, 0));
        check_last_error(SetHandleInformation(stdout_read.get(), HANDLE_FLAG_INHERIT, 0));

        PROCESS_INFORMATION process_info{};

        {
            const std::array pipes{stdin_read.get(), stdout_write.get()};
            auto attribute_list = make_proc_thread_attribute_list(pipes);
            auto reserved2      = make_startup_info_reserved2(pipes);

            STARTUPINFOEXW start_info_ex{.StartupInfo{.cb{sizeof(STARTUPINFOEXW)},
                                             .cbReserved2{static_cast<WORD>(reserved2.size())},
                                             .lpReserved2{reinterpret_cast<LPBYTE>(reserved2.data())}},
                .lpAttributeList{attribute_list.get()}};

            auto native_cmd_line = to_native_string(cmd_line);
            scope_exit stdin_read_scope{[&] { stdin_read.reset(); }};
            scope_exit stdout_write_scope{[&] { stdout_write.reset(); }};

            check_last_error(CreateProcessW(nullptr, native_cmd_line.data(), nullptr, nullptr, TRUE,
                CREATE_NO_WINDOW | EXTENDED_STARTUPINFO_PRESENT, nullptr, nullptr, &start_info_ex.StartupInfo,
                &process_info));
        }

        scope_exit thread_scope{[&] { CloseHandle(process_info.hThread); }};
        scope_exit process_scope{[&] { CloseHandle(process_info.hProcess); }};

        DWORD bytes_read{};
        thread_local std::array<char, 4096> buffer;

        while (ReadFile(stdout_read.get(), buffer.data(), static_cast<DWORD>(buffer.size()), &bytes_read, nullptr)
               && bytes_read != 0) {
            invoke_optional(stdout_callback, std::string(buffer.data(), bytes_read));
        }

        WaitForSingleObject(process_info.hProcess, INFINITE);

        DWORD exit_code{};

        check_last_error(GetExitCodeProcess(process_info.hProcess, &exit_code));

        return exit_code;
    }
} // namespace glasssix
