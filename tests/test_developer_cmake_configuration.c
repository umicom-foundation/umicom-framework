/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_developer_cmake_configuration.c
 *
 * PURPOSE:
 *   Verify consistent configuration selection across native development and delivery plans without executing tools.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "support/assertions.h"
#include "umicom/developer/developer.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct ConfigurationFixture {
    UmiDeveloperRuntime *runtime;
    UmiDeveloperPipeline *pipeline;
    UmiDeveloperOperationSnapshot operation;
    UmiDeveloperRuntimeSnapshot before;
    UmiDeveloperRuntimeSnapshot after;
} ConfigurationFixture;

/* Inspect owned command arguments, without spawning a compiler or shell. */
static void Command(ConfigurationFixture *fixture, const char *id,
                    const char *program, const char *directory,
                    const char *const *arguments, size_t count)
{
    assert(umi_developer_pipeline_find(fixture->pipeline, id, &fixture->operation) == UMI_STATUS_OK);
    assert(strcmp(fixture->operation.program, program) == 0);
    assert(strcmp(fixture->operation.working_directory, directory) == 0);
    assert(fixture->operation.argument_count == count);
    for (size_t index = 0U; index < count; ++index)
        assert(strcmp(fixture->operation.arguments[index], arguments[index]) == 0);
}
static void CompleteChain(ConfigurationFixture *fixture, const char *const *ids, size_t count)
{
    for (size_t index = 0U; index < count; ++index) {
        assert(umi_developer_pipeline_next_ready(fixture->pipeline, &fixture->operation) == UMI_STATUS_OK);
        assert(strcmp(fixture->operation.id, ids[index]) == 0);
        assert(umi_developer_pipeline_start(fixture->pipeline, ids[index]) == UMI_STATUS_OK);
        assert(umi_developer_pipeline_complete(fixture->pipeline, ids[index], 0,
            "Synthetic completion; no command executed") == UMI_STATUS_OK);
    }
}
int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    const int runtime = strcmp(argv[1], "runtime") == 0;
    const int delivery = strcmp(argv[1], "delivery") == 0;
    if (!runtime && !delivery && strcmp(argv[1], "pipeline") != 0) return 2;
    const char *mode = argv[2], *configuration = NULL;
    const char *generator = "Ninja Multi-Config";
    char boundary[UMI_DEVELOPER_ID_CAPACITY];
    char oversized[UMI_DEVELOPER_ID_CAPACITY + 1U];
    memset(boundary, 'p', sizeof(boundary)); boundary[sizeof(boundary) - 1U] = '\0';
    memset(oversized, 'q', sizeof(oversized)); oversized[sizeof(oversized) - 1U] = '\0';
    const int refusal = strcmp(mode, "oversized") == 0;
    const int build_only = strcmp(mode, "build-only") == 0;
    if (strcmp(mode, "default") == 0) configuration = NULL;
    else if (strcmp(mode, "empty") == 0) configuration = "";
    else if (strcmp(mode, "debug") == 0) configuration = "Debug";
    else if (strcmp(mode, "release") == 0 || build_only) configuration = "Release";
    else if (strcmp(mode, "relwithdebinfo") == 0) configuration = "RelWithDebInfo";
    else if (strcmp(mode, "minsize") == 0) configuration = "MinSizeRel";
    else if (strcmp(mode, "custom") == 0) configuration = "Profile Optimized";
    else if (strcmp(mode, "boundary") == 0) configuration = boundary;
    else if (refusal) configuration = oversized;
    else if (strcmp(mode, "single-config") == 0) { configuration = "Release"; generator = "Ninja"; }
    else return 2;
    if (delivery && build_only) return 2;

    ConfigurationFixture *fixture = calloc(1U, sizeof(*fixture));
    assert(fixture != NULL);
    assert(umi_developer_runtime_create_owned(&fixture->runtime) == UMI_STATUS_OK);
    fixture->pipeline = umi_developer_runtime_pipeline(fixture->runtime);
    const char *source = "C:/Development Practice/source";
    const char *build = "C:/Development Practice/build";
    const char *resolved = configuration != NULL && configuration[0] != '\0'
        ? configuration : delivery ? "Release" : "Debug";
    UmiDeveloperCMakePlanRequest request = {0};
    request.struct_size = (uint32_t)sizeof(request);
    request.api_version = UMI_DEVELOPER_CMAKE_PLAN_API_VERSION;
    request.plan_id = "configuration";
    request.configuration_id = configuration;
    request.source_directory = source;
    request.build_directory = build;
    request.generator = generator;
    request.include_configure = !build_only;
    request.include_build = 1;
    request.include_tests = !build_only;
    request.parallel_jobs = build_only ? 0U : 3U;
    UmiDeveloperCMakeDeliveryPlanRequest release = {0};
    release.struct_size = (uint32_t)sizeof(release);
    release.api_version = UMI_DEVELOPER_CMAKE_DELIVERY_PLAN_API_VERSION;
    release.plan_id = request.plan_id;
    release.configuration_id = configuration;
    release.source_directory = source;
    release.build_directory = build;
    release.generator = generator;
    release.parallel_jobs = request.parallel_jobs;
    release.install_prefix = "C:/Development Practice/stage";

    UmiDeveloperCMakePlanSnapshot plan, previous_plan;
    UmiDeveloperCMakeDeliveryPlanSnapshot delivery_plan, previous_delivery;
    memset(&plan, 0x5a, sizeof(plan)); memcpy(&previous_plan, &plan, sizeof(plan));
    memset(&delivery_plan, 0x6b, sizeof(delivery_plan));
    memcpy(&previous_delivery, &delivery_plan, sizeof(delivery_plan));
    assert(umi_developer_runtime_snapshot(fixture->runtime, &fixture->before) == UMI_STATUS_OK);
    UmiStatus status = delivery
        ? umi_developer_cmake_delivery_plan_submit(fixture->pipeline, &release, &delivery_plan)
        : runtime ? umi_developer_runtime_submit_cmake_plan(fixture->runtime, &request, &plan)
        : umi_developer_cmake_plan_submit(fixture->pipeline, &request, &plan);
    if (refusal) {
        assert(status == UMI_STATUS_CAPACITY_EXCEEDED);
        assert(memcmp(&plan, &previous_plan, sizeof(plan)) == 0);
        assert(memcmp(&delivery_plan, &previous_delivery, sizeof(delivery_plan)) == 0);
        assert(umi_developer_runtime_snapshot(fixture->runtime, &fixture->after) == UMI_STATUS_OK);
        assert(memcmp(&fixture->before, &fixture->after, sizeof(fixture->before)) == 0);
        /* A corrected request can reuse the identity refused above. */
        request.configuration_id = "Release"; release.configuration_id = "Release";
        status = delivery
            ? umi_developer_cmake_delivery_plan_submit(fixture->pipeline, &release, &delivery_plan)
            : runtime ? umi_developer_runtime_submit_cmake_plan(fixture->runtime, &request, &plan)
            : umi_developer_cmake_plan_submit(fixture->pipeline, &request, &plan);
        resolved = "Release";
    }
    assert(status == UMI_STATUS_OK);
    const size_t expected_count = delivery ? 5U : build_only ? 1U : 3U;
    assert((delivery ? delivery_plan.operation_count : plan.operation_count) == expected_count);
    assert(umi_developer_runtime_snapshot(fixture->runtime, &fixture->after) == UMI_STATUS_OK);
    assert(fixture->after.pipeline.operation_count == expected_count);
    assert(fixture->after.pipeline.dependency_count == expected_count - 1U);
    assert(fixture->after.journal.entry_count == (runtime ? expected_count : 0U));

    char build_type[UMI_DEVELOPER_ARGUMENT_CAPACITY];
    int written = snprintf(build_type, sizeof(build_type), "-DCMAKE_BUILD_TYPE=%s", resolved);
    assert(written > 0 && (size_t)written < sizeof(build_type));
    if (!build_only) {
        const char *args[] = {"-S", source, "-B", build, "-G", generator, build_type};
        Command(fixture, delivery ? delivery_plan.configure_operation_id : plan.configure_operation_id,
            "cmake", source, args, sizeof(args) / sizeof(args[0]));
    }
    const char *build_args[] = {"--build", build, "--config", resolved, "--parallel", "3"};
    Command(fixture, delivery ? delivery_plan.build_operation_id : plan.build_operation_id,
        "cmake", source, build_args, build_only ? 4U : 6U);
    if (!build_only) {
        const char *args[] = {"--test-dir", build, "--output-on-failure", "-C", resolved};
        Command(fixture, delivery ? delivery_plan.test_operation_id : plan.test_operation_id,
            "ctest", source, args, sizeof(args) / sizeof(args[0]));
    }
    if (delivery) {
        const char *install_args[] = {"--install", build, "--config", resolved,
            "--prefix", release.install_prefix};
        Command(fixture, delivery_plan.install_operation_id, "cmake", source,
            install_args, sizeof(install_args) / sizeof(install_args[0]));
        const char *package_args[] = {"--config", "CPackConfig.cmake", "-C", resolved};
        Command(fixture, delivery_plan.package_operation_id, "cpack", build,
            package_args, sizeof(package_args) / sizeof(package_args[0]));
        const char *ids[] = {delivery_plan.configure_operation_id, delivery_plan.build_operation_id,
            delivery_plan.test_operation_id, delivery_plan.install_operation_id, delivery_plan.package_operation_id};
        CompleteChain(fixture, ids, expected_count);
    } else if (build_only) {
        assert(plan.configure_operation_id[0] == '\0' && plan.test_operation_id[0] == '\0');
        const char *ids[] = {plan.build_operation_id};
        CompleteChain(fixture, ids, expected_count);
    } else {
        const char *ids[] = {plan.configure_operation_id, plan.build_operation_id, plan.test_operation_id};
        CompleteChain(fixture, ids, expected_count);
    }
    umi_developer_runtime_destroy(fixture->runtime);
    free(fixture);
    return 0;
}
