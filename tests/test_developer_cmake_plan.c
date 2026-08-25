/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_developer_cmake_plan.c
 *
 * PURPOSE:
 *   Verify structured CMake configure-build-test-run and complete delivery
 *   planning with deterministic dependency ordering and no process launches.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>

#include "umicom/developer/cmake_plan.h"

static int operation_has_argument(
    const UmiDeveloperOperationSnapshot *operation,
    const char *argument)
{
    size_t index;

    if (operation == NULL || argument == NULL) {
        return 0;
    }
    for (index = 0U; index < operation->argument_count; ++index) {
        if (strcmp(operation->arguments[index], argument) == 0) {
            return 1;
        }
    }
    return 0;
}

static void verify_delivery_plan(void)
{
    UmiDeveloperPipeline *pipeline = NULL;
    UmiDeveloperCMakeDeliveryPlanRequest request;
    UmiDeveloperCMakeDeliveryPlanSnapshot plan;
    UmiDeveloperPipelineSnapshot pipeline_snapshot;
    UmiDeveloperOperationSnapshot operation;
    const char *expected_order[5];
    size_t index;

    assert(umi_developer_pipeline_create(&pipeline) == UMI_STATUS_OK);

    memset(&request, 0, sizeof(request));
    request.struct_size = (uint32_t)sizeof(request);
    request.api_version = UMI_DEVELOPER_CMAKE_DELIVERY_PLAN_API_VERSION;
    request.plan_id = "framework.delivery";
    request.project_id = "umicom-framework";
    request.configuration_id = "Release";
    request.source_directory = ".";
    request.build_directory = "build/windows-ucrt64-release";
    request.generator = "Ninja";
    request.test_filter = "framework\\.";
    request.install_prefix = "install/framework";
    request.package_generator = "ZIP";
    request.parallel_jobs = 4U;
    request.timeout_ms = 600000U;
    request.stage_flags = 0U;

    memset(&plan, 0, sizeof(plan));
    assert(umi_developer_cmake_delivery_plan_submit(
        pipeline, &request, &plan) == UMI_STATUS_OK);
    assert(plan.struct_size == (uint32_t)sizeof(plan));
    assert(plan.api_version == UMI_DEVELOPER_CMAKE_DELIVERY_PLAN_API_VERSION);
    assert(plan.operation_count == 5U);
    assert(strcmp(plan.configure_operation_id, "framework.delivery.configure") == 0);
    assert(strcmp(plan.build_operation_id, "framework.delivery.build") == 0);
    assert(strcmp(plan.test_operation_id, "framework.delivery.test") == 0);
    assert(strcmp(plan.install_operation_id, "framework.delivery.install") == 0);
    assert(strcmp(plan.package_operation_id, "framework.delivery.package") == 0);

    assert(umi_developer_pipeline_snapshot(pipeline, &pipeline_snapshot) ==
           UMI_STATUS_OK);
    assert(pipeline_snapshot.operation_count == 5U);
    assert(pipeline_snapshot.dependency_count == 4U);

    assert(umi_developer_pipeline_find(
        pipeline, plan.build_operation_id, &operation) == UMI_STATUS_OK);
    assert(operation.kind == UMI_DEVELOPER_OPERATION_BUILD);
    assert(strcmp(operation.program, "cmake") == 0);
    assert(operation_has_argument(&operation, "--parallel"));
    assert(operation_has_argument(&operation, "4"));
    assert(operation_has_argument(&operation, "--config"));
    assert(operation_has_argument(&operation, "Release"));

    assert(umi_developer_pipeline_find(
        pipeline, plan.test_operation_id, &operation) == UMI_STATUS_OK);
    assert(operation.kind == UMI_DEVELOPER_OPERATION_TEST);
    assert(strcmp(operation.program, "ctest") == 0);
    assert(operation_has_argument(&operation, "-R"));
    assert(operation_has_argument(&operation, "framework\\."));

    assert(umi_developer_pipeline_find(
        pipeline, plan.install_operation_id, &operation) == UMI_STATUS_OK);
    assert(operation.kind == UMI_DEVELOPER_OPERATION_CUSTOM);
    assert(strcmp(operation.program, "cmake") == 0);
    assert(operation_has_argument(&operation, "--install"));
    assert(operation_has_argument(&operation, "--prefix"));
    assert(operation_has_argument(&operation, "install/framework"));

    assert(umi_developer_pipeline_find(
        pipeline, plan.package_operation_id, &operation) == UMI_STATUS_OK);
    assert(operation.kind == UMI_DEVELOPER_OPERATION_PACKAGE);
    assert(strcmp(operation.program, "cpack") == 0);
    assert(strcmp(operation.working_directory,
                  "build/windows-ucrt64-release") == 0);
    assert(operation_has_argument(&operation, "CPackConfig.cmake"));
    assert(operation_has_argument(&operation, "ZIP"));

    expected_order[0] = plan.configure_operation_id;
    expected_order[1] = plan.build_operation_id;
    expected_order[2] = plan.test_operation_id;
    expected_order[3] = plan.install_operation_id;
    expected_order[4] = plan.package_operation_id;

    for (index = 0U; index < 5U; ++index) {
        assert(umi_developer_pipeline_next_ready(pipeline, &operation) ==
               UMI_STATUS_OK);
        assert(strcmp(operation.id, expected_order[index]) == 0);
        assert(umi_developer_pipeline_start(pipeline, operation.id) ==
               UMI_STATUS_OK);
        assert(umi_developer_pipeline_complete(
            pipeline, operation.id, 0, "stage complete") == UMI_STATUS_OK);
    }

    assert(umi_developer_cmake_delivery_plan_submit(
        pipeline, &request, NULL) == UMI_STATUS_ALREADY_EXISTS);
    umi_developer_pipeline_destroy(pipeline);

    pipeline = NULL;
    assert(umi_developer_pipeline_create(&pipeline) == UMI_STATUS_OK);
    request.plan_id = "framework.package-only";
    request.stage_flags = (uint32_t)UMI_DEVELOPER_CMAKE_DELIVERY_PACKAGE;
    memset(&plan, 0, sizeof(plan));
    assert(umi_developer_cmake_delivery_plan_submit(
        pipeline, &request, &plan) == UMI_STATUS_OK);
    assert(plan.operation_count == 3U);
    assert(plan.test_operation_id[0] == '\0');
    assert(plan.install_operation_id[0] == '\0');
    assert(plan.package_operation_id[0] != '\0');
    assert(umi_developer_pipeline_snapshot(pipeline, &pipeline_snapshot) ==
           UMI_STATUS_OK);
    assert(pipeline_snapshot.dependency_count == 2U);
    umi_developer_pipeline_destroy(pipeline);

    pipeline = NULL;
    assert(umi_developer_pipeline_create(&pipeline) == UMI_STATUS_OK);
    request.plan_id = "framework.invalid-delivery";
    request.stage_flags = 1U << 31;
    assert(umi_developer_cmake_delivery_plan_submit(
        pipeline, &request, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    assert(umi_developer_pipeline_count(pipeline) == 0U);
    umi_developer_pipeline_destroy(pipeline);
}

int main(void)
{
    UmiDeveloperPipeline *pipeline = NULL;
    UmiDeveloperCMakePlanRequest request;
    UmiDeveloperCMakePlanSnapshot plan;
    UmiDeveloperOperationSnapshot operation;
    const char *run_arguments[] = {"--console"};

    assert(umi_developer_pipeline_create(&pipeline) == UMI_STATUS_OK);

    memset(&request, 0, sizeof(request));
    request.struct_size = (uint32_t)sizeof(request);
    request.api_version = UMI_DEVELOPER_CMAKE_PLAN_API_VERSION;
    request.plan_id = "studio.debug";
    request.project_id = "umicom-studio";
    request.configuration_id = "Debug";
    request.source_directory = "C:/Dev/umicom/umicom-studio";
    request.build_directory = "C:/Dev/umicom/umicom-studio/build";
    request.generator = "Ninja";
    request.parallel_jobs = 4U;
    request.timeout_ms = 600000U;
    request.include_configure = 1;
    request.include_build = 1;
    request.include_tests = 1;
    request.include_run = 1;
    request.run_program = "build/bin/umicom-studio-ide";
    request.run_arguments = run_arguments;
    request.run_argument_count = 1U;

    assert(umi_developer_cmake_plan_submit(pipeline, &request, &plan) ==
           UMI_STATUS_OK);
    assert(plan.operation_count == 4U);
    assert(umi_developer_pipeline_count(pipeline) == 4U);

    assert(umi_developer_pipeline_find(
        pipeline, plan.configure_operation_id, &operation) == UMI_STATUS_OK);
    assert(strcmp(operation.program, "cmake") == 0);
    assert(operation.argument_count == 7U);
    assert(strcmp(operation.arguments[0], "-S") == 0);

    assert(umi_developer_pipeline_next_ready(pipeline, &operation) ==
           UMI_STATUS_OK);
    assert(strcmp(operation.id, plan.configure_operation_id) == 0);

    assert(umi_developer_pipeline_start(pipeline, operation.id) == UMI_STATUS_OK);
    assert(umi_developer_pipeline_complete(
        pipeline, operation.id, 0, "configured") == UMI_STATUS_OK);

    assert(umi_developer_pipeline_next_ready(pipeline, &operation) ==
           UMI_STATUS_OK);
    assert(strcmp(operation.id, plan.build_operation_id) == 0);

    assert(umi_developer_cmake_plan_submit(pipeline, &request, NULL) ==
           UMI_STATUS_ALREADY_EXISTS);

    umi_developer_pipeline_destroy(pipeline);
    verify_delivery_plan();
    return 0;
}
