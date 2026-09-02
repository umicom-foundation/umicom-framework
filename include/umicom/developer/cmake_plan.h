/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/cmake_plan.h
 *
 * PURPOSE:
 *   Build dependency-safe CMake/Ninja development and delivery plans using
 *   structured configure, build, test, install, package and run operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
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
#define UMI_DEVELOPER_CMAKE_DELIVERY_PLAN_API_VERSION 1U

/**
 * Represent the developer cmake plan request data shared with callers of this public
 * contract.
 */
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

/**
 * Represent the developer cmake plan snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiDeveloperCMakePlanSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char configure_operation_id[UMI_DEVELOPER_ID_CAPACITY];
    char build_operation_id[UMI_DEVELOPER_ID_CAPACITY];
    char test_operation_id[UMI_DEVELOPER_ID_CAPACITY];
    char run_operation_id[UMI_DEVELOPER_ID_CAPACITY];
    size_t operation_count;
} UmiDeveloperCMakePlanSnapshot;

/**
 * List the named developer cmake delivery stage flags values accepted by this public
 * contract.
 */
typedef enum UmiDeveloperCMakeDeliveryStageFlags {
    UMI_DEVELOPER_CMAKE_DELIVERY_TESTS = 1U << 0,
    UMI_DEVELOPER_CMAKE_DELIVERY_INSTALL = 1U << 1,
    UMI_DEVELOPER_CMAKE_DELIVERY_PACKAGE = 1U << 2,
    UMI_DEVELOPER_CMAKE_DELIVERY_ALL =
        UMI_DEVELOPER_CMAKE_DELIVERY_TESTS |
        UMI_DEVELOPER_CMAKE_DELIVERY_INSTALL |
        UMI_DEVELOPER_CMAKE_DELIVERY_PACKAGE
} UmiDeveloperCMakeDeliveryStageFlags;

/**
 * Represent the developer cmake delivery plan request data shared with callers of this
 * public contract.
 */
typedef struct UmiDeveloperCMakeDeliveryPlanRequest {
    uint32_t struct_size;
    uint32_t api_version;
    const char *plan_id;
    const char *project_id;
    const char *configuration_id;
    const char *source_directory;
    const char *build_directory;
    const char *generator;
    const char *test_filter;
    const char *install_prefix;
    const char *package_generator;
    uint32_t parallel_jobs;
    uint32_t timeout_ms;
    uint32_t stage_flags;
} UmiDeveloperCMakeDeliveryPlanRequest;

/**
 * Represent the developer cmake delivery plan snapshot data shared with callers of this
 * public contract.
 */
typedef struct UmiDeveloperCMakeDeliveryPlanSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char configure_operation_id[UMI_DEVELOPER_ID_CAPACITY];
    char build_operation_id[UMI_DEVELOPER_ID_CAPACITY];
    char test_operation_id[UMI_DEVELOPER_ID_CAPACITY];
    char install_operation_id[UMI_DEVELOPER_ID_CAPACITY];
    char package_operation_id[UMI_DEVELOPER_ID_CAPACITY];
    size_t operation_count;
} UmiDeveloperCMakeDeliveryPlanSnapshot;

/**
 * Provide the developer cmake plan submit operation used by this module and its client
 * applications.
 */
UmiStatus umi_developer_cmake_plan_submit(
    UmiDeveloperPipeline *pipeline,
    const UmiDeveloperCMakePlanRequest *request,
    UmiDeveloperCMakePlanSnapshot *out_plan);

/* stage_flags == 0 requests the complete test/install/package delivery chain. */
UmiStatus umi_developer_cmake_delivery_plan_submit(
    UmiDeveloperPipeline *pipeline,
    const UmiDeveloperCMakeDeliveryPlanRequest *request,
    UmiDeveloperCMakeDeliveryPlanSnapshot *out_plan);

#ifdef __cplusplus
}
#endif

#endif
