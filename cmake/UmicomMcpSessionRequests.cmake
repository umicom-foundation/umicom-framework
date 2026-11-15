# Umicom Framework
# File: cmake/UmicomMcpSessionRequests.cmake
#
# PURPOSE:
# Register ready-session reply publication and consumer regressions.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT


if(BUILD_TESTING AND TARGET Umicom::ai)
    add_executable(umicom-mcp-session-request-test
        tests/ai_mcp/test_session_requests.c
        tests/ai_mcp/test_session_request_allocation.c)
    target_link_libraries(umicom-mcp-session-request-test PRIVATE Umicom::ai)
    set_target_properties(umicom-mcp-session-request-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-mcp-session-request-test)
    umicom_apply_sanitizers(umicom-mcp-session-request-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-mcp-session-request-test)
    endif()
    function(_umicom_mcp_request_case mode name)
        set(_name "framework.ai.mcp-request.${mode}.${name}")
        add_test(NAME "${_name}" COMMAND umicom-mcp-session-request-test "${mode}" "${name}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 30 LABELS "framework;ai;mcp;lifetime")
        umicom_test_impact_owners("${_name}" TARGETS umicom-mcp-session-request-test)
    endfunction()
    foreach(_case IN ITEMS success error unterminated no-output empty last-byte not-ready missing-request revision-max allocation nested nested-retire nulls revision-ok revision-error state-ok state-error disconnect-ok disconnect-error request-ok request-error notify-ok notify-error data-ok data-error disconnect-unterminated)
        _umicom_mcp_request_case(direct "${_case}")
    endforeach()
    foreach(_mode IN ITEMS tool resource prompt)
        foreach(_case IN ITEMS success error unterminated disconnect-ok disconnect-error request-ok)
            _umicom_mcp_request_case("${_mode}" "${_case}")
        endforeach()
    endforeach()
    foreach(_case IN ITEMS success error unterminated disconnect-ok disconnect-error revision-ok)
        _umicom_mcp_request_case(ping "${_case}")
    endforeach()
    foreach(_mode IN ITEMS tools resources prompts)
        foreach(_case IN ITEMS success error unterminated disconnect-ok disconnect-error request-ok notify-ok data-ok)
            _umicom_mcp_request_case("${_mode}" "${_case}")
        endforeach()
    endforeach()
endif()
