include(${GX_MISCELLANEOUS_ROOT}/cmake/GXThirdPartyLibrary.cmake)

if(GX_STATIC_RUNTIME)
    set(runtime_args STATIC_RUNTIME)
    set(misc_runtime_args CMAKE_ARGS -DGX_STATIC_RUNTIME=ON)
else()
    set(runtime_args "")
    set(misc_runtime_args CMAKE_ARGS -DGX_STATIC_RUNTIME=OFF)
endif()

if(CMAKE_CROSSCOMPILING)
    set(additional_args CMAKE_ARGS -DGX_TOOLCHAIN_PREFIX=${GX_TOOLCHAIN_PREFIX})
    set(toolchain_file ${GX_MISCELLANEOUS_ROOT}/cmake/Armv7Toolchain.cmake)
else()
    set(additional_args "")
    set(toolchain_file "")
endif()

find_package(Threads REQUIRED)

gx_make_install_third_party_library(
    GXMiscellaneous
	REQUIRED
    PARALLEL_BUILD
    SYNC_BUILD_TYPE
    GENERATOR ${CMAKE_GENERATOR}
    ${misc_runtime_args}
    ${additional_args}
    SOURCE_DIR ${GX_MISCELLANEOUS_ROOT}
    TOOLCHAIN_FILE ${toolchain_file}
)

gx_make_install_third_party_library(
    SEAL
	REQUIRED
    PARALLEL_BUILD
    SYNC_BUILD_TYPE
    GENERATOR ${CMAKE_GENERATOR}
    ${runtime_args}
    ${additional_args}
    SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/third-party/SEAL
    TOOLCHAIN_FILE ${toolchain_file}
)

gx_make_install_third_party_library(
    Jdk
	REQUIRED
    GENERATOR ${CMAKE_GENERATOR}
    ${additional_args}
    SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/third-party/jdk
    TOOLCHAIN_FILE ${toolchain_file}
    CMAKE_ARGS -DGX_JDK_ROOT=${GX_JDK_ROOT} -DGX_MISCELLANEOUS_ROOT=${GX_MISCELLANEOUS_ROOT}
)
