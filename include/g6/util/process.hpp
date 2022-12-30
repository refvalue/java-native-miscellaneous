#pragma once

#include <cstdint>
#include <functional>
#include <string_view>

#include <g6/compat.hpp>

namespace glasssix {
    GX_API(JAVAUTIL) std::int32_t start_process(
        std::string_view cmd_line, const std::function<void(std::string_view content)>& stdout_callback);
}
