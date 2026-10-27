# Umicom Framework
# File: cmake/UmicomSanitizers.cmake
# Purpose: Check sanitizer availability and propagate required link runtimes.
# Author and organisation: Sammy Hegab, Umicom Foundation
# Licence: MIT
include_guard(GLOBAL)

# A flag accepted during compilation is insufficient: the executable must link
# both runtimes. This probe compiles and links, but never executes, a program.
function(umicom_check_sanitizers)
    if(NOT UMICOM_ENABLE_SANITIZERS)
        return()
    endif()
    if(MSVC OR NOT CMAKE_C_COMPILER_ID MATCHES "^(GNU|Clang|AppleClang)$")
        message(FATAL_ERROR
            "Umicom address/undefined sanitizers require a GNU-style GCC or Clang "
            "toolchain with both runtimes. This compiler is unsupported by this "
            "qualification mode. Use a supported separate build, or configure "
            "with UMICOM_ENABLE_SANITIZERS=OFF for an ordinary build.")
    endif()
    # Keep cached evidence tied to the compiler, platform and build flags.
    string(TOUPPER "${CMAKE_BUILD_TYPE}" configuration)
    set(configuration_flags "${CMAKE_C_FLAGS_${configuration}}|${CMAKE_EXE_LINKER_FLAGS_${configuration}}")
    string(SHA256 identity
        "${CMAKE_C_COMPILER}|${CMAKE_C_COMPILER_ID}|${CMAKE_C_COMPILER_VERSION}|${CMAKE_SYSTEM_NAME}|${CMAKE_SYSTEM_PROCESSOR}|${CMAKE_C_FLAGS}|${CMAKE_BUILD_TYPE}|${CMAKE_EXE_LINKER_FLAGS}|${configuration_flags}|${CMAKE_SYSROOT}|${CMAKE_C_COMPILER_TARGET}")
    set(result "UMICOM_SANITIZERS_LINK_${identity}")
    include(CMakePushCheckState)
    include(CheckCSourceCompiles)
    cmake_push_check_state(RESET)
    # Cross toolchains sometimes probe static libraries; require a real link.
    set(CMAKE_TRY_COMPILE_TARGET_TYPE EXECUTABLE)
    set(CMAKE_REQUIRED_FLAGS "-fsanitize=address,undefined -fno-omit-frame-pointer")
    set(CMAKE_REQUIRED_LINK_OPTIONS "-fsanitize=address,undefined")
    check_c_source_compiles([=[
#ifndef __has_feature
#define __has_feature(feature) 0
#endif
#if !defined(__SANITIZE_ADDRESS__) && !__has_feature(address_sanitizer)
#error AddressSanitizer instrumentation is not active
#endif
int main(void) {
    volatile int value = 1;
    return value - 1;
}
]=] ${result})
    cmake_pop_check_state()
    if(NOT ${result})
        # Installing the missing runtimes should permit a subsequent configure
        # without requiring the developer to discard unrelated cache settings.
        unset(${result} CACHE)
        message(FATAL_ERROR
            "Umicom sanitizer qualification cannot compile and link with this "
            "toolchain. Install matching address/undefined sanitizer runtimes or "
            "use a supported GCC/Clang host build. See CMake configure diagnostics. "
            "UMICOM_ENABLE_SANITIZERS=OFF selects an ordinary build; it does not "
            "provide sanitizer coverage.")
    endif()
endfunction()

function(umicom_apply_sanitizers target)
    if(NOT UMICOM_ENABLE_SANITIZERS)
        return()
    endif()
    if(NOT TARGET "${target}")
        message(FATAL_ERROR "Cannot instrument missing target '${target}'")
    endif()
    umicom_check_sanitizers()
    get_target_property(applied "${target}" UMICOM_SANITIZERS_APPLIED)
    if(applied)
        return()
    endif()
    get_target_property(kind "${target}" TYPE)
    if(kind STREQUAL "INTERFACE_LIBRARY")
        # An interface target owns no compiled sources.
        target_compile_options("${target}" INTERFACE
            -fsanitize=address,undefined -fno-omit-frame-pointer)
        target_link_options("${target}" INTERFACE -fsanitize=address,undefined)
    else()
        target_compile_options("${target}" PRIVATE
            -fsanitize=address,undefined -fno-omit-frame-pointer)
        if(kind STREQUAL "STATIC_LIBRARY" OR kind STREQUAL "OBJECT_LIBRARY")
            # Libraries with no link step must pass runtimes to their consumers.
            target_link_options("${target}" INTERFACE -fsanitize=address,undefined)
        else()
            target_link_options("${target}" PRIVATE -fsanitize=address,undefined)
        endif()
    endif()
    set_property(TARGET "${target}" PROPERTY UMICOM_SANITIZERS_APPLIED TRUE)
endfunction()
