# Umicom Framework
# File: cmake/UmicomMcpTransportPublication.cmake
#
# PURPOSE:
# Register bounded MCP result text and checked response publication tests.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

if(BUILD_TESTING AND TARGET Umicom::ai)
    add_executable(umicom-mcp-checked-transport-test
        tests/ai_mcp/test_checked_transport.c
        tests/ai_mcp/test_transport_allocation.c)
    target_link_libraries(umicom-mcp-checked-transport-test PRIVATE Umicom::ai)
    set_target_properties(umicom-mcp-checked-transport-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-mcp-checked-transport-test)
    umicom_apply_sanitizers(umicom-mcp-checked-transport-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-mcp-checked-transport-test)
    endif()
    foreach(_case IN ITEMS success empty one-byte last-byte malformed no-write partial error error-malformed tuple-change alias-input allocation legacy-error)
        set(_name "framework.ai.mcp-transport.unit.${_case}")
        add_test(NAME "${_name}" COMMAND umicom-mcp-checked-transport-test unit "${_case}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;mcp;publication")
        umicom_test_impact_owners("${_name}" TARGETS umicom-mcp-checked-transport-test)
    endforeach()
    foreach(_entry IN ITEMS tool resource prompt discovery initialize ping)
        foreach(_outcome IN ITEMS success malformed error)
            set(_name "framework.ai.mcp-transport.consumer.${_entry}.${_outcome}")
            add_test(NAME "${_name}" COMMAND umicom-mcp-checked-transport-test consumer "${_entry}" "${_outcome}")
            set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;mcp;publication")
            umicom_test_impact_owners("${_name}" TARGETS umicom-mcp-checked-transport-test)
        endforeach()
    endforeach()
    add_test(NAME framework.ai.mcp-transport.nulls COMMAND umicom-mcp-checked-transport-test nulls all)
    set_tests_properties(framework.ai.mcp-transport.nulls PROPERTIES TIMEOUT 45 LABELS "framework;ai;mcp;publication")
    umicom_test_impact_owners(framework.ai.mcp-transport.nulls TARGETS umicom-mcp-checked-transport-test)
endif()
