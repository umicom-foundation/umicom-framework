#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomAiWorkflowPreflight.cmake
#
# PURPOSE:
#   Compose typed preflight with an existing canonical planning dependency.
# EDUCATIONAL OVERVIEW:
#   Opt-in composition preserves existing build targets and source ownership.
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
# LICENCE:
#   MIT for this original Umicom contribution.
#-----------------------------------------------------------------------------

include_guard(GLOBAL)
include(CMakeParseArguments)

# Use the integration target in normal Framework/SDK builds. The explicit
# dependency argument exists only for the isolated canonical-source test slice.
function(umicom_add_ai_workflow_preflight)
    cmake_parse_arguments(PARSE_ARGV 0 UMI "" "DEPENDENCY_TARGET" "")
    if(UMI_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "Unknown workflow preflight composition arguments")
    endif()
    if(TARGET Umicom::ai_workflow_preflight)
        return()
    endif()
    if(NOT UMI_DEPENDENCY_TARGET)
        set(UMI_DEPENDENCY_TARGET Umicom::integration)
    endif()
    if(NOT TARGET "${UMI_DEPENDENCY_TARGET}")
        message(FATAL_ERROR "Workflow preflight requires canonical dependency ${UMI_DEPENDENCY_TARGET}")
    endif()
    set(root "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/..")
    add_library(umicom_ai_workflow_preflight STATIC "${root}/src/ai_workflow/preflight.c")
    add_library(Umicom::ai_workflow_preflight ALIAS umicom_ai_workflow_preflight)
    target_include_directories(umicom_ai_workflow_preflight PUBLIC "${root}/include")
    target_link_libraries(umicom_ai_workflow_preflight PUBLIC "${UMI_DEPENDENCY_TARGET}")
    set_target_properties(umicom_ai_workflow_preflight PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom_ai_workflow_preflight)
    endif()
endfunction()
