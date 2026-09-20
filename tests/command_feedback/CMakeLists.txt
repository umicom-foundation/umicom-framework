#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/command_feedback/CMakeLists.txt
# PURPOSE: Test the production presentation runtime with real catalogue recipes.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
add_executable(umicom-command-feedback-test test_command_feedback.c)
target_link_libraries(umicom-command-feedback-test PRIVATE Umicom::application)
umicom_apply_warnings(umicom-command-feedback-test)
umicom_apply_sanitizers(umicom-command-feedback-test)
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-command-feedback-test)
endif()
foreach(case IN ITEMS
    update-message
    update-badge
    update-state
    update-progress
    update-dirty
    update-flag
    update-valid
    check-pure
    check-invalid
    failure-helper
    projection
    unbound
    guarded-unbound
    read-only
    not-started
    stopped
    busy
    loading
    hidden
    background
    success
    failure
    denied
    unsupported
    cancelled
    unknown-status
    malformed-success
    malformed-failure
    invalid-success-state
    host-failure
    refresh-failure
    recipe-studio
    recipe-trader
    recipe-desktop
    recipe-bank
    recipe-tms)
    add_test(NAME framework.command_feedback.${case}
        COMMAND umicom-command-feedback-test ${case})
    set_tests_properties(framework.command_feedback.${case} PROPERTIES TIMEOUT 30
        LABELS "framework;command-feedback;regression")
endforeach()

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../../examples/command_feedback"
    "${CMAKE_CURRENT_BINARY_DIR}/example")
if(TARGET umicom_ui_gtk4)
    add_executable(umicom-command-feedback-gtk4-test test_command_feedback_gtk4.c)
    target_link_libraries(umicom-command-feedback-gtk4-test PRIVATE Umicom::ui_gtk4 Umicom::application)
    umicom_apply_warnings(umicom-command-feedback-gtk4-test)
    umicom_apply_sanitizers(umicom-command-feedback-gtk4-test)
    foreach(product IN ITEMS bank tms)
        add_test(NAME framework.command_feedback.gtk4.${product}
            COMMAND umicom-command-feedback-gtk4-test ${product})
        set_tests_properties(framework.command_feedback.gtk4.${product} PROPERTIES
            TIMEOUT 30 SKIP_RETURN_CODE 77 LABELS "framework;command-feedback;gtk4")
    endforeach()
endif()
