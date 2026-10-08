# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Portable retention belongs to platform; GTK rendering remains an optional adapter.
include_guard(GLOBAL)
target_sources(umicom_platform PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../src/platform/output_tail.c")
function(umicom_attach_live_test_output_gtk4)
    if(NOT TARGET umicom_ui_gtk4)
        return()
    endif()
    get_target_property(attached umicom_ui_gtk4 UMICOM_LIVE_TEST_OUTPUT_ATTACHED)
    if(attached)
        return()
    endif()
    target_sources(umicom_ui_gtk4 PRIVATE
        "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../adapters/gtk4/output_view_gtk4.c"
        "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../adapters/gtk4/ctest_output_gtk4.c")
    target_link_libraries(umicom_ui_gtk4 PUBLIC Umicom::testing)
    set_property(TARGET umicom_ui_gtk4 PROPERTY UMICOM_LIVE_TEST_OUTPUT_ATTACHED TRUE)
    if(BUILD_TESTING)
        add_executable(umicom-live-test-output-gtk4-check
            "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/live_test_output/test_gtk4.c")
        target_link_libraries(umicom-live-test-output-gtk4-check PRIVATE Umicom::ui_gtk4)
        umicom_apply_warnings(umicom-live-test-output-gtk4-check)
        umicom_apply_sanitizers(umicom-live-test-output-gtk4-check)
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(umicom-live-test-output-gtk4-check)
        endif()
        foreach(case pause refresh resume copy binary split-utf8 invalid stale status retained unchanged
                attempt skipped capture-error saturated generic-invalid)
            add_test(NAME "framework.live_test_output.gtk4.${case}"
                COMMAND umicom-live-test-output-gtk4-check "${case}")
            set_tests_properties("framework.live_test_output.gtk4.${case}" PROPERTIES
                TIMEOUT 30 SKIP_RETURN_CODE 77 LABELS "framework;testing;output;gtk4;regression")
        endforeach()
    endif()
endfunction()
umicom_attach_live_test_output_gtk4()
if(NOT TARGET umicom_ui_gtk4)
    cmake_language(DEFER CALL umicom_attach_live_test_output_gtk4)
endif()
if(BUILD_TESTING)
    include("${CMAKE_CURRENT_LIST_DIR}/../tests/live_test_output/checks.cmake")
endif()
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/live-test-output.html"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/learning COMPONENT Learning)
