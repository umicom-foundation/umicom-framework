# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
cmake_minimum_required(VERSION 3.24)
project(UmicomQueueShutdown LANGUAGES C)
include(CTest)
include(GNUInstallDirs)
find_package(Threads REQUIRED)
get_filename_component(_queue_default "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
set(UMICOM_FRAMEWORK_SOURCE "${_queue_default}" CACHE PATH "Canonical Framework source")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
option(UMICOM_QUEUE_SHUTDOWN_SANITIZERS "Instrument this focused native build" OFF)
option(UMICOM_QUEUE_SHUTDOWN_STRICT "Treat focused-build warnings as errors" ON)
option(UMICOM_QUEUE_SHUTDOWN_DISABLE_TRY_JOIN "Exercise the explicit unsupported-platform path" OFF)
option(UMICOM_QUEUE_SHUTDOWN_RETAIN_THREAD_TESTS "Register the preceding thread-lifetime suite" ON)
function(umicom_apply_warnings target)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4)
        if(UMICOM_QUEUE_SHUTDOWN_STRICT)
            target_compile_options(${target} PRIVATE /WX)
        endif()
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
        if(UMICOM_QUEUE_SHUTDOWN_STRICT)
            target_compile_options(${target} PRIVATE -Werror)
        endif()
    endif()
endfunction()
function(umicom_apply_sanitizers target)
    if(UMICOM_QUEUE_SHUTDOWN_SANITIZERS)
        if(MSVC)
            message(FATAL_ERROR "Use GCC or Clang for this sanitizer laboratory")
        endif()
        target_compile_options(${target} PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
        target_link_options(${target} PRIVATE -fsanitize=address,undefined)
    endif()
endfunction()
# This is the actual platform implementation, deliberately built as a focused
# dependency subset. No stub worker, task, queue, clock or cancellation service.
add_library(umicom_base STATIC "${UMICOM_FRAMEWORK_SOURCE}/src/base/status.c")
add_library(Umicom::base ALIAS umicom_base)
set_target_properties(umicom_base PROPERTIES EXPORT_NAME base
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
target_include_directories(umicom_base PUBLIC
    $<BUILD_INTERFACE:${UMICOM_FRAMEWORK_SOURCE}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
umicom_apply_warnings(umicom_base)
umicom_apply_sanitizers(umicom_base)
add_library(umicom_platform STATIC
    "${UMICOM_FRAMEWORK_SOURCE}/src/platform/threading.c"
    "${UMICOM_FRAMEWORK_SOURCE}/src/platform/cancellation.c"
    "${UMICOM_FRAMEWORK_SOURCE}/src/platform/clock.c"
    "${UMICOM_FRAMEWORK_SOURCE}/src/platform/task.c"
    "${UMICOM_FRAMEWORK_SOURCE}/src/platform/task_queue.c")
add_library(Umicom::platform ALIAS umicom_platform)
set_target_properties(umicom_platform PROPERTIES EXPORT_NAME platform
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
target_include_directories(umicom_platform PUBLIC
    $<BUILD_INTERFACE:${UMICOM_FRAMEWORK_SOURCE}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(umicom_platform PUBLIC Threads::Threads Umicom::base)
if(UMICOM_QUEUE_SHUTDOWN_DISABLE_TRY_JOIN)
    target_compile_definitions(umicom_platform PRIVATE UMICOM_THREAD_DISABLE_TRY_JOIN=1)
endif()
umicom_apply_warnings(umicom_platform)
umicom_apply_sanitizers(umicom_platform)
include("${UMICOM_FRAMEWORK_SOURCE}/cmake/UmicomQueueShutdown.cmake")
if(UMICOM_QUEUE_SHUTDOWN_RETAIN_THREAD_TESTS)
    include("${UMICOM_FRAMEWORK_SOURCE}/cmake/UmicomThreadSafety.cmake")
endif()
install(TARGETS umicom_base umicom_platform EXPORT UmicomQueueShutdownTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES "${UMICOM_FRAMEWORK_SOURCE}/include/umicom/base/status.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/base)
foreach(header IN ITEMS threading cancellation clock task task_queue)
    install(FILES "${UMICOM_FRAMEWORK_SOURCE}/include/umicom/platform/${header}.h"
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/platform)
endforeach()
include(CMakePackageConfigHelpers)
configure_package_config_file("${CMAKE_CURRENT_LIST_DIR}/UmicomQueueShutdownConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/UmicomQueueShutdownConfig.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/UmicomQueueShutdown")
install(EXPORT UmicomQueueShutdownTargets NAMESPACE Umicom::
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/UmicomQueueShutdown")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/UmicomQueueShutdownConfig.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/UmicomQueueShutdown")
