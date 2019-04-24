# Skeleton code will have tons of unused variables
IF (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-reorder -Wno-unknown-pragmas -Wno-unused-variable -Wno-unused-but-set-variable")
ENDIF ()

IF (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	# Clang complains more about GLU and overloading functions
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations -Wno-overloaded-virtual")
ENDIF ()
