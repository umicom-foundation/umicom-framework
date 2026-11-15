# Umicom Framework
# File: cmake/UmicomMcpInitializationAdmission.cmake
#
# PURPOSE:
# Register complete MCP negotiation and protocol-only JSON consumers.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

include_guard(GLOBAL)
if(BUILD_TESTING AND TARGET Umicom::ai)
    add_executable(umicom-mcp-initialization-admission-test
        tests/ai_mcp/test_initialize_admission.c tests/ai_mcp/test_initialize_allocation.c)
    target_link_libraries(umicom-mcp-initialization-admission-test PRIVATE Umicom::ai)
    set_target_properties(umicom-mcp-initialization-admission-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-mcp-initialization-admission-test)
    umicom_apply_sanitizers(umicom-mcp-initialization-admission-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-mcp-initialization-admission-test)
    endif()
    function(_umicom_mcp_initialize_case mode name)
        set(_name "framework.ai.mcp-initialize.${mode}.${name}")
        add_test(NAME "${_name}" COMMAND umicom-mcp-initialization-admission-test "${mode}" "${name}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 30 LABELS "framework;ai;mcp;negotiation")
        umicom_test_impact_owners("${_name}" TARGETS umicom-mcp-initialization-admission-test)
    endfunction()
    foreach(_case IN ITEMS
        valid
        empty-capabilities
        nested-capabilities
        root-capabilities
        unknown-duplicates
        unicode
        escaped-keys
        empty-version
        other-version
        empty-object
        array-root
        scalar-root
        missing-protocol
        nested-protocol
        protocol-kind
        protocol-empty
        missing-capabilities
        capabilities-kind
        missing-server
        server-kind
        missing-name
        name-empty
        name-kind
        missing-version
        version-kind
        duplicate-protocol
        duplicate-capabilities
        duplicate-server
        duplicate-name
        duplicate-version
        invalid-extension
        nul-extension
        surrogate-extension
        trailing
        truncated
        tools-kind
        tools-duplicate
        resources-kind
        resources-duplicate
        prompts-kind
        prompts-duplicate
        logging-kind
        logging-duplicate
        protocol-boundary
        protocol-overflow
        name-boundary
        name-overflow
        version-boundary
        version-overflow
        byte-last
        byte-overflow
        depth-overflow
        invalid-utf8)
        _umicom_mcp_initialize_case(codec "${_case}")
        _umicom_mcp_initialize_case(session "${_case}")
    endforeach()
    foreach(_case IN ITEMS nulls allocation alias)
        _umicom_mcp_initialize_case(special "${_case}")
    endforeach()
    # Reuse the established corpus with only the protocol dependency. A developer
    # archive must no longer be necessary to link the generic JSON implementation.
    add_executable(umicom-protocol-json-tree-test tests/json_tree/test_tree.c)
    target_link_libraries(umicom-protocol-json-tree-test PRIVATE Umicom::protocol)
    set_target_properties(umicom-protocol-json-tree-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-protocol-json-tree-test)
    umicom_apply_sanitizers(umicom-protocol-json-tree-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-protocol-json-tree-test)
    endif()
    foreach(_case IN ITEMS null ownership grammar unicode integers booleans bounds depth cancellation members siblings span)
        set(_name "framework.protocol.json-tree.${_case}")
        add_test(NAME "${_name}" COMMAND umicom-protocol-json-tree-test "${_case}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 30 LABELS "framework;protocol;json;ownership")
        umicom_test_impact_owners("${_name}" TARGETS umicom-protocol-json-tree-test)
    endforeach()
endif()

# Configuration checks source ownership independently of whether tests are enabled.
# This catches accidental duplicate compilation or a missing protocol source.
foreach(_json_source IN ITEMS json_text.c json_tree.c)
    file(REAL_PATH "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/${_json_source}" _expected_source)
    foreach(_owner IN ITEMS umicom_protocol umicom_developer)
        get_target_property(_owner_directory "${_owner}" SOURCE_DIR)
        get_target_property(_owner_sources "${_owner}" SOURCES)
        set(_matches 0)
        foreach(_source IN LISTS _owner_sources)
            if(_source MATCHES "\\$<")
                continue()
            endif()
            file(REAL_PATH "${_source}" _actual_source BASE_DIRECTORY "${_owner_directory}")
            if(_actual_source STREQUAL _expected_source)
                math(EXPR _matches "${_matches} + 1")
            endif()
        endforeach()
        if((_owner STREQUAL "umicom_protocol" AND NOT _matches EQUAL 1) OR
           (_owner STREQUAL "umicom_developer" AND NOT _matches EQUAL 0))
            message(FATAL_ERROR "Unexpected shared JSON source ownership: ${_owner} / ${_json_source} / ${_matches}")
        endif()
    endforeach()
endforeach()
