
find_path( XOSHIRO_INCLUDE_DIRS "xoshiro.h"
        PATHS "${CMAKE_CURRENT_SOURCE_DIR}"
        NO_DEFAULT_PATH )
find_path( XOSHIRO_SOURCE_DIR "xoshiro.c"
        PATHS "${CMAKE_CURRENT_SOURCE_DIR}"
        NO_DEFAULT_PATH )
mark_as_advanced( XOSHIRO_INCLUDE_DIRS )
mark_as_advanced( XOSHIRO_SOURCE_DIR )

set( XOSHIRO_HEADERS
        "${CMAKE_CURRENT_SOURCE_DIR}/xoshiro.h"
        CACHE INTERNAL "xoshiro headers" )
set( XOSHIRO_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/xoshiro.c"
        CACHE INTERNAL "xoshiro sources" )