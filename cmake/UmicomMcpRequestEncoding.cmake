# Umicom Framework
# File: cmake/UmicomMcpRequestEncoding.cmake
#
# PURPOSE:
# Register transactional MCP parameter and escaping regressions.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT


if(BUILD_TESTING AND TARGET Umicom::ai)
    add_executable(umicom-mcp-request-encoding-test
        tests/ai_mcp/test_request_encoding.c
        tests/ai_mcp/test_request_allocation.c
        tests/ai_mcp/test_initialize_allocation.c)
    target_link_libraries(umicom-mcp-request-encoding-test PRIVATE Umicom::ai)
    set_target_properties(umicom-mcp-request-encoding-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-mcp-request-encoding-test)
    umicom_apply_sanitizers(umicom-mcp-request-encoding-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-mcp-request-encoding-test)
    endif()
    function(_umicom_mcp_encoding_case kind name)
        set(_name "framework.ai.mcp-encoding.${kind}.${name}")
        add_test(NAME "${_name}" COMMAND umicom-mcp-request-encoding-test "${kind}" "${name}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 30 LABELS "framework;ai;mcp;encoding")
        umicom_test_impact_owners("${_name}" TARGETS umicom-mcp-request-encoding-test)
    endfunction()
    foreach(_kind IN ITEMS escape initialize tool resource prompt)
        foreach(_case IN ITEMS basic empty escaped unicode invalid-utf8 exact short alias field-last field-overflow nulls allocation)
            _umicom_mcp_encoding_case("${_kind}" "${_case}")
        endforeach()
    endforeach()
    foreach(_byte RANGE 1 31)
        _umicom_mcp_encoding_case(escape "control-${_byte}")
    endforeach()
    foreach(_case IN ITEMS utf8-surrogate utf8-truncated utf8-continuation utf8-above-max utf8-five-byte)
        _umicom_mcp_encoding_case(escape "${_case}")
    endforeach()
    foreach(_case IN ITEMS version-last version-overflow version-alias version-utf8)
        _umicom_mcp_encoding_case(initialize "${_case}")
    endforeach()
    foreach(_kind IN ITEMS tool prompt)
        foreach(_case IN ITEMS raw-empty raw-spelling raw-duplicate raw-alias raw-allocation raw-byte-last raw-byte-overflow raw-depth raw-array raw-string raw-number raw-null raw-trailing raw-unclosed raw-injection raw-escape raw-whitespace)
            _umicom_mcp_encoding_case("${_kind}" "${_case}")
        endforeach()
    endforeach()
endif()
