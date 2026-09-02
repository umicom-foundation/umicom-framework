#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomApplicationProductisationContribution.cmake
#
# PURPOSE:
#   Provide one build contract for thin application productisation
#   contributions so every module uses the same Framework linkage, warnings,
#   sanitizers, test registration and install behaviour.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
include(CMakeParseArguments)

# Define the add application productisation contribution build helper so parent and
# application projects apply one consistent rule.
function(umicom_add_application_productisation_contribution)
    set(options)
    set(one_value_arguments NAME TARGET SOURCE TEST_SOURCE)
    set(multi_value_arguments)
    cmake_parse_arguments(
        UMI_CONTRIBUTION
        "${options}"
        "${one_value_arguments}"
        "${multi_value_arguments}"
        ${ARGN})

    # Visit each bounded item once so every record receives the same rule.
    foreach(required_argument NAME TARGET SOURCE TEST_SOURCE)
        # Apply this branch only when its contract condition is satisfied.
        if(NOT UMI_CONTRIBUTION_${required_argument})
            message(FATAL_ERROR
                "Application productisation contribution requires ${required_argument}")
        endif()
    endforeach()
    # Load the dependency only when the parent build has not already provided its target.
    if(NOT TARGET Umicom::application)
        message(FATAL_ERROR
            "${UMI_CONTRIBUTION_NAME} requires Framework target Umicom::application")
    endif()
    # Configure the optional target only when its feature has created it.
    if(TARGET "${UMI_CONTRIBUTION_TARGET}")
        message(FATAL_ERROR
            "Duplicate application contribution target '${UMI_CONTRIBUTION_TARGET}'")
    endif()

    add_library("${UMI_CONTRIBUTION_TARGET}" STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/${UMI_CONTRIBUTION_SOURCE}")
    target_include_directories("${UMI_CONTRIBUTION_TARGET}" PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
    target_link_libraries("${UMI_CONTRIBUTION_TARGET}" PUBLIC
        Umicom::application)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings("${UMI_CONTRIBUTION_TARGET}")
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers("${UMI_CONTRIBUTION_TARGET}")
    endif()

    set_property(GLOBAL APPEND PROPERTY
        UMICOM_APPLICATION_PRODUCTISATION_CONTRIBUTIONS
        "${UMI_CONTRIBUTION_TARGET}")

    # Register verification targets only when the developer has enabled testing.
    if(BUILD_TESTING)
        set(test_target "${UMI_CONTRIBUTION_TARGET}-test")
        add_executable("${test_target}"
            "${CMAKE_CURRENT_SOURCE_DIR}/${UMI_CONTRIBUTION_TEST_SOURCE}")
        target_link_libraries("${test_target}" PRIVATE
            "${UMI_CONTRIBUTION_TARGET}")
        # Use the shared build helper when it is available from the parent composition.
        if(COMMAND umicom_apply_warnings)
            umicom_apply_warnings("${test_target}")
        endif()
        # Use the shared build helper when it is available from the parent composition.
        if(COMMAND umicom_apply_sanitizers)
            umicom_apply_sanitizers("${test_target}")
        endif()
        add_test(
            NAME "${UMI_CONTRIBUTION_NAME}.module.productisation"
            COMMAND "${test_target}")
        set_tests_properties(
            "${UMI_CONTRIBUTION_NAME}.module.productisation"
            PROPERTIES LABELS
                "application;productisation;adoption;${UMI_CONTRIBUTION_NAME}")
        # Use the shared build helper when it is available from the parent composition.
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target("${test_target}")
        endif()
    endif()

    install(TARGETS "${UMI_CONTRIBUTION_TARGET}"
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endfunction()
