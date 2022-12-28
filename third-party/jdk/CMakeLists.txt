# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.14)

project(jdk CXX)

if(NOT GX_JDK_ROOT)
    message(FATAL_ERROR "GX_JDK_ROOT not found.")
endif()

if(NOT GX_MISCELLANEOUS_ROOT)
    message(FATAL_ERROR "GX_MISCELLANEOUS_ROOT not found.")
endif()

set(target_version 1.8.0)
set(target_name jdk)
set(package_name Jdk)
set(cmake_module_dir ${GX_MISCELLANEOUS_ROOT}/cmake)

add_library(${target_name} INTERFACE)

target_include_directories(
	${target_name}
	INTERFACE
		$<INSTALL_INTERFACE:${GX_JDK_ROOT}/include>
		$<INSTALL_INTERFACE:$<IF:$<BOOL:${WIN32}>,${GX_JDK_ROOT}/include/win32,${GX_JDK_ROOT}/include/linux>>
)

include(${cmake_module_dir}/GXUtil.cmake)
include(${cmake_module_dir}/GXPackaging.cmake)

gx_make_install_package(
	INTERFACE
	TARGET_NAME ${target_name}
	PACKAGE_NAME ${package_name}
	TARGET_VERSION ${target_version}
)
