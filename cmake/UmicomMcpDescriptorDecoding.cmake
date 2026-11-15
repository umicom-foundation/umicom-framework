# Umicom Framework
# File: cmake/UmicomMcpDescriptorDecoding.cmake
#
# PURPOSE:
# Register structured MCP descriptor and discovery integration regressions.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

if(BUILD_TESTING AND TARGET Umicom::ai)
    add_executable(umicom-mcp-descriptor-decoding-test
        tests/ai_mcp/test_descriptor_decoding.c
        tests/ai_mcp/test_initialize_allocation.c)
    target_link_libraries(umicom-mcp-descriptor-decoding-test PRIVATE Umicom::ai)
    set_target_properties(umicom-mcp-descriptor-decoding-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-mcp-descriptor-decoding-test)
    umicom_apply_sanitizers(umicom-mcp-descriptor-decoding-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-mcp-descriptor-decoding-test)
    endif()
    function(_umicom_mcp_descriptor_case kind mode name field)
        set(_name "framework.ai.mcp-descriptor.${kind}.${mode}.${name}.${field}")
        add_test(NAME "${_name}" COMMAND umicom-mcp-descriptor-decoding-test "${kind}" "${mode}" "${name}" "${field}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 30 LABELS "framework;ai;mcp;decoding")
        umicom_test_impact_owners("${_name}" TARGETS umicom-mcp-descriptor-decoding-test)
    endfunction()
    foreach(_kind IN ITEMS tool resource prompt)
        foreach(_case IN ITEMS minimal full unicode empty-identity missing-identity nested-identity wrong-identity duplicate-identity invalid-extension trailing root-array byte-last byte-overflow identity-boundary identity-overflow alias allocation nulls)
            _umicom_mcp_descriptor_case("${_kind}" codec "${_case}" all)
        endforeach()
        foreach(_case IN ITEMS missing-identity nested-identity wrong-identity duplicate-identity invalid-extension)
            _umicom_mcp_descriptor_case("${_kind}" discovery "${_case}" all)
        endforeach()
        if(_kind STREQUAL "tool")
            set(_fields description inputSchema)
        elseif(_kind STREQUAL "resource")
            set(_fields name description mimeType)
        else()
            set(_fields description arguments)
        endif()
        foreach(_field IN LISTS _fields)
            foreach(_case IN ITEMS wrong-kind duplicate nested empty unicode boundary overflow)
                _umicom_mcp_descriptor_case("${_kind}" codec "${_case}" "${_field}")
            endforeach()
            foreach(_case IN ITEMS wrong-kind overflow)
                _umicom_mcp_descriptor_case("${_kind}" discovery "${_case}" "${_field}")
            endforeach()
        endforeach()
    endforeach()
endif()
