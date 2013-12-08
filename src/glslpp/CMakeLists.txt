
include( ${glslcc_SOURCE_DIR}/build/CMakeCommon.txt )

set( SRCS
		glslppafx.cpp
		main.cpp
		preproc.cpp
		tokens.cpp
)

include_directories( ${glslcc_SOURCE_DIR}/src/glslpp )
include_directories( ${glslcc_SOURCE_DIR}/src/common )

add_executable( glslpp ${SRCS} ${HDRS} )

set_target_properties( glslpp PROPERTIES RUNTIME_OUTPUT_NAME_DEBUG glslpp_d )

# TODO: This should go into CMakeCommon.txt, I think. But for now, leave it here.
if (MSVC)
	set_target_properties( glslpp PROPERTIES COMPILE_FLAGS "/Yuglslppafx.h" )
	set_source_files_properties( glslppafx.cpp PROPERTIES COMPILE_FLAGS "/Ycglslppafx.h" )
endif(MSVC)
