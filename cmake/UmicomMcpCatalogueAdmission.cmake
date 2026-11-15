# Umicom Framework
# File: cmake/UmicomMcpCatalogueAdmission.cmake
#
# PURPOSE:
# Register bounded resource and prompt catalogue regression coverage.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

if(BUILD_TESTING AND TARGET Umicom::ai)
    add_executable(umicom-mcp-catalogue-admission-test tests/ai_mcp/test_catalogue_admission.c)
    target_link_libraries(umicom-mcp-catalogue-admission-test PRIVATE Umicom::ai)
    set_target_properties(umicom-mcp-catalogue-admission-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-mcp-catalogue-admission-test)
    umicom_apply_sanitizers(umicom-mcp-catalogue-admission-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-mcp-catalogue-admission-test)
    endif()
    function(_umicom_mcp_catalogue_case kind name field)
        set(_name "framework.ai.mcp-catalogue.${kind}.${name}.${field}")
        add_test(NAME "${_name}" COMMAND umicom-mcp-catalogue-admission-test "${kind}" "${name}" "${field}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 30 LABELS "framework;ai;mcp;catalogue")
        umicom_test_impact_owners("${_name}" TARGETS umicom-mcp-catalogue-admission-test)
    endfunction()
    foreach(_kind IN ITEMS resource prompt)
        foreach(_case IN ITEMS empty resolve count-overflow count-max duplicate-active empty-active duplicate-candidate empty-candidate full unused alias-inactive revision-max revision-last query-boundary query-overflow nulls)
            _umicom_mcp_catalogue_case("${_kind}" "${_case}" all)
        endforeach()
        if(_kind STREQUAL "resource")
            set(_fields identity name description mime_type)
        else()
            set(_fields identity description arguments_schema)
        endif()
        foreach(_field IN LISTS _fields)
            foreach(_case IN ITEMS candidate-unterminated candidate-boundary existing-early existing-late)
                _umicom_mcp_catalogue_case("${_kind}" "${_case}" "${_field}")
            endforeach()
        endforeach()
    endforeach()
endif()
