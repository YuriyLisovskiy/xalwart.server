cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 20)

set(FRAMEWORK_NAME "xalwart")
set(LIBRARY_NAME "${FRAMEWORK_NAME}.server")
set(LIBRARY_VERSION "0.0.0")

project(${LIBRARY_NAME})

set(
    DEFAULT_INCLUDE_PATHS
        "/usr/local"
        "/usr"
)

foreach(ENTRY ${DEFAULT_INCLUDE_PATHS})
    include_directories(${ENTRY}/include)
    link_directories(${ENTRY}/lib)
endforeach()

# Search for xalwart.base
find_library(
    XALWART_BASE xalwart.base REQUIRED
    PATHS ${DEFAULT_INCLUDE_PATHS}
)

function(print SRC_LIST)
    foreach(entry ${SRC_LIST})
        message(${entry})
    endforeach(entry)
endfunction(print)

set(LIB_SRC_DIR ${PROJECT_SOURCE_DIR}/src)
file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true ${LIB_SRC_DIR}/*.h ${LIB_SRC_DIR}/*.cpp)
foreach(entry ${SOURCES})
    if (IS_DIRECTORY ${entry})
        list(REMOVE_ITEM SOURCES ${entry})
    endif()
endforeach()
list(LENGTH SOURCES SOURCES_COUNT)
message(STATUS "[INFO] Sources found: ${SOURCES_COUNT}")
if(NOT ${SOURCES_COUNT})
    return()
endif()

# Build shared library.
add_library(${LIBRARY_NAME} ${SOURCES})

target_link_libraries(${LIBRARY_NAME} PUBLIC ${XALWART_BASE})

set(LIBRARY_ROOT /usr/local CACHE STRING "Installation root directory.")
set(LIBRARY_INCLUDE_DIR ${LIBRARY_ROOT}/include CACHE STRING "Include installation directory.")
set(LIBRARY_LINK_DIR ${LIBRARY_ROOT}/lib CACHE STRING "Library installation directory.")

# Install the library.
install(
    TARGETS ${LIBRARY_NAME}
    ARCHIVE DESTINATION ${LIBRARY_LINK_DIR}
    LIBRARY DESTINATION ${LIBRARY_LINK_DIR}
)
install(
    DIRECTORY "${LIB_SRC_DIR}/"
    DESTINATION ${LIBRARY_INCLUDE_DIR}/${LIBRARY_NAME}
    FILES_MATCHING
    PATTERN "*.h"
)
