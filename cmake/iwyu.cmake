cmake_minimum_required(VERSION 3.15)

option(USE_IWYU "Use include-what-you-use for static include analysis" OFF)

if (NOT USE_IWYU)
    message(STATUS "    Not using include-what-you-use! To enable it, append -DUSE_IWYU=ON")
    return()
endif()

message(STATUS "    Using include-what-you-use")

find_program(
    iwyu_exe
    NAMES include-what-you-use iwyu
)

if(NOT iwyu_exe)
    message(WARNING "    Could not find the program include-what-you-use, baling out...")
    return()
else()
    message(STATUS "    Using include-what-you-use include analysis.")
endif()
