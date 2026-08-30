#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomWebApiWorkbenchPlatform.cmake
#
# PURPOSE:
#   Compose existing web, Integration Fabric, Security and Build capabilities
#   into the Framework-owned HTTP/API/Web/Cloud developer workbench.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)

set(UMICOM_WEB_API_WORKBENCH_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")

if(NOT TARGET umicom_developer OR NOT TARGET umicom_web OR
   NOT TARGET umicom_integration OR NOT TARGET umicom_build OR
   NOT TARGET umicom_security)
    message(FATAL_ERROR
        "Web API Workbench requires developer, web, integration, build and security targets")
endif()

target_sources(umicom_developer PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/web/workbench/types.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/web/workbench/environment.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/web/workbench/request.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/web/workbench/auth_profile.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/web/workbench/response.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/web/workbench/transport.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/web/workbench/history.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/web/workbench/collection.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/web/workbench/openapi.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/web/workbench/soap.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/web/workbench/stream.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/web/workbench/server_diagnostics.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/web/workbench/cloud_object.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/web/workbench/cloud_queue.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/web/workbench/deployment_projection.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/web/workbench/command_catalogue.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/web/workbench/runtime.c"
)

target_link_libraries(umicom_developer PUBLIC
    Umicom::web
    Umicom::integration
    Umicom::build
    Umicom::security
)

if(BUILD_TESTING)
    function(umicom_add_web_api_workbench_test target test_name source)
        if(TARGET "${target}")
            return()
        endif()
        add_executable("${target}" "${UMICOM_WEB_API_WORKBENCH_ROOT}/${source}")
        target_link_libraries("${target}" PRIVATE Umicom::Framework)
        if(COMMAND umicom_apply_warnings)
            umicom_apply_warnings("${target}")
        endif()
        if(COMMAND umicom_apply_sanitizers)
            umicom_apply_sanitizers("${target}")
        endif()
        add_test(NAME "${test_name}" COMMAND "${target}")
        set_tests_properties("${test_name}" PROPERTIES
            LABELS "framework;web-api-workbench;http;openapi;cloud")
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target("${target}")
        endif()
    endfunction()

    umicom_add_web_api_workbench_test(umicom-web-workbench-types-test
        framework.web_api_workbench.types tests/web_workbench/test_types.c)
    umicom_add_web_api_workbench_test(umicom-web-workbench-environment-test
        framework.web_api_workbench.environment tests/web_workbench/test_environment.c)
    umicom_add_web_api_workbench_test(umicom-web-workbench-request-test
        framework.web_api_workbench.request tests/web_workbench/test_request.c)
    umicom_add_web_api_workbench_test(umicom-web-workbench-auth-test
        framework.web_api_workbench.auth tests/web_workbench/test_auth_profile.c)
    umicom_add_web_api_workbench_test(umicom-web-workbench-response-test
        framework.web_api_workbench.response tests/web_workbench/test_response.c)
    umicom_add_web_api_workbench_test(umicom-web-workbench-transport-test
        framework.web_api_workbench.transport tests/web_workbench/test_transport.c)
    umicom_add_web_api_workbench_test(umicom-web-workbench-history-test
        framework.web_api_workbench.history tests/web_workbench/test_history.c)
    umicom_add_web_api_workbench_test(umicom-web-workbench-collection-test
        framework.web_api_workbench.collection tests/web_workbench/test_collection.c)
    umicom_add_web_api_workbench_test(umicom-web-workbench-openapi-test
        framework.web_api_workbench.openapi tests/web_workbench/test_openapi.c)
    umicom_add_web_api_workbench_test(umicom-web-workbench-soap-test
        framework.web_api_workbench.soap tests/web_workbench/test_soap.c)
    umicom_add_web_api_workbench_test(umicom-web-workbench-stream-test
        framework.web_api_workbench.stream tests/web_workbench/test_stream.c)
    umicom_add_web_api_workbench_test(umicom-web-workbench-server-test
        framework.web_api_workbench.server_diagnostics tests/web_workbench/test_server_diagnostics.c)
    umicom_add_web_api_workbench_test(umicom-web-workbench-cloud-object-test
        framework.web_api_workbench.cloud_object tests/web_workbench/test_cloud_object.c)
    umicom_add_web_api_workbench_test(umicom-web-workbench-cloud-queue-test
        framework.web_api_workbench.cloud_queue tests/web_workbench/test_cloud_queue.c)
    umicom_add_web_api_workbench_test(umicom-web-workbench-deployment-test
        framework.web_api_workbench.deployment tests/web_workbench/test_deployment_projection.c)
    umicom_add_web_api_workbench_test(umicom-web-workbench-commands-test
        framework.web_api_workbench.commands tests/web_workbench/test_command_catalogue.c)
    umicom_add_web_api_workbench_test(umicom-web-workbench-runtime-test
        framework.web_api_workbench.runtime tests/web_workbench/test_runtime.c)
endif()
