#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomCodeGuardOutput.cmake
#
# PURPOSE:
# Attach shared CodeGuard output presentation and register context/reset regressions.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

if(TARGET umicom_ui_gtk4)
    target_sources(umicom_ui_gtk4 PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/../adapters/gtk4/codeguard_output_gtk4.c")
    target_link_libraries(umicom_ui_gtk4 PUBLIC Umicom::codeguard)
    if(BUILD_TESTING)
        add_executable(umicom-output-view-reset-tests
            "${CMAKE_CURRENT_LIST_DIR}/../tests/live_build_output/test_presenter_reset.c")
        add_executable(umicom-codeguard-output-gtk4-tests
            "${CMAKE_CURRENT_LIST_DIR}/../tests/codeguard/test_output_gtk4.c")
        foreach(_target IN ITEMS umicom-output-view-reset-tests umicom-codeguard-output-gtk4-tests)
            target_link_libraries(${_target} PRIVATE Umicom::ui_gtk4)
            target_compile_features(${_target} PRIVATE c_std_23)
            umicom_apply_warnings(${_target})
            umicom_apply_sanitizers(${_target})
            if(COMMAND umicom_register_validation_target)
                umicom_register_validation_target(${_target})
            endif()
        endforeach()
        foreach(_case IN ITEMS paused following restart-identity nested-reset nested-update release invalid retained)
            add_test(NAME "framework.output_view_reset.${_case}"
                COMMAND umicom-output-view-reset-tests "${_case}")
            set_tests_properties("framework.output_view_reset.${_case}" PROPERTIES
                TIMEOUT 30 SKIP_RETURN_CODE 77 LABELS "framework;output;gtk4;ownership")
        endforeach()
        foreach(_case IN ITEMS ready success pending failed cancelled stale closed context-change report-loss oversize malformed unchanged invalid owner-release nested request-restart)
            add_test(NAME "framework.codeguard_output.${_case}"
                COMMAND umicom-codeguard-output-gtk4-tests "${_case}")
            set_tests_properties("framework.codeguard_output.${_case}" PROPERTIES
                TIMEOUT 30 SKIP_RETURN_CODE 77 LABELS "framework;codeguard;output;gtk4")
        endforeach()
    endif()
endif()
