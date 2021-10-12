cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 20)

set(FRAMEWORK_NAME "xalwart")
set(LIBRARY_NAME "${FRAMEWORK_NAME}.server")
set(LIBRARY_VERSION "0.0.0")

project(${LIBRARY_NAME})

set(ROOT_DIR /usr/local)
set(INCLUDE_DIR ${ROOT_DIR}/include)
set(LIB_DIR ${ROOT_DIR}/lib)

include_directories(${INCLUDE_DIR})
link_directories(${LIB_DIR})

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

target_link_libraries(${LIBRARY_NAME} PUBLIC ${FRAMEWORK_NAME}.base)

# Install library.
install(
    TARGETS ${LIBRARY_NAME}
    ARCHIVE DESTINATION ${LIB_DIR}
    LIBRARY DESTINATION ${LIB_DIR}
)

install(
    DIRECTORY "${LIB_SRC_DIR}/"
    DESTINATION ${INCLUDE_DIR}/${LIBRARY_NAME}
    FILES_MATCHING
    PATTERN "*.h"
)
