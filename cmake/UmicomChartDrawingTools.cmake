# Tool geometry and protected mutations belong to the existing chart library.
target_sources(umicom_chart PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/chart/drawing_tools.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/chart/drawing_edit.c")
if(BUILD_TESTING)
    function(umicom_chart_tool_cases group file)
        set(target "umicom-chart-tool-${group}-test")
        add_executable(${target} "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/chart_tools/${file}")
        target_link_libraries(${target} PRIVATE Umicom::trading_ui Umicom::trading_chart_persistence)
        umicom_apply_warnings(${target})
        umicom_apply_sanitizers(${target})
        foreach(case IN LISTS ARGN)
            add_test(NAME "framework.chart_tools.${group}.${case}" COMMAND ${target} "${case}")
            set_tests_properties("framework.chart_tools.${group}.${case}" PROPERTIES
                TIMEOUT 30 LABELS "framework;chart;regression")
        endforeach()
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${target})
        endif()
    endfunction()
    umicom_chart_tool_cases(geometry test_geometry.c
        catalogue invalid range reversed-zone ray-right ray-left ray-away slope-clipping
        offscreen single-time extreme-time render-capacity render-theme)
    umicom_chart_tool_cases(edit test_edit.c lock move duplicate stale wrong-pane invalid capacity)
    umicom_chart_tool_cases(workspace test_workspace.c create edit guards persistence empty-history scene)
endif()
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/CHART_DRAWING_TOOLS.md"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom-framework/docs)

if(BUILD_TESTING AND TARGET Umicom::ui_gtk4)
    add_executable(umicom-chart-tool-native-test "${CMAKE_CURRENT_LIST_DIR}/../tests/chart_tools/test_native.c")
    target_link_libraries(umicom-chart-tool-native-test PRIVATE Umicom::ui_gtk4)
    umicom_apply_warnings(umicom-chart-tool-native-test)
    umicom_apply_sanitizers(umicom-chart-tool-native-test)
    foreach(case new-tools lock move duplicate stale-move selection-change escape instrument-change retained)
        add_test(NAME "framework.chart_tools.native.${case}" COMMAND umicom-chart-tool-native-test "${case}")
        set_tests_properties("framework.chart_tools.native.${case}" PROPERTIES
            TIMEOUT 60 SKIP_RETURN_CODE 77 LABELS "framework;chart;gtk4;regression")
    endforeach()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-chart-tool-native-test)
    endif()
endif()
