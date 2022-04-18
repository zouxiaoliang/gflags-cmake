cmake_minimum_required(VERSION 3.5)

project(gflags-cmake LANGUAGES CXX C)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD_REQUIRED ON)

if (CMAKE_SYSTEM_PROCESSOR STREQUAL "AMD64")
    set(TARGET_PROCESSOR "x86_64")
elseif (CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
    set(TARGET_PROCESSOR "x86_64")
elseif (CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")
    set(TARGET_PROCESSOR "aarch64")
elseif (CMAKE_SYSTEM_PROCESSOR STREQUAL "arm64")
    set(TARGET_PROCESSOR "arm64")
else()
    message(FATAL_ERROR "Unsupported architecture ${CMAKE_HOST_SYSTEM_PROCESSOR}")
endif()

if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
    set(PLATFORM_POSIX 1)
    set(PLATFORM_LINUX 1)
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
    set(PLATFORM_POSIX 1)
    set(PLATFORM_MACOS 1)
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
    set(PLATFORM_WINDOWS 1)
else()
endif()

function(gflagsMain)
    set(library_root "${CMAKE_CURRENT_SOURCE_DIR}/gflags")

    set(sources
        "${CMAKE_CURRENT_SOURCE_DIR}/flags.cc"
        "${CMAKE_CURRENT_SOURCE_DIR}/flagalias.cc"
        "${library_root}/src/gflags.cc"
        "${library_root}/src/gflags_reporting.cc"
        "${library_root}/src/gflags_completions.cc"
        )

    add_library(thirdparty_gflags ${sources})

    if(DEFINED PLATFORM_LINUX)
        target_include_directories(thirdparty_gflags
            PRIVATE
            "${CMAKE_CURRENT_SOURCE_DIR}/generated/linux/private"
            "${CMAKE_CURRENT_SOURCE_DIR}/generated/linux/public"

            SYSTEM INTERFACE
            "${CMAKE_CURRENT_SOURCE_DIR}/generated/linux/public"
            )
    elseif(DEFINED PLATFORM_WINDOWS)
        list(APPEND sources
            "${library_root}/src/windows_port.cc"
            )

        target_include_directories(thirdparty_gflags
            PRIVATE
            "${CMAKE_CURRENT_SOURCE_DIR}/generated/windows/private"
            "${CMAKE_CURRENT_SOURCE_DIR}/generated/windows/public"

            SYSTEM INTERFACE
            "${CMAKE_CURRENT_SOURCE_DIR}/generated/windows/public"
            )
    elseif(DEFINED PLATFORM_MACOS)
        target_include_directories(thirdparty_gflags
            PRIVATE
            "${CMAKE_CURRENT_SOURCE_DIR}/generated/darwin/private"
            "${CMAKE_CURRENT_SOURCE_DIR}/generated/darwin/public"

            SYSTEM INTERFACE
            "${CMAKE_CURRENT_SOURCE_DIR}/generated/darwin/public"
            )
    endif()

    target_compile_definitions(thirdparty_gflags
        PUBLIC
        GFLAGS_IS_A_DLL=0
        GFLAGS_DLL_DECLARE_FLAG=
        GFLAGS_DLL_DEFINE_FLAG=
        NDEBUG
        )

    target_include_directories(thirdparty_gflags
        PRIVATE
        "${library_root}/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/generated"

        SYSTEM INTERFACE
        "${CMAKE_CURRENT_SOURCE_DIR}/generated"
        "${CMAKE_CURRENT_SOURCE_DIR}/"
        )

    target_link_libraries(thirdparty_gflags PRIVATE

        )

endfunction()

gflagsMain()
