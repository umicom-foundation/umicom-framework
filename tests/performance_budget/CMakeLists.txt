#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/performance_budget/CMakeLists.txt
#
# PURPOSE:
#   Isolate three existing budget owners and retain their prior archive regressions.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.25)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    project(UmicomBudgetAdmissionChecks LANGUAGES C)
    include(CTest)
    get_filename_component(_budget_root "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
    add_library(budget_subject STATIC
        "${_budget_root}/src/base/value_archive.c"
        "${_budget_root}/src/observability/performance/types.c"
        "${_budget_root}/src/observability/performance/memory_budget.c"
        "${_budget_root}/src/observability/performance/resource_budget.c"
        "${_budget_root}/src/observability/performance/performance_budget.c")
    target_include_directories(budget_subject PUBLIC "${_budget_root}/include")
    target_compile_features(budget_subject PUBLIC c_std_23)
    if(NOT WIN32)
        target_link_libraries(budget_subject PUBLIC m)
    endif()
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(budget_subject PRIVATE -Wall -Wextra -Wpedantic -Werror)
    endif()
    set(_budget_library budget_subject)
else()
    set(_budget_library umicom_observability)
endif()
set(_budget_kind 0)
foreach(_owner memory resource performance)
    set(_target "umicom-budget-${_owner}-admission-test")
    add_executable(${_target} test_budget_admission.c)
    target_compile_definitions(${_target} PRIVATE UMI_BUDGET_KIND=${_budget_kind})
    target_compile_features(${_target} PRIVATE c_std_23)
    target_link_libraries(${_target} PRIVATE ${_budget_library})
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${_target} PRIVATE -Wall -Wextra -Wpedantic -Werror)
    endif()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(${_target})
    endif()
    foreach(_case sequence nonfinite finite-budget identity validation finite-observation invalid-record)
        add_test(NAME framework.budget_admission.${_owner}.${_case} COMMAND ${_target} ${_case})
        set_tests_properties(framework.budget_admission.${_owner}.${_case} PROPERTIES TIMEOUT 20 LABELS "framework;observability;budget")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(framework.budget_admission.${_owner}.${_case} TARGETS ${_target})
        endif()
    endforeach()
    if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
        add_executable("budget-existing-${_owner}-test" "../observability_performance/test_${_owner}_budget.c")
        target_compile_features("budget-existing-${_owner}-test" PRIVATE c_std_23)
        target_link_libraries("budget-existing-${_owner}-test" PRIVATE ${_budget_library})
        add_test(NAME framework.budget_admission.${_owner}.existing COMMAND "budget-existing-${_owner}-test")
    endif()
    math(EXPR _budget_kind "${_budget_kind} + 1")
endforeach()
