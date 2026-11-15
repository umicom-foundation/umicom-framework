# Umicom Framework
# File: cmake/UmicomMcpToolRegistration.cmake
#
# PURPOSE:
# Register complete MCP tool publication and preserved binding regressions.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

if(BUILD_TESTING AND TARGET Umicom::ai)
    add_executable(umicom-mcp-tool-registration-test
        tests/ai_mcp/test_bridge_publication.c
        tests/ai_mcp/test_bridge_allocation.c)
    target_link_libraries(umicom-mcp-tool-registration-test PRIVATE Umicom::ai)
    set_target_properties(umicom-mcp-tool-registration-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-mcp-tool-registration-test)
    umicom_apply_sanitizers(umicom-mcp-tool-registration-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-mcp-tool-registration-test)
    endif()
    function(_umicom_mcp_registration_case mode name)
        set(_name "framework.ai.mcp-registration.${mode}.${name}")
        add_test(NAME "${_name}" COMMAND umicom-mcp-tool-registration-test "${mode}" "${name}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;mcp;publication")
        umicom_test_impact_owners("${_name}" TARGETS umicom-mcp-tool-registration-test)
    endfunction()
    foreach(_case IN ITEMS count duplicate empty-name name description schema permission)
        _umicom_mcp_registration_case(catalogue "${_case}")
    endforeach()
    foreach(_case IN ITEMS id display endpoint prefix)
        _umicom_mcp_registration_case(server "${_case}")
    endforeach()
    foreach(_case IN ITEMS registry-count registry-text id-overflow permission-overflow late-collision allocation)
        _umicom_mcp_registration_case(refusal "${_case}")
    endforeach()
    foreach(_mode IN ITEMS descriptor boundary)
        foreach(_field IN ITEMS name description schema permission)
            _umicom_mcp_registration_case("${_mode}" "${_field}")
        endforeach()
    endforeach()
    foreach(_case IN ITEMS success explicit empty-prefix flags empty rebind rebind-empty full-catalogue)
        _umicom_mcp_registration_case(published "${_case}")
    endforeach()
    foreach(_case IN ITEMS exact overflow)
        _umicom_mcp_registration_case(capacity "${_case}")
    endforeach()
    foreach(_case IN ITEMS revision inactive borrowed-slot nulls)
        _umicom_mcp_registration_case(misc "${_case}")
    endforeach()
endif()
