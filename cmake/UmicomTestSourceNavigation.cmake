# Test-source parsing, provenance and document operations use real Framework owners.
# These cases require no display, process launch or market/bank connection.
if(BUILD_TESTING)
    function(umicom_test_source_cases group file)
        set(target "umicom-test-source-${group}-test")
        add_executable(${target} "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/test_source_navigation/${file}")
        target_link_libraries(${target} PRIVATE Umicom::test_ui)
        umicom_apply_warnings(${target})
        umicom_apply_sanitizers(${target})
        foreach(case IN LISTS ARGN)
            add_test(NAME "framework.test_source.${group}.${case}" COMMAND ${target} "${case}")
            set_tests_properties("framework.test_source.${group}.${case}" PROPERTIES
                TIMEOUT 30 LABELS "framework;test-source;regression")
        endforeach()
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${target})
        endif()
    endfunction()
    umicom_test_source_cases(parser test_parser.c
        assertions windows compiler ctest-prefix colours unknown invalid-positions capacity arguments)
    umicom_test_source_cases(links test_links.c
        selection ownership duplicates sessions fragments capacity retention empty)
    umicom_test_source_cases(open test_open.c
        absolute relative draft missing-file missing-line explicit-base invalid-location discovered
        other-tab new-tab-missing-line file-uri)
endif()
