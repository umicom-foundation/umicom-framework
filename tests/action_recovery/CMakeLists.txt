#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/action_recovery/CMakeLists.txt
# PURPOSE: Exercise project discovery, rejection feedback and launcher recovery.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
# These tests link the existing production libraries; they do not substitute
# another project graph, trading workspace, process catalogue or launcher.
function(umicom_add_action_recovery_test group library source)
    set(target "umicom-action-recovery-${group}-test")
    add_executable("${target}" "${source}")
    target_link_libraries("${target}" PRIVATE "${library}")
    set_target_properties("${target}" PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED ON)
    umicom_apply_warnings("${target}")
    umicom_apply_sanitizers("${target}")
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target("${target}")
    endif()
    foreach(test_case IN LISTS ARGN)
        add_test(NAME "framework.action_recovery.${group}.${test_case}"
            COMMAND "${target}" "${test_case}")
        set_tests_properties("framework.action_recovery.${group}.${test_case}"
            PROPERTIES TIMEOUT 30 LABELS "framework;action-recovery;regression")
    endforeach()
endfunction()

umicom_add_action_recovery_test(discovery Umicom::platform test_discovery.c
    capacity success unchanged missing-root closed)
umicom_add_action_recovery_test(feedback Umicom::trading_ui test_feedback.c
    live-denied submit-denied selection-missing cancel-missing quantity-invalid
    preview-once valid-selection no-handler)
umicom_add_action_recovery_test(launcher Umicom::application test_launcher.c
    message-atomicity activation-missing stop-failure restart-failure retry
    exit-during-failure replacement-during-failure exit-during-success
    replacement-during-success disabled-during-restart)
add_custom_target(umicom-action-recovery-tests DEPENDS
    umicom-action-recovery-discovery-test
    umicom-action-recovery-feedback-test
    umicom-action-recovery-launcher-test)

# GNU linker wrapping injects a completion error or owner change after the real
# walk. This Linux-only fixture does not substitute the production visitor.
if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    umicom_add_action_recovery_test(interference Umicom::platform test_discovery_interference.c
        read-error trust-change workspace-change close allocation)
    target_link_options(umicom-action-recovery-interference-test PRIVATE
        "-Wl,--wrap=umi_directory_walk" "-Wl,--wrap=calloc")
    add_dependencies(umicom-action-recovery-tests umicom-action-recovery-interference-test)
endif()

# Build the public example in the normal regression path so the printed API
# usage cannot drift into an uncompiled, fictional interface.
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../../examples/action_recovery"
    "${CMAKE_CURRENT_BINARY_DIR}/example")
add_dependencies(umicom-action-recovery-tests umicom-action-recovery-example)
