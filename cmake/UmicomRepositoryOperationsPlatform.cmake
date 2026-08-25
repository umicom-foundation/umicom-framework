#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomRepositoryOperationsPlatform.cmake
#
# PURPOSE:
#   Framework repository operation backend enabled
#
# ARCHITECTURE:
#   Extend the established umicom_repository target; no competing repository
#   library, VCS subsystem or maintenance model is created.
#
# Created by: Sammy Hegab
# Organisation: Umicom Foundation
# Licence: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)

set(UMICOM_FRAMEWORK_COMPLETION_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")
if(NOT TARGET umicom_repository)
    message(FATAL_ERROR "UmicomRepositoryOperationsPlatform.cmake requires umicom_repository.")
endif()

target_sources(umicom_repository PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/operations/repository_operation.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/operations/repository_operation_exit.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/operations/repository_operation_formatter.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/operations/repository_operation_request.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/operations/repository_operation_result.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/operations/repository_operation_service.c"
)

if(BUILD_TESTING)
    if(NOT COMMAND umicom_add_framework_completion_test)
        function(umicom_add_framework_completion_test target test_name source)
            if(TARGET "${target}")
                return()
            endif()
            add_executable("${target}" "${UMICOM_FRAMEWORK_COMPLETION_ROOT}/${source}")
            target_link_libraries("${target}" PRIVATE Umicom::Framework)
            if(COMMAND umicom_apply_warnings)
                umicom_apply_warnings("${target}")
            endif()
            if(COMMAND umicom_apply_sanitizers)
                umicom_apply_sanitizers("${target}")
            endif()
            add_test(NAME "${test_name}" COMMAND "${target}")
        endfunction()
    endif()

umicom_add_framework_completion_test(
    umicom-repository-operations-repository-operation-test
    framework.repository_operations.repository.operation
    tests/repository_operations/test_repository_operation.c)
umicom_add_framework_completion_test(
    umicom-repository-operations-repository-operation-exit-test
    framework.repository_operations.repository.operation.exit
    tests/repository_operations/test_repository_operation_exit.c)
umicom_add_framework_completion_test(
    umicom-repository-operations-repository-operation-formatter-test
    framework.repository_operations.repository.operation.formatter
    tests/repository_operations/test_repository_operation_formatter.c)
umicom_add_framework_completion_test(
    umicom-repository-operations-repository-operation-request-test
    framework.repository_operations.repository.operation.request
    tests/repository_operations/test_repository_operation_request.c)
umicom_add_framework_completion_test(
    umicom-repository-operations-repository-operation-result-test
    framework.repository_operations.repository.operation.result
    tests/repository_operations/test_repository_operation_result.c)
umicom_add_framework_completion_test(
    umicom-repository-operations-repository-operation-service-test
    framework.repository_operations.repository.operation.service
    tests/repository_operations/test_repository_operation_service.c)
endif()

message(STATUS "Framework repository operation backend enabled")
