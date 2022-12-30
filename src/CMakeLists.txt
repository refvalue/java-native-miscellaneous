set(target_version 1.0.0)
set(target_name gx-java-util)
set(package_name GXJavaUtil)

add_library(${target_name} SHARED)

file(
	GLOB private_sources
	CONFIGURE_DEPENDS
	*.hpp
	*.cpp
)

target_sources(
	${target_name}
	PRIVATE ${private_sources}
)

if(NOT "${GX_TOOLHAIN_TARGET_NAME}" STREQUAL "")
	include(GXCompat)
	gx_patch_compat(
		TARGET ${target_name}
		TOOLCHAIN_TARGET_NAME ${GX_TOOLHAIN_TARGET_NAME}
	)
endif()

target_include_directories(
	${target_name}
	INTERFACE
		$<BUILD_INTERFACE:${GX_PUBLIC_INCLUDE_DIR}>
		$<INSTALL_INTERFACE:include>
	PRIVATE ${GX_PUBLIC_INCLUDE_DIR}/g6/util
)

target_link_libraries(
	${target_name}
	PRIVATE
	Threads::Threads
	GXMiscellaneous::gx-miscellaneous
	SEAL::seal
	gx-jni
	Jdk::jdk
)

target_compile_definitions(${target_name} PRIVATE GX_IS_JAVAUTIL_IMPL=1)

set_target_properties(${target_name}
	PROPERTIES
	CXX_STANDARD 20
	CXX_STANDARD_REQUIRED ON
)

include(GXPackaging)

gx_make_install_package(
	TARGET_NAME ${target_name}
	PACKAGE_NAME ${package_name}
	TARGET_VERSION ${target_version}
	PATH_VARS CMAKE_INSTALL_PREFIX
)
