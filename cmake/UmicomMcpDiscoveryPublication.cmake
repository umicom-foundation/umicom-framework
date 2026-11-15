# Umicom Framework
# File: cmake/UmicomMcpDiscoveryPublication.cmake
#
# PURPOSE:
# Register failure-preserving MCP list and composed client discovery regressions.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

if(BUILD_TESTING AND TARGET Umicom::ai)
    add_executable(umicom-mcp-discovery-publication-test
        tests/ai_mcp/test_discovery_publication.c
        tests/ai_mcp/test_discovery_allocation.c)
    target_link_libraries(umicom-mcp-discovery-publication-test PRIVATE Umicom::ai)
    set_target_properties(umicom-mcp-discovery-publication-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-mcp-discovery-publication-test)
    umicom_apply_sanitizers(umicom-mcp-discovery-publication-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-mcp-discovery-publication-test)
    endif()
    function(_umicom_mcp_discovery_case mode kind name)
        set(_name "framework.ai.mcp-discovery.${mode}.${kind}.${name}")
        add_test(NAME "${_name}" COMMAND umicom-mcp-discovery-publication-test "${mode}" "${kind}" "${name}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;mcp;publication")
        umicom_test_impact_owners("${_name}" TARGETS umicom-mcp-discovery-publication-test)
    endfunction()
    foreach(_kind IN ITEMS tools resources prompts)
        foreach(_case IN ITEMS success empty maximum fresh error missing duplicate late-invalid truncated overflow allocation revision disconnect not-ready null-session)
            _umicom_mcp_discovery_case(single "${_kind}" "${_case}")
        endforeach()
        foreach(_case IN ITEMS error late-invalid revision disconnect flags subset)
            _umicom_mcp_discovery_case(client "${_kind}" "${_case}")
        endforeach()
    endforeach()
    foreach(_case IN ITEMS success none allocation-1 allocation-2 allocation-3 allocation-4)
        _umicom_mcp_discovery_case(client tools "${_case}")
    endforeach()
    _umicom_mcp_discovery_case(nulls all all)
endif()
