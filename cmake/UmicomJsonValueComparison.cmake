# Umicom Framework
# File: cmake/UmicomJsonValueComparison.cmake
#
# PURPOSE:
# Register immutable JSON comparison and whole-subtree metadata uniqueness regressions.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

include_guard(GLOBAL)
if(BUILD_TESTING)
    add_executable(umicom-json-value-comparison-test "${CMAKE_CURRENT_LIST_DIR}/../tests/json_tree/test_value_comparison.c")
    target_link_libraries(umicom-json-value-comparison-test PRIVATE Umicom::developer)
    umicom_apply_warnings(umicom-json-value-comparison-test)
    umicom_apply_sanitizers(umicom-json-value-comparison-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-json-value-comparison-test)
    endif()
    foreach(case null null-bool bool bool-change integer numeric-spelling negative-zero big-integer string escape unicode array array-order array-count object-order key-escape nested key-change value-change object-count empty-array empty-object type-change string-number duplicate-left duplicate-right duplicate-escaped nested-duplicate cancel null-left null-right negative-node outside-node null-output subtree long-key deep wide)
        add_test(NAME framework.language_runtime.json_value_comparison.${case} COMMAND umicom-json-value-comparison-test ${case})
        set_tests_properties(framework.language_runtime.json_value_comparison.${case} PROPERTIES TIMEOUT 30
            LABELS "framework;language;locations;ownership;regression")
    endforeach()
endif()

# Whole-document admission reuses the canonical protocol-owned JSON tree.
if(BUILD_TESTING)
    add_executable(umicom-json-unique-keys-test "${CMAKE_CURRENT_LIST_DIR}/../tests/json_tree/test_unique_keys.c")
    target_link_libraries(umicom-json-unique-keys-test PRIVATE Umicom::protocol)
    set_target_properties(umicom-json-unique-keys-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-json-unique-keys-test)
    umicom_apply_sanitizers(umicom-json-unique-keys-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-json-unique-keys-test)
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-json-unique-keys-test FILES
            "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/json_tree.c"
            "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/json_tree_compare.inc")
    endif()
    foreach(_case IN ITEMS empty-object
            empty-array
            primitive
            string
            unique
            nested
            separate-objects
            case-distinct
            unicode-distinct
            duplicate
            escaped-duplicate
            unicode-duplicate
            metadata-duplicate
            array-duplicate
            late-duplicate
            empty-key
            duplicate-empty-key
            subtree
            outside-subtree
            subtree-duplicate
            null-tree
            negative-node
            outside-node
            cancel
            cancel-empty
            cancel-reset
            long-key
            long-duplicate
            deep
            wide)
        set(_test "framework.language_runtime.json_unique_keys.${_case}")
        add_test(NAME "${_test}" COMMAND umicom-json-unique-keys-test "${_case}")
        set_tests_properties("${_test}" PROPERTIES TIMEOUT 30
            LABELS "framework;language;json;metadata;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("${_test}" TARGETS umicom-json-unique-keys-test)
        endif()
    endforeach()
endif()

if(BUILD_TESTING)
    add_executable(umicom-json-unique-keys-allocation-test "${CMAKE_CURRENT_LIST_DIR}/../tests/json_tree/test_unique_keys_allocation.c")
    target_link_libraries(umicom-json-unique-keys-allocation-test PRIVATE Umicom::protocol)
    set_target_properties(umicom-json-unique-keys-allocation-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-json-unique-keys-allocation-test)
    umicom_apply_sanitizers(umicom-json-unique-keys-allocation-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-json-unique-keys-allocation-test)
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-json-unique-keys-allocation-test FILES
            "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/json_tree.c"
            "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/json_tree_compare.inc")
    endif()
    foreach(_case IN ITEMS allocation duplicate-cleanup cancel invalid)
        set(_test "framework.language_runtime.json_unique_keys_allocation.${_case}")
        add_test(NAME "${_test}" COMMAND umicom-json-unique-keys-allocation-test "${_case}")
        set_tests_properties("${_test}" PROPERTIES TIMEOUT 30
            LABELS "framework;language;json;metadata;ownership;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("${_test}" TARGETS umicom-json-unique-keys-allocation-test)
        endif()
    endforeach()
endif()
