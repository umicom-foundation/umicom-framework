# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Only native in-memory catalogue work is executed by these fixtures.
foreach(_query_target IN ITEMS umicom-test-explorer-query umicom-test-explorer-query-failure)
    if(_query_target STREQUAL "umicom-test-explorer-query")
        set(_query_source tests/test_explorer_query.c)
        set(_query_cases filter clear suite disabled sort group combined selection empty
            invalid-text invalid-sort invalid-group invalid-outcome null-output null-filter binding bound-create)
        set(_query_prefix framework.test.explorer_query)
    else()
        set(_query_source tests/test_explorer_query_failure.c)
        set(_query_cases model-allocation filter-allocation sort-allocation group-allocation
            workspace-allocation workspace-stale workspace-overflow session-overflow model-overflow retry)
        set(_query_prefix framework.test.explorer_query_failure)
    endif()
    add_executable(${_query_target} ${_query_source})
    target_link_libraries(${_query_target} PRIVATE Umicom::test_platform)
    set_target_properties(${_query_target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(${_query_target})
    umicom_apply_sanitizers(${_query_target})
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(${_query_target})
    endif()
    foreach(_query_case IN LISTS _query_cases)
        add_test(NAME "${_query_prefix}.${_query_case}" COMMAND ${_query_target} "${_query_case}")
        set_tests_properties("${_query_prefix}.${_query_case}" PROPERTIES
            TIMEOUT 60 LABELS "framework;test-platform;transaction;regression")
    endforeach()
endforeach()
