#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomDebugReceiveChecks.cmake
#
# PURPOSE:
#   Qualify debugger receive ownership and shared byte framing.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
if(BUILD_TESTING)
    add_executable(umicom-debug-receive-checks
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_receive_boundaries.c")
    target_link_libraries(umicom-debug-receive-checks PRIVATE Umicom::developer)
    set_target_properties(umicom-debug-receive-checks PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-debug-receive-checks)
    umicom_apply_sanitizers(umicom-debug-receive-checks)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-debug-receive-checks)
    endif()
    foreach(case arguments oversize read-error empty malformed-json fragmented buffered-pair buffered-error)
        add_test(NAME "framework.debug_runtime.receive.${case}"
            COMMAND umicom-debug-receive-checks "${case}")
        set_tests_properties("framework.debug_runtime.receive.${case}" PROPERTIES
            TIMEOUT 30 LABELS "framework;debugger;framing;ownership;regression")
    endforeach()
endif()
