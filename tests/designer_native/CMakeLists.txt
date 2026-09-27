# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
add_executable(umicom-designer-native-test "${_native_test_source}/test_native.c")
target_link_libraries(umicom-designer-native-test PRIVATE Umicom::designer_native)
umicom_apply_warnings(umicom-designer-native-test)
umicom_apply_sanitizers(umicom-designer-native-test)
set(_cases
    remove_subtree remove_reverse remove_missing deep_cycle_removal
    failed_property alias_property full_attributes bad_attribute_count unterminated_node
    duplicate_attribute invalid_parent rejected_document_node unclosed_quote quoted_comments
    notes_profile unknown_component unknown_property wrong_type size_range bad_utf8
    numeric_string contradictory_value orphan cycle nested_window second_root hidden_root leaf_children
    bad_action action_source action_target missing_action future_schema maximum_nodes too_many_nodes
    depth_limit split_tabs empty_tabs one_split_child plan_lifetime invalid_names escaped_source invalid_paths)
foreach(_case IN LISTS _cases)
    add_test(NAME framework.designer_native.${_case} COMMAND umicom-designer-native-test ${_case})
    set_tests_properties(framework.designer_native.${_case} PROPERTIES TIMEOUT 30)
endforeach()
file(MAKE_DIRECTORY "${_native_test_binary}/fixtures")
add_test(NAME framework.designer_native.publish_exact COMMAND ${CMAKE_COMMAND}
    -DTOOL=$<TARGET_FILE:umicom-designer-native-test>
    -DROOT=${_native_test_binary}/fixtures -DCASE=publish_exact -P ${_native_test_source}/run_file_test.cmake)
add_test(NAME framework.designer_native.self_test COMMAND umicom-designer-project --self-test)
add_test(NAME framework.designer_native.lesson COMMAND umicom-designer-project-example)
add_test(NAME framework.designer_native.cli_help COMMAND umicom-designer-project --help)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    add_executable(umicom-designer-native-fault-test "${_native_test_source}/test_faults.c")
    target_link_libraries(umicom-designer-native-fault-test PRIVATE Umicom::designer_native)
    target_link_options(umicom-designer-native-fault-test PRIVATE
        -Wl,--wrap=malloc -Wl,--wrap=calloc -Wl,--wrap=realloc -Wl,--wrap=write -Wl,--wrap=fsync)
    umicom_apply_warnings(umicom-designer-native-fault-test)
    umicom_apply_sanitizers(umicom-designer-native-fault-test)
    foreach(_case IN ITEMS allocation_plan allocation_clone allocation_notes)
        add_test(NAME framework.designer_native.${_case} COMMAND umicom-designer-native-fault-test ${_case})
    endforeach()
    foreach(_case IN ITEMS write_failure flush_failure directory_flush_failure short_write)
        add_test(NAME framework.designer_native.${_case} COMMAND ${CMAKE_COMMAND}
            -DTOOL=$<TARGET_FILE:umicom-designer-native-fault-test>
            -DROOT=${_native_test_binary}/fixtures -DCASE=${_case} -P ${_native_test_source}/run_file_test.cmake)
    endforeach()
endif()
# This test builds an independently installed focused SDK and then actually
# compiles generated C. It is independent of the complete parent SDK export.
add_test(NAME framework.designer_native.generated_build COMMAND ${CMAKE_COMMAND}
    -DFRAMEWORK=${_native_test_source}/../..
    -DROOT=${_native_test_binary}/generated-build
    -DCOMPILER=${CMAKE_C_COMPILER} -P ${_native_test_source}/generated_build.cmake)
set_tests_properties(framework.designer_native.generated_build PROPERTIES TIMEOUT 180)
