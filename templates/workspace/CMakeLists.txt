cmake_minimum_required(VERSION 3.24)

project(UmicomWorkspace VERSION 0.4.0 LANGUAGES C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

option(UMICOM_ENABLE_STRICT_WARNINGS "Enable strict compiler warnings" ON)
option(UMICOM_ENABLE_SANITIZERS "Enable AddressSanitizer and UndefinedBehaviourSanitizer" OFF)
option(UMICOM_BUILD_GTK4 "Build GTK4 GUI wizard and generated GTK frontends" ON)

include(CTest)

function(umicom_apply_warnings target)
    if(NOT UMICOM_ENABLE_STRICT_WARNINGS)
        return()
    endif()
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4)
    else()
        target_compile_options(${target} PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow
        )
    endif()
endfunction()

function(umicom_apply_sanitizers target)
    if(UMICOM_ENABLE_SANITIZERS AND NOT MSVC)
        target_compile_options(${target} PRIVATE
            -fsanitize=address,undefined -fno-omit-frame-pointer
        )
        target_link_options(${target} PRIVATE
            -fsanitize=address,undefined
        )
    endif()
endfunction()

if(UMICOM_BUILD_GTK4)
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(GTK4 QUIET IMPORTED_TARGET gtk4)
    endif()
endif()

add_subdirectory(framework)
add_subdirectory(tools/umicom)

# Applications are discovered by their own CMakeLists.txt files.
file(GLOB application_children CONFIGURE_DEPENDS
    RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/applications"
    "${CMAKE_CURRENT_SOURCE_DIR}/applications/*"
)
foreach(application_child IN LISTS application_children)
    if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/applications/${application_child}"
       AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/applications/${application_child}/CMakeLists.txt")
        add_subdirectory("applications/${application_child}")
    endif()
endforeach()
