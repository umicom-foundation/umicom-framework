# Umicom Framework
# File: cmake/UmicomAiWorkflowPreflight.cmake
#
# PURPOSE:
# Compose and export the shared typed workflow preflight service without a provider runtime.
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

function(umicom_add_ai_workflow_preflight)
    cmake_parse_arguments(PARSE_ARGV 0 PREFLIGHT "" "DEPENDENCY_TARGET" "")
    if(PREFLIGHT_UNPARSED_ARGUMENTS OR PREFLIGHT_KEYWORDS_MISSING_VALUES)
        message(FATAL_ERROR "Workflow preflight received unsupported or incomplete arguments")
    endif()
    if(NOT PREFLIGHT_DEPENDENCY_TARGET)
        set(PREFLIGHT_DEPENDENCY_TARGET Umicom::integration)
    endif()
    if(NOT TARGET "${PREFLIGHT_DEPENDENCY_TARGET}")
        message(FATAL_ERROR "Workflow preflight requires canonical integration graph services")
    endif()
    if(TARGET Umicom::ai_workflow_preflight)
        get_target_property(_imported Umicom::ai_workflow_preflight IMPORTED)
        get_target_property(_dependency Umicom::ai_workflow_preflight UMICOM_WORKFLOW_DEPENDENCY)
        if((_imported AND PREFLIGHT_DEPENDENCY_TARGET STREQUAL "Umicom::integration") OR
           _dependency STREQUAL PREFLIGHT_DEPENDENCY_TARGET)
            return()
        endif()
        message(FATAL_ERROR "Workflow preflight already exists with a different dependency")
    endif()
    if(TARGET umicom_ai_workflow_preflight)
        message(FATAL_ERROR "Workflow preflight target name is already in use")
    endif()
    get_filename_component(_root "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/.." ABSOLUTE)
    add_library(umicom_ai_workflow_preflight STATIC
        "${_root}/src/ai_workflow/preflight.c"
        "${_root}/src/ai_workflow/diagnostic.c"
        # Memory analysis shares canonical validation and ordering, never another graph owner.
        "${_root}/src/ai_workflow/memory_plan.c")
    add_library(Umicom::ai_workflow_preflight ALIAS umicom_ai_workflow_preflight)
    set_target_properties(umicom_ai_workflow_preflight PROPERTIES
        EXPORT_NAME ai_workflow_preflight
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO
        UMICOM_WORKFLOW_DEPENDENCY "${PREFLIGHT_DEPENDENCY_TARGET}")
    target_include_directories(umicom_ai_workflow_preflight PUBLIC
        $<BUILD_INTERFACE:${_root}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
    target_link_libraries(umicom_ai_workflow_preflight PUBLIC "${PREFLIGHT_DEPENDENCY_TARGET}")
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom_ai_workflow_preflight)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom_ai_workflow_preflight)
    endif()
    # A focused source study may supply its own graph target. Only the canonical
    # Framework composition participates in the public install/export set.
    if(PREFLIGHT_DEPENDENCY_TARGET STREQUAL "Umicom::integration")
        include("${CMAKE_CURRENT_FUNCTION_LIST_DIR}/UmicomSdkComponent.cmake")
        install(TARGETS umicom_ai_workflow_preflight EXPORT UmicomFrameworkTargets
            ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT Framework)
        umicom_register_sdk_libraries(umicom_ai_workflow_preflight)
        install(FILES "${_root}/include/umicom/ai_workflow/preflight.h"
            "${_root}/include/umicom/ai_workflow/memory_plan.h"
            DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/ai_workflow" COMPONENT Framework)
        foreach(_component Framework FrameworkSDK)
            set(_selection EXCLUDE_FROM_ALL)
            if(_component STREQUAL "Framework")
                set(_selection)
            endif()
            install(DIRECTORY "${_root}/examples/ai_workflow/"
                DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom-framework/examples/ai_workflow"
                COMPONENT "${_component}" ${_selection})
            install(DIRECTORY "${_root}/examples/ai_workflow_memory/"
                DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom-framework/examples/ai_workflow_memory"
                COMPONENT "${_component}" ${_selection})
        endforeach()
    endif()
endfunction()
