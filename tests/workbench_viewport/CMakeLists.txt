# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
add_executable(umicom-workbench-viewport-test test_viewport.c
    "${CMAKE_CURRENT_LIST_DIR}/../../examples/workbench_viewport/practice_layout.c")
target_link_libraries(umicom-workbench-viewport-test PRIVATE Umicom::workbench_viewport)
set_target_properties(umicom-workbench-viewport-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umicom-workbench-viewport-test)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom-workbench-viewport-test)
endif()
set(_cases example tab_position hidden_tab no_active_tab all_tabs_hidden hidden_sidebar hidden_container
    hidden_root auto_visibility minimum_clamp right_minimum insufficient zero_viewport negative_origin
    edge_coordinates focus utf8 immutable sweep large depth depth_exceeded zero_decoration invalid_options invalid_output
    invalid_document nan infinite ratio child_range child_count duplicate_child duplicate_id parent
    unreachable root root_parent self_cycle disconnected_cycle hidden_invalid unterminated overlong_utf8 surrogate control_label negative_size
    sum_overflow coordinate_overflow negative_viewport bad_schema bad_structure bad_node_structure tab_range
    panel_children nested_window missing_component too_many_nodes empty_document)
foreach(_case IN LISTS _cases)
    add_test(NAME framework.workbench_viewport.${_case} COMMAND umicom-workbench-viewport-test ${_case})
    set_tests_properties(framework.workbench_viewport.${_case} PROPERTIES TIMEOUT 30 LABELS "framework;layout;native")
endforeach()
add_test(NAME framework.workbench_viewport.lesson COMMAND umicom-layout-example)
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-workbench-viewport-test)
    umicom_register_validation_target(umicom-layout-example)
endif()
