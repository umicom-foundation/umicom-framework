/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_developer_cmake_plan.c
 *
 * PURPOSE:
 *   Verify structured CMake configure-build-test-run planning and dependency
 *   ordering without launching external processes.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>

#include "umicom/developer/cmake_plan.h"

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
    return 0;
}
