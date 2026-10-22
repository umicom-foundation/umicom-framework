# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
cmake_minimum_required(VERSION 3.24)
project(UmicomReleaseBaseline LANGUAGES C)
include(CTest)
include(GNUInstallDirs)
find_package(Threads REQUIRED)
get_filename_component(_root "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
set(UMICOM_FRAMEWORK_SOURCE "${_root}" CACHE PATH "Canonical Framework source")
option(UMICOM_RELEASE_BASELINE_SANITIZERS "Address and undefined-behaviour checks" OFF)
option(UMICOM_RELEASE_BASELINE_RETAIN_LIFECYCLE "Retain thread and queue checks" ON)
option(UMICOM_RELEASE_BASELINE_COMPOSITION_TESTS "Check immediate and deferred wiring" ON)
option(UMICOM_RELEASE_BASELINE_DEFER "Exercise real deferred attachment" OFF)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
function(umicom_apply_warnings target)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /WX)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
    endif()
endfunction()
function(umicom_apply_sanitizers target)
    if(UMICOM_RELEASE_BASELINE_SANITIZERS)
        if(MSVC)
            message(FATAL_ERROR "Use GCC or Clang for this ASan/UBSan profile")
        endif()
        target_compile_options(${target} PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
        target_link_options(${target} PRIVATE -fsanitize=address,undefined)
    endif()
endfunction()
if(UMICOM_RELEASE_BASELINE_DEFER)
    include("${UMICOM_FRAMEWORK_SOURCE}/cmake/UmicomReleaseBaseline.cmake")
endif()
# Compile the actual canonical implementation files, not service substitutes.
# This remains an explicitly focused dependency subset, not the full Core SDK.
add_library(umicom_base STATIC "${UMICOM_FRAMEWORK_SOURCE}/src/base/status.c")
add_library(Umicom::base ALIAS umicom_base)
# Inventory fingerprints use the same SHA-256 implementation as the full SDK.
# Keep it with its base owner so exported static consumers receive the definition.
target_sources(umicom_base PRIVATE "${UMICOM_FRAMEWORK_SOURCE}/src/base/sha256.c")
add_library(umicom_distribution STATIC "${UMICOM_FRAMEWORK_SOURCE}/src/distribution/runtime/release_gate.c")
add_library(Umicom::distribution ALIAS umicom_distribution)
target_link_libraries(umicom_distribution PUBLIC Umicom::base)
# Source observation and archive I/O are part of distribution even when thread
# lifecycle qualification is disabled. Export their real platform owner too.
add_library(umicom_platform STATIC
    "${UMICOM_FRAMEWORK_SOURCE}/src/platform/input_file.c"
    "${UMICOM_FRAMEWORK_SOURCE}/src/platform/output_file.c")
add_library(Umicom::platform ALIAS umicom_platform)
target_link_libraries(umicom_platform PUBLIC Umicom::base)
# The original owner list is retained for review. Platform now shares these
# export, language and warning properties because distribution depends on it.
#[==[
foreach(_target IN ITEMS umicom_base umicom_distribution)
]==]
foreach(_target IN ITEMS umicom_base umicom_distribution umicom_platform)
    string(REPLACE "umicom_" "" _export "${_target}")
    set_target_properties(${_target} PROPERTIES EXPORT_NAME ${_export} C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    target_include_directories(${_target} PUBLIC $<BUILD_INTERFACE:${UMICOM_FRAMEWORK_SOURCE}/include> $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
    umicom_apply_warnings(${_target})
    umicom_apply_sanitizers(${_target})
endforeach()
include("${UMICOM_FRAMEWORK_SOURCE}/cmake/UmicomReleaseBaseline.cmake")
if(UMICOM_RELEASE_BASELINE_RETAIN_LIFECYCLE)
# The lifecycle-only platform declaration is retained for review; native I/O now has an unconditional owner.
if(FALSE)
    add_library(umicom_platform STATIC
        "${UMICOM_FRAMEWORK_SOURCE}/src/platform/threading.c"
        "${UMICOM_FRAMEWORK_SOURCE}/src/platform/cancellation.c"
        "${UMICOM_FRAMEWORK_SOURCE}/src/platform/clock.c"
        "${UMICOM_FRAMEWORK_SOURCE}/src/platform/task.c"
        "${UMICOM_FRAMEWORK_SOURCE}/src/platform/task_queue.c")
    add_library(Umicom::platform ALIAS umicom_platform)
    target_include_directories(umicom_platform PUBLIC "${UMICOM_FRAMEWORK_SOURCE}/include")
    target_link_libraries(umicom_platform PUBLIC Threads::Threads Umicom::base)
    set_target_properties(umicom_platform PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom_platform)
    umicom_apply_sanitizers(umicom_platform)

endif()
    # Optional thread sources extend the same owner instead of creating a second one.
    target_sources(umicom_platform PRIVATE
        "${UMICOM_FRAMEWORK_SOURCE}/src/platform/threading.c"
        "${UMICOM_FRAMEWORK_SOURCE}/src/platform/cancellation.c"
        "${UMICOM_FRAMEWORK_SOURCE}/src/platform/clock.c"
        "${UMICOM_FRAMEWORK_SOURCE}/src/platform/task.c"
        "${UMICOM_FRAMEWORK_SOURCE}/src/platform/task_queue.c")
    target_link_libraries(umicom_platform PUBLIC Threads::Threads)
    include("${UMICOM_FRAMEWORK_SOURCE}/cmake/UmicomQueueShutdown.cmake")
    include("${UMICOM_FRAMEWORK_SOURCE}/cmake/UmicomThreadSafety.cmake")
endif()
# Retain the earlier export list; the platform archive is required by distribution consumers.
if(FALSE)
install(TARGETS umicom_base umicom_distribution EXPORT UmicomReleaseBaselineTargets ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif()
install(TARGETS umicom_base umicom_distribution umicom_platform EXPORT UmicomReleaseBaselineTargets ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES "${UMICOM_FRAMEWORK_SOURCE}/include/umicom/base/status.h" DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/base)
install(FILES "${UMICOM_FRAMEWORK_SOURCE}/include/umicom/distribution/runtime/release_gate.h" "${UMICOM_FRAMEWORK_SOURCE}/include/umicom/distribution/runtime/types.h" DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/distribution/runtime)
include(CMakePackageConfigHelpers)
configure_package_config_file("${CMAKE_CURRENT_LIST_DIR}/UmicomReleaseBaselineConfig.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/UmicomReleaseBaselineConfig.cmake" INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/UmicomReleaseBaseline")
install(EXPORT UmicomReleaseBaselineTargets NAMESPACE Umicom:: DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/UmicomReleaseBaseline")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/UmicomReleaseBaselineConfig.cmake" DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/UmicomReleaseBaseline")
