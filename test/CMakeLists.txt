set(target_name gx-java-util-test)

add_executable(${target_name})

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

target_link_libraries(
	${target_name}
	PRIVATE
	gx-java-util
	Threads::Threads
	GXMiscellaneous::gx-miscellaneous
)

set_target_properties(${target_name}
	PROPERTIES
	CXX_STANDARD 20
	CXX_STANDARD_REQUIRED ON
)
