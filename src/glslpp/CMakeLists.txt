
include( ${glslcc_SOURCE_DIR}/build/CMakeCommon.txt )

find_package(FLEX)
find_package(BISON)

FLEX_TARGET(glslpp ${glslcc_SOURCE_DIR}/src/grammar/glslpp.l ${CMAKE_CURRENT_BINARY_DIR}/tokenspp.cpp COMPILE_FLAGS "-d")
BISON_TARGET(glslpp ${glslcc_SOURCE_DIR}/src/grammar/glslpp.y ${CMAKE_CURRENT_BINARY_DIR}/parserpp.cpp COMPILE_FLAGS "-t -v")

ADD_FLEX_BISON_DEPENDENCY(glslpp glslpp)

set( SRCS
		glslppafx.cpp
		main.cpp
		preproc.cpp
		tokens.cpp
)

include_directories( ${glslcc_SOURCE_DIR}/src/glslpp )
include_directories( ${glslcc_SOURCE_DIR}/src/common )

add_executable( glslpp ${SRCS} ${BISON_glslpp_OUTPUTS} ${FLEX_glslpp_OUTPUTS} ${HDRS} )

set_target_properties( glslpp PROPERTIES RUNTIME_OUTPUT_NAME_DEBUG glslpp_d )

# TODO: This should go into CMakeCommon.txt, I think. But for now, leave it here.
if (MSVC)
	set_target_properties( glslpp PROPERTIES COMPILE_FLAGS "/Yuglslppafx.h" )
	set_source_files_properties( glslppafx.cpp PROPERTIES COMPILE_FLAGS "/Ycglslppafx.h" )
endif(MSVC)
