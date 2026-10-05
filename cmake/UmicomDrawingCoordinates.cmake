# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Use the production chart widget and existing workspace geometry/history owner.
include_guard(GLOBAL)
if(BUILD_TESTING AND TARGET Umicom::ui_gtk4)
    add_executable(umicom-drawing-coordinates-native-test "${CMAKE_CURRENT_LIST_DIR}/../tests/chart_coordinates/test_native.c")
    target_link_libraries(umicom-drawing-coordinates-native-test PRIVATE Umicom::ui_gtk4)
    umicom_apply_warnings(umicom-drawing-coordinates-native-test)
    umicom_apply_sanitizers(umicom-drawing-coordinates-native-test)
    foreach(case apply undo no-op precision support resistance invalid-time overflow-time invalid-price comma-price
            overflow-price underflow-price empty zero-area stale selection instrument locked no-load draft-refresh retained level-no-op)
        add_test(NAME framework.drawing_coordinates.gtk4.${case} COMMAND umicom-drawing-coordinates-native-test ${case})
        set_tests_properties(framework.drawing_coordinates.gtk4.${case} PROPERTIES
            TIMEOUT 60 SKIP_RETURN_CODE 77 LABELS "framework;chart;gtk4;regression")
    endforeach()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-drawing-coordinates-native-test)
    endif()
endif()
