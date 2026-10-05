# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Included-file discovery uses native fixtures without invoking CMake itself.
include_guard(GLOBAL)
if(BUILD_TESTING)
    add_executable(umicom-preset-graph-test "${CMAKE_CURRENT_LIST_DIR}/../tests/project_presets/test_graph.c")
    target_link_libraries(umicom-preset-graph-test PRIVATE Umicom::developer)
    umicom_apply_warnings(umicom-preset-graph-test)
    umicom_apply_sanitizers(umicom-preset-graph-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-preset-graph-test)
    endif()
    foreach(case basic missing include-missing malformed cycle repeat diamond visibility inherit-cycle inherit-missing inherit-type duplicate duplicate-field precedence null false deferred override first-parent user user-only reverse macro-source macro-file macro-environment macro-format include-format unicode documents large bytes cancel legacy)
        file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qualification/preset-graph/${case}")
        add_test(NAME framework.project_presets.graph.${case} COMMAND umicom-preset-graph-test ${case})
        set_tests_properties(framework.project_presets.graph.${case} PROPERTIES TIMEOUT 60
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qualification/preset-graph/${case}"
            LABELS "framework;developer;presets;regression")
    endforeach()
endif()
