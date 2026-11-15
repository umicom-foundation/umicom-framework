# Umicom Framework
# File: cmake/UmicomMcpServerRegistryAdmission.cmake
#
# PURPOSE:
# Register complete MCP server registry admission and revision checks.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT


if(BUILD_TESTING AND TARGET Umicom::ai)
    add_executable(umicom-mcp-server-registry-test tests/ai_mcp/test_server_registry_admission.c)
    target_link_libraries(umicom-mcp-server-registry-test PRIVATE Umicom::ai)
    set_target_properties(umicom-mcp-server-registry-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-mcp-server-registry-test)
    umicom_apply_sanitizers(umicom-mcp-server-registry-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-mcp-server-registry-test)
    endif()
    function(_umicom_mcp_server_registry_case name field)
        set(_name "framework.ai.mcp-server-registry.${name}.${field}")
        add_test(NAME "${_name}" COMMAND umicom-mcp-server-registry-test "${name}" "${field}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 30 LABELS "framework;ai;mcp;registry")
        umicom_test_impact_owners("${_name}" TARGETS umicom-mcp-server-registry-test)
    endfunction()
    foreach(_case IN ITEMS empty resolve mutable missing count-overflow count-max duplicate-active full full-duplicate unused alias-inactive duplicate-candidate trust-candidate trust-existing query-empty query-overflow query-boundary nulls remove-first remove-middle remove-last remove-alias remove-missing revision-max revision-add-last revision-remove-last)
        _umicom_mcp_server_registry_case("${_case}" all)
    endforeach()
    foreach(_field IN ITEMS id display endpoint permission)
        foreach(_case IN ITEMS candidate-unterminated candidate-last existing-first existing-last empty)
            _umicom_mcp_server_registry_case("${_case}" "${_field}")
        endforeach()
    endforeach()
endif()
