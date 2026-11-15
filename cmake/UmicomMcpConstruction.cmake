# Umicom Framework
# File: cmake/UmicomMcpConstruction.cmake
#
# PURPOSE:
# Register staged MCP metadata, session and client construction regressions.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT


if(BUILD_TESTING AND TARGET Umicom::ai)
    add_executable(umicom-mcp-construction-test tests/ai_mcp/test_construction.c)
    target_link_libraries(umicom-mcp-construction-test PRIVATE Umicom::ai)
    set_target_properties(umicom-mcp-construction-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-mcp-construction-test)
    umicom_apply_sanitizers(umicom-mcp-construction-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-mcp-construction-test)
    endif()
    function(_umicom_mcp_construction_case mode name field)
        set(_name "framework.ai.mcp-construction.${mode}.${name}.${field}")
        add_test(NAME "${_name}" COMMAND umicom-mcp-construction-test "${mode}" "${name}" "${field}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 30 LABELS "framework;ai;mcp;construction")
        umicom_test_impact_owners("${_name}" TARGETS umicom-mcp-construction-test)
    endfunction()
    foreach(_case IN ITEMS basic untrusted verified trust-low trust-high alias-all cross-alias null-output)
        _umicom_mcp_construction_case(server "${_case}" all)
    endforeach()
    foreach(_field IN ITEMS id display endpoint)
        foreach(_case IN ITEMS null empty last unterminated alias)
            _umicom_mcp_construction_case(server "${_case}" "${_field}")
        endforeach()
    endforeach()
    foreach(_mode IN ITEMS session client)
        foreach(_case IN ITEMS basic null-server null-transport null-output no-request no-notify no-context self-context alias-server alias-transport alias-both metadata trust-low trust-high)
            _umicom_mcp_construction_case("${_mode}" "${_case}" all)
        endforeach()
        foreach(_field IN ITEMS id display endpoint permission)
            foreach(_case IN ITEMS empty last unterminated)
                _umicom_mcp_construction_case("${_mode}" "${_case}" "${_field}")
            endforeach()
        endforeach()
    endforeach()
endif()
