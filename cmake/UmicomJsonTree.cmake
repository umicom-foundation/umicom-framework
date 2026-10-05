# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# The owned JSON service shares the existing developer parser/text owner.
include_guard(GLOBAL)
target_sources(umicom_developer PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/json_tree.c")
if(BUILD_TESTING)
    add_executable(umicom-json-tree-test "${CMAKE_CURRENT_LIST_DIR}/../tests/json_tree/test_tree.c")
    target_link_libraries(umicom-json-tree-test PRIVATE Umicom::developer)
    umicom_apply_warnings(umicom-json-tree-test)
    umicom_apply_sanitizers(umicom-json-tree-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-json-tree-test)
    endif()
    foreach(case null ownership grammar unicode integers booleans bounds depth cancellation members siblings span)
        add_test(NAME framework.json_tree.${case} COMMAND umicom-json-tree-test ${case})
        set_tests_properties(framework.json_tree.${case} PROPERTIES TIMEOUT 30
            LABELS "framework;json;ownership;regression")
    endforeach()
endif()
