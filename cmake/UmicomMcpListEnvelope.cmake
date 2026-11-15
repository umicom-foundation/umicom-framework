# Umicom Framework
# File: cmake/UmicomMcpListEnvelope.cmake
#
# PURPOSE:
# Register complete MCP list and discovery publication regressions.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT


if(BUILD_TESTING AND TARGET Umicom::ai)
    add_executable(umicom-mcp-list-envelope-test
        tests/ai_mcp/test_list_envelope.c
        tests/ai_mcp/test_list_allocation.c)
    target_link_libraries(umicom-mcp-list-envelope-test PRIVATE Umicom::ai)
    set_target_properties(umicom-mcp-list-envelope-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-mcp-list-envelope-test)
    umicom_apply_sanitizers(umicom-mcp-list-envelope-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-mcp-list-envelope-test)
    endif()
    function(_umicom_mcp_list_case mode name)
        set(_name "framework.ai.mcp-list.${mode}.${name}")
        add_test(NAME "${_name}" COMMAND umicom-mcp-list-envelope-test "${mode}" "${name}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 30 LABELS "framework;ai;mcp;json")
        umicom_test_impact_owners("${_name}" TARGETS umicom-mcp-list-envelope-test)
    endfunction()
    foreach(_case IN ITEMS empty objects spaced nested-lookalike root-precedence missing null-list object-list string-list root-array leading-comma trailing-comma double-comma missing-comma nonobject-first nonobject-late duplicate-list unclosed-array unclosed-root trailing-root trailing-value unknown-escape surrogate nul-text invalid-number invalid-whitespace unicode unknown-duplicate nested-shapes escaped-key escaped-duplicate key-last key-overflow byte-last byte-overflow depth-last depth-overflow invalid-utf8 callback-first callback-second nested-visit mutate-input fault-parse fault-buffer fault-empty fault-preflight nulls parser-output)
        _umicom_mcp_list_case(visitor "${_case}")
    endforeach()
    foreach(_kind IN ITEMS tool resource prompt)
        foreach(_case IN ITEMS empty objects nested-lookalike missing null-list object-list string-list root-array leading-comma trailing-comma double-comma missing-comma nonobject-first nonobject-late duplicate-list unclosed-array unclosed-root trailing-root trailing-value unknown-escape surrogate nul-text invalid-number invalid-whitespace)
            _umicom_mcp_list_case("${_kind}" "${_case}")
        endforeach()
    endforeach()
endif()
