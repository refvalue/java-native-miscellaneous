set(target_version 1.0.0)
set(target_name gx-jni)

add_library(${target_name})

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
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(
	${target_name}
	PRIVATE
	Threads::Threads
	GXMiscellaneous::gx-miscellaneous
	Jdk::jdk
)

set_target_properties(${target_name}
	PROPERTIES
	CXX_STANDARD 20
	CXX_STANDARD_REQUIRED ON
)
