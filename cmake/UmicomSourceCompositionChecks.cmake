#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomSourceCompositionChecks.cmake
#
# PURPOSE:
#   Link independent native services together to protect private symbol ownership.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#---------------------------------------------------------------------------

include_guard(GLOBAL)
if(NOT BUILD_TESTING OR NOT TARGET Umicom::source_contracts OR NOT TARGET Umicom::setup_centre)
    return()
endif()
get_filename_component(_umi_composition_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
set(_umi_composition_fixture "${CMAKE_CURRENT_BINARY_DIR}/source-setup-composition")
file(MAKE_DIRECTORY "${_umi_composition_fixture}")
configure_file("${_umi_composition_root}/tests/source_contracts/test_setup_composition.c"
    "${_umi_composition_fixture}/audit.c" COPYONLY)
file(WRITE "${_umi_composition_fixture}/suite.umi" "UMICOM_SUITE\t1\n")

# Both archive orders must resolve independently owned helpers. Each executable
# calls public services; no private implementation is copied into the fixture.
foreach(_order IN ITEMS source-first setup-first)
    set(_target "umicom-source-setup-${_order}-test")
    add_executable("${_target}" "${_umi_composition_root}/tests/source_contracts/test_setup_composition.c")
    if(_order STREQUAL "source-first")
        target_link_libraries("${_target}" PRIVATE Umicom::source_contracts Umicom::setup_centre)
    else()
        target_link_libraries("${_target}" PRIVATE Umicom::setup_centre Umicom::source_contracts)
    endif()
    set_target_properties("${_target}" PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings("${_target}")
    umicom_apply_sanitizers("${_target}")
    # Validation-target registration is optional in a plain Framework configure.
    # Retain the former unconditional call for review; CTest registration below
    # remains mandatory whenever this composition is available.
    if(FALSE)
        umicom_register_validation_target("${_target}")
    endif()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target("${_target}")
    endif()
    add_test(NAME "framework.source_contracts.setup_composition.${_order}"
        COMMAND "${_target}" "${_umi_composition_fixture}")
    set_tests_properties("framework.source_contracts.setup_composition.${_order}" PROPERTIES
        TIMEOUT 20 LABELS "framework;native;source-contracts;setup;regression")
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners("framework.source_contracts.setup_composition.${_order}"
            TARGETS "${_target}")
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs("${_target}" FILES
            "${_umi_composition_fixture}/audit.c" "${_umi_composition_fixture}/suite.umi")
    endif()
endforeach()
