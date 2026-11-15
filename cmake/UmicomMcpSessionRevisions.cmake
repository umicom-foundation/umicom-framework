# Umicom Framework
# File: cmake/UmicomMcpSessionRevisions.cmake
#
# PURPOSE:
# Register MCP lifecycle revision and callback ownership regressions.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

if(BUILD_TESTING AND TARGET Umicom::ai)
    add_executable(umicom-mcp-session-revisions-test tests/ai_mcp/test_session_revisions.c)
    target_link_libraries(umicom-mcp-session-revisions-test PRIVATE Umicom::ai)
    set_target_properties(umicom-mcp-session-revisions-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-mcp-session-revisions-test)
    umicom_apply_sanitizers(umicom-mcp-session-revisions-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-mcp-session-revisions-test)
    endif()
    function(_umicom_mcp_session_revision_case mode name outcome)
        set(_name "framework.ai.mcp-session-revision.${mode}.${name}.${outcome}")
        add_test(NAME "${_name}" COMMAND umicom-mcp-session-revisions-test "${mode}" "${name}" "${outcome}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 30 LABELS "framework;ai;mcp;lifecycle")
        umicom_test_impact_owners("${_name}" TARGETS umicom-mcp-session-revisions-test)
    endfunction()
    foreach(_stage IN ITEMS transport notify)
        foreach(_action IN ITEMS revision exhaust disconnect state)
            foreach(_outcome IN ITEMS success error)
                _umicom_mcp_session_revision_case("${_stage}" "${_action}" "${_outcome}")
            endforeach()
        endforeach()
    endforeach()
    foreach(_case IN ITEMS success no-notify unsupported transport-error notify-error not-connected)
        _umicom_mcp_session_revision_case(normal "${_case}" all)
    endforeach()
    foreach(_case IN ITEMS initialize-max initialize-last disconnect-max legacy-disconnect-max disconnect-last legacy-disconnect-last)
        _umicom_mcp_session_revision_case(limit "${_case}" all)
    endforeach()
    _umicom_mcp_session_revision_case(nulls all all)
endif()
