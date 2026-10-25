# Umicom Framework
# File: cmake/UmicomDebugBacktrace.cmake
# PURPOSE: Own debugger stack capture, native review and focused regression registration.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
include_guard(GLOBAL)
target_sources(umicom_debug PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug/backtrace.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug/backtrace_text.c")
target_link_libraries(umicom_debug PRIVATE Umicom::document)
if(COMMAND umicom_target_qualification_inputs)
    umicom_target_qualification_inputs(umicom_debug FILES
        "${CMAKE_CURRENT_LIST_DIR}/../src/debug/backtrace_private.h")
endif()
if(TARGET umicom_ui_gtk4)
    target_sources(umicom_ui_gtk4 PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../adapters/gtk4/debug_backtrace.c")
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom_ui_gtk4 FILES
            "${CMAKE_CURRENT_LIST_DIR}/../adapters/gtk4/window_removal_private.h")
    endif()
endif()
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/guides/captured-call-stacks.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom-framework/docs" COMPONENT Framework)
# Declare both public surfaces for selective SDK ownership as well as full installs.
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/debug/backtrace.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/debug" COMPONENT Framework)
if(COMMAND umicom_release_inventory_own_headers)
    umicom_release_inventory_own_headers(umicom_debug include/umicom/debug/backtrace.h)
endif()
if(TARGET umicom_ui_gtk4)
    install(FILES "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/ui/gtk4/debug_backtrace.h"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/ui/gtk4" COMPONENT Framework)
    if(COMMAND umicom_release_inventory_own_headers)
        umicom_release_inventory_own_headers(umicom_ui_gtk4 include/umicom/ui/gtk4/debug_backtrace.h)
    endif()
endif()
if(NOT BUILD_TESTING)
    return()
endif()
function(umicom_backtrace_group group owner)
    set(target "umi-debug-backtrace-${group}-test")
    add_executable(${target} "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/debug_backtrace/test_${group}.c")
    target_link_libraries(${target} PRIVATE ${owner})
    set_target_properties(${target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(${target})
    umicom_apply_sanitizers(${target})
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(${target})
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(${target} FILES
            "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/debug_backtrace/fixture.h")
    endif()
    foreach(case IN LISTS ARGN)
        set(test "framework.debug.backtrace.${group}.${case}")
        add_test(NAME ${test} COMMAND ${target} ${case})
        set_tests_properties(${test} PROPERTIES TIMEOUT 45
            LABELS "framework;debugger;backtrace;regression")
        if(group STREQUAL "gtk4")
            set_tests_properties(${test} PROPERTIES SKIP_RETURN_CODE 77 ENVIRONMENT "G_DEBUG=fatal-warnings")
        endif()
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(${test} TARGETS ${target})
        endif()
    endforeach()
endfunction()
umicom_backtrace_group(capture Umicom::debug normal empty running missing ownership clone stale-frame stale-thread stale-selection stale-owner unrelated-console limits filtered output-errors preserve-read)
umicom_backtrace_group(format Umicom::debug omit filenames full unicode controls windows trailing-separator no-source unknown-line empty-name empty-stack short query invalid-mode invalid-utf8 preserved-output thread-controls)
if(TARGET umicom_ui_gtk4)
    umicom_backtrace_group(gtk4 Umicom::ui_gtk4 normal cloned paths hidden-copy retained-copy owner-close close-refresh policy-change added-close invalid)
endif()
