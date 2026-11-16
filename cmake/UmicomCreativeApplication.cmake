# Umicom Framework
# File: cmake/UmicomCreativeApplication.cmake
#
# PURPOSE:
#   Attach an existing creative workspace component and its example without compiling Framework implementation sources.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

include_guard(GLOBAL)
include(GNUInstallDirs)
include(CMakeParseArguments)
function(umicom_creative_configure_target target)
    set_target_properties(${target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(${target})
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(${target})
    endif()
endfunction()

# Product profile selection reuses the exported library in both dependency modes.
# Validate the full attachment before changing the consumer target's properties.
function(umicom_attach_creative_workspace)
    cmake_parse_arguments(PARSE_ARGV 0 UMI "" "NAME;TARGET" "")
    if(UMI_UNPARSED_ARGUMENTS OR UMI_KEYWORDS_MISSING_VALUES OR NOT UMI_NAME OR
       NOT UMI_TARGET OR NOT TARGET "${UMI_TARGET}")
        message(FATAL_ERROR "Creative attachment requires NAME and an existing TARGET")
    endif()
    if(NOT TARGET Umicom::creative_workspace)
        message(FATAL_ERROR "Creative attachment requires the Framework creative_workspace component")
    endif()
    if(NOT UMI_NAME MATCHES "^(media|music|cad|kitchen|games|web-studio|mobile-studio)$")
        message(FATAL_ERROR "Unknown creative profile: ${UMI_NAME}")
    endif()
    get_target_property(attached "${UMI_TARGET}" UMICOM_CREATIVE_WORKSPACE_ATTACHED)
    if(attached)
        message(FATAL_ERROR "Creative workspace already attached to ${UMI_TARGET}")
    endif()
    set(example "umicom-${UMI_NAME}-creative-workspace-example")
    if(TARGET "${example}")
        message(FATAL_ERROR "Creative example target already exists: ${example}")
    endif()
    get_target_property(_imported Umicom::creative_workspace IMPORTED)
    if(_imported)
        # The installed package publishes its relocatable data-file location.
        # Never fall back to a nearby source checkout for an imported component.
        set(_source "${UMICOM_FRAMEWORK_CREATIVE_EXAMPLE_SOURCE}")
    else()
        set(_source "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../examples/creative_workspace_main.c")
    endif()
    if(NOT IS_ABSOLUTE "${_source}" OR NOT EXISTS "${_source}" OR IS_DIRECTORY "${_source}")
        message(FATAL_ERROR "Creative application example source is missing from the selected Framework SDK or source tree")
    endif()
    set_target_properties("${UMI_TARGET}" PROPERTIES
        UMICOM_CREATIVE_WORKSPACE_ATTACHED TRUE UMICOM_CREATIVE_WORKSPACE_PROFILE "${UMI_NAME}")
    target_link_libraries("${UMI_TARGET}" PUBLIC Umicom::creative_workspace)
    add_executable("${example}" "${_source}")
    target_compile_definitions("${example}" PRIVATE UMICOM_CREATIVE_PROFILE="${UMI_NAME}")
    target_link_libraries("${example}" PRIVATE Umicom::creative_workspace)
    umicom_creative_configure_target("${example}")
    install(TARGETS "${example}" RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
    if(BUILD_TESTING)
        add_test(NAME "${UMI_NAME}.creative_workspace.example" COMMAND "${example}")
        set_tests_properties("${UMI_NAME}.creative_workspace.example" PROPERTIES
            TIMEOUT 30 LABELS "creative-workspace;example")
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target("${example}")
        endif()
    endif()
endfunction()
