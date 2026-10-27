# Umicom Framework
# File: cmake/UmicomWindowInputLifetimeTests.cmake
# Purpose: Qualify text-input teardown before native window surfaces disappear.
# Author and organisation: Sammy Hegab, Umicom Foundation
# Licence: MIT
if(TARGET Umicom::ui_gtk4 AND NOT TARGET umicom-window-input-lifetime-test)
    add_executable(umicom-window-input-lifetime-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/window_lifetime/test_input_lifetime.c")
    target_link_libraries(umicom-window-input-lifetime-test PRIVATE Umicom::ui_gtk4)
    umicom_apply_warnings(umicom-window-input-lifetime-test)
    umicom_apply_sanitizers(umicom-window-input-lifetime-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-window-input-lifetime-test)
    endif()
    foreach(case direct parent)
        add_test(NAME framework.window_input_lifetime.${case}
            COMMAND umicom-window-input-lifetime-test ${case})
        set_tests_properties(framework.window_input_lifetime.${case} PROPERTIES
            TIMEOUT 30 SKIP_RETURN_CODE 77 RUN_SERIAL TRUE
            ENVIRONMENT "G_DEBUG=fatal-criticals"
            LABELS "framework;gtk4;window_lifetime")
    endforeach()
endif()
