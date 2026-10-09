# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Standalone regression build for the exception snapshot public-type collision.
# Build the real Framework record owner and compile the real runtime consumers;
# this project does not replace production implementations with test stubs.
cmake_minimum_required(VERSION 3.25)
project(UmicomDebugExceptionSnapshotContracts LANGUAGES C)
include(CTest)

get_filename_component(UMICOM_FRAMEWORK_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)

function(umicom_exception_contract_target target)
    target_include_directories("${target}" PRIVATE "${UMICOM_FRAMEWORK_ROOT}/include")
    set_target_properties("${target}" PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    if(MSVC)
        target_compile_options("${target}" PRIVATE /W4 /WX)
    elseif(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        # Keep diagnostics strict within this focused regression project.
        target_compile_options("${target}" PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
    endif()
endfunction()

add_library(umicom-exception-record-contract-owner STATIC
    "${UMICOM_FRAMEWORK_ROOT}/src/debug/exception.c"
    "${UMICOM_FRAMEWORK_ROOT}/src/base/snapshot_validation.c"
    "${UMICOM_FRAMEWORK_ROOT}/src/base/value_archive.c"
    "${UMICOM_FRAMEWORK_ROOT}/src/base/status.c")
umicom_exception_contract_target(umicom-exception-record-contract-owner)

foreach(order registry-first filters-first)
    set(target "umicom-exception-snapshot-${order}-test")
    add_executable("${target}" test_snapshot_headers.c)
    umicom_exception_contract_target("${target}")
    if(order STREQUAL "filters-first")
        target_compile_definitions("${target}" PRIVATE UMICOM_EXCEPTION_FILTERS_FIRST=1)
    endif()
    target_link_libraries("${target}" PRIVATE umicom-exception-record-contract-owner)
    foreach(case lifecycle fields boundaries invalid capture replace
                 archive-value archive-collection archive-refused archive-empty)
        add_test(NAME "framework.debug.exceptions.snapshot.${order}.${case}"
            COMMAND "${target}" "${case}")
        set_tests_properties("framework.debug.exceptions.snapshot.${order}.${case}"
            PROPERTIES TIMEOUT 60 LABELS "framework;debug;exceptions;contracts;regression")
    endforeach()
endforeach()

# These are compile checks, not executed runtime tests. Compiling the actual
# sources catches a stale member, output pointer or allocation type without
# requiring the complete debugger service link graph for this focused project.
add_library(umicom-exception-runtime-consumer-compile OBJECT
    "${UMICOM_FRAMEWORK_ROOT}/src/debug_runtime/platform.c"
    "${UMICOM_FRAMEWORK_ROOT}/src/debug_runtime/service_bridge.c"
    "${UMICOM_FRAMEWORK_ROOT}/tests/debug_exceptions/test_protocol.c")
umicom_exception_contract_target(umicom-exception-runtime-consumer-compile)

# When GTK is installed, compile the real panel allocation and host callback
# consumer as well. A missing GTK dependency is reported explicitly and does not
# turn these optional compile checks into claimed GUI test passes.
find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(UMICOM_EXCEPTION_CONTRACT_GTK4 QUIET IMPORTED_TARGET gtk4)
endif()
if(TARGET PkgConfig::UMICOM_EXCEPTION_CONTRACT_GTK4)
    add_library(umicom-exception-gtk4-consumer-compile OBJECT
        "${UMICOM_FRAMEWORK_ROOT}/adapters/gtk4/debug_exception_filters.c"
        "${UMICOM_FRAMEWORK_ROOT}/tests/debug_exceptions/test_gtk4.c")
    umicom_exception_contract_target(umicom-exception-gtk4-consumer-compile)
    target_link_libraries(umicom-exception-gtk4-consumer-compile
        PRIVATE PkgConfig::UMICOM_EXCEPTION_CONTRACT_GTK4)
else()
    message(STATUS "GTK4 unavailable: exception panel and host callback compile checks are omitted.")
endif()
