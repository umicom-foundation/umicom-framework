#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomOutputViewLifetime.cmake
#
# PURPOSE:
# Register shared output callback lifetime and nested-publication regressions.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

if(BUILD_TESTING AND TARGET umicom_ui_gtk4)
    add_executable(umicom-output-view-lifetime-tests
        "${CMAKE_CURRENT_LIST_DIR}/../tests/live_build_output/test_presenter_lifetime.c")
    target_link_libraries(umicom-output-view-lifetime-tests PRIVATE Umicom::ui_gtk4)
    target_compile_features(umicom-output-view-lifetime-tests PRIVATE c_std_23)
    umicom_apply_warnings(umicom-output-view-lifetime-tests)
    umicom_apply_sanitizers(umicom-output-view-lifetime-tests)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-output-view-lifetime-tests)
    endif()
    foreach(_case IN ITEMS update-release status-release refresh-release follow-release window-close update-nested status-nested refresh-nested follow-nested recursive-refresh recursive-follow recursive-copy)
        add_test(NAME "framework.output_view_lifetime.${_case}"
            COMMAND umicom-output-view-lifetime-tests "${_case}")
        set_tests_properties("framework.output_view_lifetime.${_case}" PROPERTIES
            TIMEOUT 30 SKIP_RETURN_CODE 77 LABELS "framework;output;gtk4;ownership")
    endforeach()
endif()
