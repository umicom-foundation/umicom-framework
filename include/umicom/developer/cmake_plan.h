/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/cmake_plan.h
 *
 * PURPOSE:
 *   Build a dependency-safe CMake/Ninja development plan using structured
 *   configure, build, test and optional run operations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This helper does not execute CMake.  It creates ordinary developer
 * operations in the reusable pipeline.  Applications can inspect or modify the
 * plan before a worker executes it, and the same dependency chain is visible
 * to progress views, logs and automation.
 */
#ifndef UMICOM_DEVELOPER_CMAKE_PLAN_H
#define UMICOM_DEVELOPER_CMAKE_PLAN_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/developer/pipeline.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CMAKE_PLAN_API_VERSION 1U

typedef struct UmiDeveloperCMakePlanRequest {
    uint32_t struct_size;
    uint32_t api_version;
    const char *plan_id;
    const char *project_id;
    const char *configuration_id;
    const char *source_directory;
    const char *build_directory;
    const char *generator;
    uint32_t parallel_jobs;
    uint32_t timeout_ms;
    int include_configure;
    int include_build;
    int include_tests;
    int include_run;
    const char *run_program;
    const char *const *run_arguments;
    size_t run_argument_count;
} UmiDeveloperCMakePlanRequest;

typedef struct UmiDeveloperCMakePlanSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char configure_operation_id[UMI_DEVELOPER_ID_CAPACITY];
    char build_operation_id[UMI_DEVELOPER_ID_CAPACITY];
    char test_operation_id[UMI_DEVELOPER_ID_CAPACITY];
    char run_operation_id[UMI_DEVELOPER_ID_CAPACITY];
    size_t operation_count;
} UmiDeveloperCMakePlanSnapshot;

UmiStatus umi_developer_cmake_plan_submit(
    UmiDeveloperPipeline *pipeline,
    const UmiDeveloperCMakePlanRequest *request,
    UmiDeveloperCMakePlanSnapshot *out_plan);

#ifdef __cplusplus
}
#endif

#endif
