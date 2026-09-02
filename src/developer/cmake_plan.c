/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/cmake_plan.c
 *
 * PURPOSE:
 *   Implement structured CMake/Ninja development and release-delivery plans.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/developer/cmake_plan.h"

#include <stdio.h>
#include <string.h>

/* Provide the copy text operation used by this module and its client applications. */
static UmiStatus copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length > 0U) {
        memcpy(destination, source, length);
    }
    destination[length] = '\0';
    return UMI_STATUS_OK;
}

/* Provide the make id operation used by this module and its client applications. */
static UmiStatus make_id(
    char *destination,
    size_t capacity,
    const char *plan_id,
    const char *suffix)
{
    int written;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || plan_id == NULL || suffix == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    written = snprintf(destination, capacity, "%s.%s", plan_id, suffix);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}

/* Provide the set common operation used by this module and its client applications. */
static UmiStatus set_common(
    UmiDeveloperOperationSnapshot *operation,
    const UmiDeveloperCMakePlanRequest *request)
{
    UmiStatus status;

    operation->timeout_ms = request->timeout_ms;
    operation->max_attempts = 2U;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (request->project_id != NULL) {
        status = copy_text(operation->project_id,
                           sizeof(operation->project_id),
                           request->project_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            return status;
        }
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (request->configuration_id != NULL) {
        status = copy_text(operation->configuration_id,
                           sizeof(operation->configuration_id),
                           request->configuration_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            return status;
        }
    }
    return UMI_STATUS_OK;
}

/* Provide the add operation operation used by this module and its client applications. */
static UmiStatus add_operation(
    UmiDeveloperPipeline *pipeline,
    UmiDeveloperOperationSnapshot *operation,
    const char *previous_operation_id)
{
    UmiStatus status = umi_developer_pipeline_submit(pipeline, operation);

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (previous_operation_id != NULL && previous_operation_id[0] != '\0') {
        status = umi_developer_pipeline_add_dependency(
            pipeline,
            operation->id,
            previous_operation_id);
    }
    return status;
}

/*
 * Provide the developer cmake plan submit operation used by this module and its client
 * applications.
 */
UmiStatus umi_developer_cmake_plan_submit(
    UmiDeveloperPipeline *pipeline,
    const UmiDeveloperCMakePlanRequest *request,
    UmiDeveloperCMakePlanSnapshot *out_plan)
{
    UmiDeveloperOperationSnapshot operation;
    UmiDeveloperOperationSnapshot existing;
    UmiDeveloperCMakePlanSnapshot plan;
    char build_type[UMI_DEVELOPER_ARGUMENT_CAPACITY];
    char parallel_jobs[32];
    char previous_id[UMI_DEVELOPER_ID_CAPACITY];
    const char *configuration;
    const char *generator;
    UmiStatus status;
    size_t required_operations = 0U;
    size_t index;
    int written;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (pipeline == NULL || request == NULL ||
        request->plan_id == NULL || request->plan_id[0] == '\0' ||
        request->source_directory == NULL ||
        request->build_directory == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (request->include_configure != 0) required_operations += 1U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (request->include_build != 0) required_operations += 1U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (request->include_tests != 0) required_operations += 1U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (request->include_run != 0) required_operations += 1U;

    /* Apply this branch only when its contract condition is satisfied. */
    if (required_operations == 0U ||
        umi_developer_pipeline_count(pipeline) + required_operations >
            UMI_DEVELOPER_PIPELINE_OPERATION_CAPACITY) {
        return required_operations == 0U
            ? UMI_STATUS_INVALID_ARGUMENT
            : UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (request->include_run != 0 &&
        (request->run_program == NULL || request->run_program[0] == '\0')) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (request->run_argument_count > UMI_DEVELOPER_MAX_ARGUMENTS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    memset(&plan, 0, sizeof(plan));
    plan.struct_size = (uint32_t)sizeof(plan);
    plan.api_version = UMI_DEVELOPER_CMAKE_PLAN_API_VERSION;
    previous_id[0] = '\0';

#define UMI_PREPARE_PLAN_ID(field_name, suffix_text)                               \
    do {                                                                           \
        status = make_id(plan.field_name, sizeof(plan.field_name),                 \
                         request->plan_id, (suffix_text));                          \
        if (status != UMI_STATUS_OK) {                                              \
            return status;                                                          \
        }                                                                           \
        if (umi_developer_pipeline_find(pipeline, plan.field_name, &existing) ==    \
            UMI_STATUS_OK) {                                                        \
            return UMI_STATUS_ALREADY_EXISTS;                                       \
        }                                                                           \
    } while (0)

    /* Apply this branch only when its contract condition is satisfied. */
    if (request->include_configure != 0) {
        UMI_PREPARE_PLAN_ID(configure_operation_id, "configure");
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (request->include_build != 0) {
        UMI_PREPARE_PLAN_ID(build_operation_id, "build");
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (request->include_tests != 0) {
        UMI_PREPARE_PLAN_ID(test_operation_id, "test");
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (request->include_run != 0) {
        UMI_PREPARE_PLAN_ID(run_operation_id, "run");
    }

#undef UMI_PREPARE_PLAN_ID

    configuration =
        request->configuration_id != NULL && request->configuration_id[0] != '\0'
            ? request->configuration_id
            : "Debug";
    generator =
        request->generator != NULL && request->generator[0] != '\0'
            ? request->generator
            : "Ninja";

    /* Apply this branch only when its contract condition is satisfied. */
    if (request->include_configure != 0) {
        status = umi_developer_operation_init(
            &operation,
            plan.configure_operation_id,
            UMI_DEVELOPER_OPERATION_CONFIGURE,
            "Configure project");
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        status = set_common(&operation, request);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        status = umi_developer_operation_set_program(
            &operation, "cmake", request->source_directory);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;

        written = snprintf(build_type, sizeof(build_type),
                           "-DCMAKE_BUILD_TYPE=%s", configuration);
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (written < 0 || (size_t)written >= sizeof(build_type)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }

        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if ((status = umi_developer_operation_add_argument(&operation, "-S")) != UMI_STATUS_OK ||
            (status = umi_developer_operation_add_argument(&operation, request->source_directory)) != UMI_STATUS_OK ||
            (status = umi_developer_operation_add_argument(&operation, "-B")) != UMI_STATUS_OK ||
            (status = umi_developer_operation_add_argument(&operation, request->build_directory)) != UMI_STATUS_OK ||
            (status = umi_developer_operation_add_argument(&operation, "-G")) != UMI_STATUS_OK ||
            (status = umi_developer_operation_add_argument(&operation, generator)) != UMI_STATUS_OK ||
            (status = umi_developer_operation_add_argument(&operation, build_type)) != UMI_STATUS_OK) {
            return status;
        }

        status = add_operation(pipeline, &operation, NULL);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        (void)copy_text(previous_id, sizeof(previous_id), operation.id);
        plan.operation_count += 1U;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (request->include_build != 0) {
        status = umi_developer_operation_init(
            &operation,
            plan.build_operation_id,
            UMI_DEVELOPER_OPERATION_BUILD,
            "Build project");
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        status = set_common(&operation, request);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        status = umi_developer_operation_set_program(
            &operation, "cmake", request->source_directory);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;

        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if ((status = umi_developer_operation_add_argument(&operation, "--build")) != UMI_STATUS_OK ||
            (status = umi_developer_operation_add_argument(&operation, request->build_directory)) != UMI_STATUS_OK) {
            return status;
        }

        /* Apply this branch only when its contract condition is satisfied. */
        if (request->parallel_jobs > 0U) {
            written = snprintf(parallel_jobs, sizeof(parallel_jobs),
                               "%u", (unsigned int)request->parallel_jobs);
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (written < 0 || (size_t)written >= sizeof(parallel_jobs)) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if ((status = umi_developer_operation_add_argument(&operation, "--parallel")) != UMI_STATUS_OK ||
                (status = umi_developer_operation_add_argument(&operation, parallel_jobs)) != UMI_STATUS_OK) {
                return status;
            }
        }

        status = add_operation(
            pipeline, &operation, previous_id[0] != '\0' ? previous_id : NULL);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        (void)copy_text(previous_id, sizeof(previous_id), operation.id);
        plan.operation_count += 1U;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (request->include_tests != 0) {
        status = umi_developer_operation_init(
            &operation,
            plan.test_operation_id,
            UMI_DEVELOPER_OPERATION_TEST,
            "Run tests");
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        status = set_common(&operation, request);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        status = umi_developer_operation_set_program(
            &operation, "ctest", request->source_directory);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;

        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if ((status = umi_developer_operation_add_argument(&operation, "--test-dir")) != UMI_STATUS_OK ||
            (status = umi_developer_operation_add_argument(&operation, request->build_directory)) != UMI_STATUS_OK ||
            (status = umi_developer_operation_add_argument(&operation, "--output-on-failure")) != UMI_STATUS_OK ||
            (status = umi_developer_operation_add_argument(&operation, "-C")) != UMI_STATUS_OK ||
            (status = umi_developer_operation_add_argument(&operation, configuration)) != UMI_STATUS_OK) {
            return status;
        }

        status = add_operation(
            pipeline, &operation, previous_id[0] != '\0' ? previous_id : NULL);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        (void)copy_text(previous_id, sizeof(previous_id), operation.id);
        plan.operation_count += 1U;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (request->include_run != 0) {
        status = umi_developer_operation_init(
            &operation,
            plan.run_operation_id,
            UMI_DEVELOPER_OPERATION_RUN,
            "Run application");
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        status = set_common(&operation, request);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        status = umi_developer_operation_set_program(
            &operation, request->run_program, request->source_directory);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;

        /* Visit each bounded item once so every record receives the same rule. */
        for (index = 0U; index < request->run_argument_count; ++index) {
            status = umi_developer_operation_add_argument(
                &operation, request->run_arguments[index]);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) {
                return status;
            }
        }

        status = add_operation(
            pipeline, &operation, previous_id[0] != '\0' ? previous_id : NULL);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        plan.operation_count += 1U;
    }

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_plan != NULL) {
        *out_plan = plan;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the set delivery common operation used by this module and its client
 * applications.
 */
static UmiStatus set_delivery_common(
    UmiDeveloperOperationSnapshot *operation,
    const UmiDeveloperCMakeDeliveryPlanRequest *request)
{
    UmiStatus status;

    operation->timeout_ms = request->timeout_ms;
    operation->max_attempts = 2U;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (request->project_id != NULL) {
        status = copy_text(operation->project_id,
                           sizeof(operation->project_id),
                           request->project_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            return status;
        }
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (request->configuration_id != NULL) {
        status = copy_text(operation->configuration_id,
                           sizeof(operation->configuration_id),
                           request->configuration_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            return status;
        }
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the add delivery common arguments operation used by this module and its client
 * applications.
 */
static UmiStatus add_delivery_common_arguments(
    UmiDeveloperOperationSnapshot *operation,
    const char *first,
    const char *second)
{
    UmiStatus status;

    status = umi_developer_operation_add_argument(operation, first);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (status == UMI_STATUS_OK && second != NULL) {
        status = umi_developer_operation_add_argument(operation, second);
    }
    return status;
}

/*
 * Provide the developer cmake delivery plan submit operation used by this module and its
 * client applications.
 */
UmiStatus umi_developer_cmake_delivery_plan_submit(
    UmiDeveloperPipeline *pipeline,
    const UmiDeveloperCMakeDeliveryPlanRequest *request,
    UmiDeveloperCMakeDeliveryPlanSnapshot *out_plan)
{
    UmiDeveloperOperationSnapshot operation;
    UmiDeveloperOperationSnapshot existing;
    UmiDeveloperCMakeDeliveryPlanSnapshot plan;
    UmiDeveloperPipelineSnapshot pipeline_snapshot;
    char build_type[UMI_DEVELOPER_ARGUMENT_CAPACITY];
    char parallel_jobs[32];
    char previous_id[UMI_DEVELOPER_ID_CAPACITY];
    const char *configuration;
    const char *generator;
    uint32_t stages;
    size_t required_operations = 2U;
    size_t required_dependencies;
    UmiStatus status;
    int written;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (pipeline == NULL || request == NULL ||
        request->struct_size < sizeof(*request) ||
        request->api_version != UMI_DEVELOPER_CMAKE_DELIVERY_PLAN_API_VERSION ||
        request->plan_id == NULL || request->plan_id[0] == '\0' ||
        request->source_directory == NULL || request->source_directory[0] == '\0' ||
        request->build_directory == NULL || request->build_directory[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    stages = request->stage_flags == 0U
        ? (uint32_t)UMI_DEVELOPER_CMAKE_DELIVERY_ALL
        : request->stage_flags;
    /* Apply this branch only when its contract condition is satisfied. */
    if ((stages & ~(uint32_t)UMI_DEVELOPER_CMAKE_DELIVERY_ALL) != 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if ((stages & (uint32_t)UMI_DEVELOPER_CMAKE_DELIVERY_TESTS) != 0U) {
        required_operations += 1U;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if ((stages & (uint32_t)UMI_DEVELOPER_CMAKE_DELIVERY_INSTALL) != 0U) {
        required_operations += 1U;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if ((stages & (uint32_t)UMI_DEVELOPER_CMAKE_DELIVERY_PACKAGE) != 0U) {
        required_operations += 1U;
    }
    required_dependencies = required_operations - 1U;

    status = umi_developer_pipeline_snapshot(pipeline, &pipeline_snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (pipeline_snapshot.operation_count + required_operations >
            UMI_DEVELOPER_PIPELINE_OPERATION_CAPACITY ||
        pipeline_snapshot.dependency_count + required_dependencies >
            UMI_DEVELOPER_PIPELINE_DEPENDENCY_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    memset(&plan, 0, sizeof(plan));
    plan.struct_size = (uint32_t)sizeof(plan);
    plan.api_version = UMI_DEVELOPER_CMAKE_DELIVERY_PLAN_API_VERSION;
    previous_id[0] = '\0';

#define UMI_PREPARE_DELIVERY_ID(field_name, suffix_text)                           \
    do {                                                                           \
        status = make_id(plan.field_name, sizeof(plan.field_name),                 \
                         request->plan_id, (suffix_text));                          \
        if (status != UMI_STATUS_OK) {                                              \
            return status;                                                          \
        }                                                                           \
        if (umi_developer_pipeline_find(pipeline, plan.field_name, &existing) ==    \
            UMI_STATUS_OK) {                                                        \
            return UMI_STATUS_ALREADY_EXISTS;                                       \
        }                                                                           \
    } while (0)

    UMI_PREPARE_DELIVERY_ID(configure_operation_id, "configure");
    UMI_PREPARE_DELIVERY_ID(build_operation_id, "build");
    /* Apply this branch only when its contract condition is satisfied. */
    if ((stages & (uint32_t)UMI_DEVELOPER_CMAKE_DELIVERY_TESTS) != 0U) {
        UMI_PREPARE_DELIVERY_ID(test_operation_id, "test");
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if ((stages & (uint32_t)UMI_DEVELOPER_CMAKE_DELIVERY_INSTALL) != 0U) {
        UMI_PREPARE_DELIVERY_ID(install_operation_id, "install");
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if ((stages & (uint32_t)UMI_DEVELOPER_CMAKE_DELIVERY_PACKAGE) != 0U) {
        UMI_PREPARE_DELIVERY_ID(package_operation_id, "package");
    }

#undef UMI_PREPARE_DELIVERY_ID

    configuration =
        request->configuration_id != NULL && request->configuration_id[0] != '\0'
            ? request->configuration_id
            : "Release";
    generator =
        request->generator != NULL && request->generator[0] != '\0'
            ? request->generator
            : "Ninja";

    status = umi_developer_operation_init(
        &operation,
        plan.configure_operation_id,
        UMI_DEVELOPER_OPERATION_CONFIGURE,
        "Configure delivery build");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = set_delivery_common(&operation, request);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_developer_operation_set_program(
        &operation, "cmake", request->source_directory);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    written = snprintf(build_type, sizeof(build_type),
                       "-DCMAKE_BUILD_TYPE=%s", configuration);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= sizeof(build_type)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if ((status = add_delivery_common_arguments(&operation, "-S", request->source_directory)) != UMI_STATUS_OK ||
        (status = add_delivery_common_arguments(&operation, "-B", request->build_directory)) != UMI_STATUS_OK ||
        (status = add_delivery_common_arguments(&operation, "-G", generator)) != UMI_STATUS_OK ||
        (status = umi_developer_operation_add_argument(&operation, build_type)) != UMI_STATUS_OK) {
        return status;
    }
    status = add_operation(pipeline, &operation, NULL);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    (void)copy_text(previous_id, sizeof(previous_id), operation.id);
    plan.operation_count += 1U;

    status = umi_developer_operation_init(
        &operation,
        plan.build_operation_id,
        UMI_DEVELOPER_OPERATION_BUILD,
        "Build delivery target");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = set_delivery_common(&operation, request);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_developer_operation_set_program(
        &operation, "cmake", request->source_directory);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Create this optional product surface only when its build option is enabled. */
    if ((status = add_delivery_common_arguments(&operation, "--build", request->build_directory)) != UMI_STATUS_OK ||
        (status = add_delivery_common_arguments(&operation, "--config", configuration)) != UMI_STATUS_OK) {
        return status;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (request->parallel_jobs > 0U) {
        written = snprintf(parallel_jobs, sizeof(parallel_jobs),
                           "%u", (unsigned int)request->parallel_jobs);
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (written < 0 || (size_t)written >= sizeof(parallel_jobs)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if ((status = add_delivery_common_arguments(&operation, "--parallel", parallel_jobs)) != UMI_STATUS_OK) {
            return status;
        }
    }
    status = add_operation(pipeline, &operation, previous_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    (void)copy_text(previous_id, sizeof(previous_id), operation.id);
    plan.operation_count += 1U;

    /* Apply this branch only when its contract condition is satisfied. */
    if ((stages & (uint32_t)UMI_DEVELOPER_CMAKE_DELIVERY_TESTS) != 0U) {
        status = umi_developer_operation_init(
            &operation,
            plan.test_operation_id,
            UMI_DEVELOPER_OPERATION_TEST,
            "Test delivery build");
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        status = set_delivery_common(&operation, request);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        status = umi_developer_operation_set_program(
            &operation, "ctest", request->source_directory);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        /* Create this optional product surface only when its build option is enabled. */
        if ((status = add_delivery_common_arguments(&operation, "--test-dir", request->build_directory)) != UMI_STATUS_OK ||
            (status = umi_developer_operation_add_argument(&operation, "--output-on-failure")) != UMI_STATUS_OK ||
            (status = add_delivery_common_arguments(&operation, "-C", configuration)) != UMI_STATUS_OK) {
            return status;
        }
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (request->test_filter != NULL && request->test_filter[0] != '\0') {
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if ((status = add_delivery_common_arguments(&operation, "-R", request->test_filter)) != UMI_STATUS_OK) {
                return status;
            }
        }
        status = add_operation(pipeline, &operation, previous_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        (void)copy_text(previous_id, sizeof(previous_id), operation.id);
        plan.operation_count += 1U;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if ((stages & (uint32_t)UMI_DEVELOPER_CMAKE_DELIVERY_INSTALL) != 0U) {
        status = umi_developer_operation_init(
            &operation,
            plan.install_operation_id,
            UMI_DEVELOPER_OPERATION_CUSTOM,
            "Install delivery build");
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        status = set_delivery_common(&operation, request);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        status = umi_developer_operation_set_program(
            &operation, "cmake", request->source_directory);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        /* Create this optional product surface only when its build option is enabled. */
        if ((status = add_delivery_common_arguments(&operation, "--install", request->build_directory)) != UMI_STATUS_OK ||
            (status = add_delivery_common_arguments(&operation, "--config", configuration)) != UMI_STATUS_OK) {
            return status;
        }
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (request->install_prefix != NULL && request->install_prefix[0] != '\0') {
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if ((status = add_delivery_common_arguments(&operation, "--prefix", request->install_prefix)) != UMI_STATUS_OK) {
                return status;
            }
        }
        status = add_operation(pipeline, &operation, previous_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        (void)copy_text(previous_id, sizeof(previous_id), operation.id);
        plan.operation_count += 1U;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if ((stages & (uint32_t)UMI_DEVELOPER_CMAKE_DELIVERY_PACKAGE) != 0U) {
        status = umi_developer_operation_init(
            &operation,
            plan.package_operation_id,
            UMI_DEVELOPER_OPERATION_PACKAGE,
            "Package delivery build");
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        status = set_delivery_common(&operation, request);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        status = umi_developer_operation_set_program(
            &operation, "cpack", request->build_directory);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if ((status = add_delivery_common_arguments(&operation, "--config", "CPackConfig.cmake")) != UMI_STATUS_OK ||
            (status = add_delivery_common_arguments(&operation, "-C", configuration)) != UMI_STATUS_OK) {
            return status;
        }
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (request->package_generator != NULL && request->package_generator[0] != '\0') {
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if ((status = add_delivery_common_arguments(&operation, "-G", request->package_generator)) != UMI_STATUS_OK) {
                return status;
            }
        }
        status = add_operation(pipeline, &operation, previous_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        plan.operation_count += 1U;
    }

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_plan != NULL) {
        *out_plan = plan;
    }
    return UMI_STATUS_OK;
}
