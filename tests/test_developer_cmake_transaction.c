/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_developer_cmake_transaction.c
 *
 * PURPOSE:
 *   Verify CMake plan refusal preserves existing work and successful plans retain every selected stage.
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

static int refuse_draft;
static void *DraftCalloc(size_t count, size_t bytes)
{
    return refuse_draft ? NULL : calloc(count, bytes);
}
/* Exercise the actual translator with a narrow draft-allocation seam.
 * The linked pipeline supplies real queue ownership and dependency admission. */
#define calloc DraftCalloc
#include "../src/developer/cmake_plan.c"
#undef calloc

typedef struct Capture {
    UmiDeveloperRuntimeSnapshot runtime;
    UmiDeveloperOperationSnapshot *operations;
    UmiDeveloperJournalEntry *events;
} Capture;
static Capture *Take(UmiDeveloperRuntime *runtime)
{
    Capture *out = calloc(1U, sizeof(*out));
    assert(out != NULL);
    assert(umi_developer_runtime_snapshot(runtime, &out->runtime) == UMI_STATUS_OK);
    size_t n = out->runtime.pipeline.operation_count;
    out->operations = calloc(n != 0U ? n : 1U, sizeof(*out->operations));
    n = out->runtime.journal.entry_count;
    out->events = calloc(n != 0U ? n : 1U, sizeof(*out->events));
    assert(out->operations != NULL && out->events != NULL);
    for (size_t i = 0U; i < out->runtime.pipeline.operation_count; ++i)
        assert(umi_developer_pipeline_at(umi_developer_runtime_pipeline(runtime), i, &out->operations[i]) == UMI_STATUS_OK);
    for (size_t i = 0U; i < out->runtime.journal.entry_count; ++i)
        assert(umi_developer_journal_at(umi_developer_runtime_journal(runtime), i, &out->events[i]) == UMI_STATUS_OK);
    return out;
}
static void Release(Capture *value)
{ free(value->operations); free(value->events); free(value); }
static void Same(UmiDeveloperRuntime *runtime, const Capture *before)
{
    Capture *after = Take(runtime);
    assert(memcmp(&before->runtime, &after->runtime, sizeof(before->runtime)) == 0);
    assert(memcmp(before->operations, after->operations,
        before->runtime.pipeline.operation_count * sizeof(*before->operations)) == 0);
    assert(memcmp(before->events, after->events,
        before->runtime.journal.entry_count * sizeof(*before->events)) == 0);
    Release(after);
}
static void Add(UmiDeveloperPipeline *pipeline, const char *id)
{
    UmiDeveloperOperationSnapshot operation;
    assert(umi_developer_operation_init(&operation, id, UMI_DEVELOPER_OPERATION_CUSTOM, id) == UMI_STATUS_OK);
    assert(umi_developer_pipeline_submit(pipeline, &operation) == UMI_STATUS_OK);
}
static void Fill(UmiDeveloperPipeline *pipeline, size_t target)
{
    while (umi_developer_pipeline_count(pipeline) < target) {
        char id[64];
        snprintf(id, sizeof(id), "occupied.%zu", umi_developer_pipeline_count(pipeline));
        Add(pipeline, id);
    }
}
static void Edges(UmiDeveloperPipeline *pipeline)
{
    Fill(pipeline, 48U);
    size_t edges = 0U;
    for (size_t i = 2U; i < 48U && edges < UMI_DEVELOPER_PIPELINE_DEPENDENCY_CAPACITY; ++i)
        for (size_t j = 1U; j < i && edges < UMI_DEVELOPER_PIPELINE_DEPENDENCY_CAPACITY; ++j) {
            char a[64], b[64];
            snprintf(a, sizeof(a), "occupied.%zu", i);
            snprintf(b, sizeof(b), "occupied.%zu", j);
            assert(umi_developer_pipeline_add_dependency(pipeline, a, b) == UMI_STATUS_OK);
            ++edges;
        }
    assert(edges == UMI_DEVELOPER_PIPELINE_DEPENDENCY_CAPACITY);
}
static void Ordered(UmiDeveloperPipeline *pipeline, const char *const *ids, size_t count)
{
    for (size_t i = 0U; i < count; ++i) {
        UmiDeveloperOperationSnapshot operation;
        assert(umi_developer_pipeline_next_ready(pipeline, &operation) == UMI_STATUS_OK);
        assert(!strcmp(operation.id, ids[i]));
        assert(umi_developer_pipeline_start(pipeline, ids[i]) == UMI_STATUS_OK);
        assert(umi_developer_pipeline_complete(pipeline, ids[i], 0, "Synthetic completion") == UMI_STATUS_OK);
    }
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *mode = argv[1];
    UmiDeveloperRuntime *runtime = NULL;
    assert(umi_developer_runtime_create_owned(&runtime) == UMI_STATUS_OK);
    UmiDeveloperPipeline *pipeline = umi_developer_runtime_pipeline(runtime);
    Add(pipeline, "existing");
    assert(umi_developer_runtime_start_operation(runtime, "existing") == UMI_STATUS_OK);
    char long_argument[UMI_DEVELOPER_ARGUMENT_CAPACITY + 1U];
    char long_program[UMI_DEVELOPER_PATH_CAPACITY + 1U];
    memset(long_argument, 'x', sizeof(long_argument)); long_argument[sizeof(long_argument) - 1U] = 0;
    memset(long_program, 'x', sizeof(long_program)); long_program[sizeof(long_program) - 1U] = 0;
    const char *run_arguments[] = {"--example", "value"};
    UmiDeveloperCMakePlanRequest dev = {0};
    dev.struct_size = (uint32_t)sizeof(dev); dev.api_version = UMI_DEVELOPER_CMAKE_PLAN_API_VERSION;
    dev.plan_id = "draft"; dev.project_id = "fixture"; dev.configuration_id = "Debug";
    dev.source_directory = "source"; dev.build_directory = "build";
    dev.include_configure = dev.include_build = dev.include_tests = dev.include_run = 1;
    dev.run_program = "synthetic-application"; dev.run_arguments = run_arguments; dev.run_argument_count = 2U;
    UmiDeveloperCMakeDeliveryPlanRequest delivery = {0};
    delivery.struct_size = (uint32_t)sizeof(delivery); delivery.api_version = UMI_DEVELOPER_CMAKE_DELIVERY_PLAN_API_VERSION;
    delivery.plan_id = "draft"; delivery.project_id = "fixture"; delivery.configuration_id = "Release";
    delivery.source_directory = "source"; delivery.build_directory = "build";
    delivery.test_filter = "fixture"; delivery.install_prefix = "install"; delivery.package_generator = "ZIP";
    int is_delivery = !strncmp(mode, "delivery-", 9U) || !strcmp(mode, "allocation-delivery");
    int use_runtime = !strncmp(mode, "runtime-", 8U);
    int success = 0, retry = 0;
    if (!strncmp(mode, "dev-flags-", 10U)) {
        unsigned flags = (unsigned)strtoul(mode + 10, NULL, 10);
        assert(flags >= 1U && flags <= 15U);
        dev.include_configure = (flags & 1U) != 0U;
        dev.include_build = (flags & 2U) != 0U;
        dev.include_tests = (flags & 4U) != 0U;
        dev.include_run = (flags & 8U) != 0U;
        success = 1;
    } else if (!strncmp(mode, "delivery-flags-", 15U)) {
        delivery.stage_flags = (uint32_t)strtoul(mode + 15, NULL, 10);
        assert(delivery.stage_flags <= 7U); success = 1;
    } else if (!strcmp(mode, "dev-run-array")) dev.run_arguments = NULL;
    else if (!strcmp(mode, "dev-run-null") || !strcmp(mode, "runtime-refusal")) run_arguments[1] = NULL;
    else if (!strcmp(mode, "dev-run-long") || !strcmp(mode, "dev-retry")) {
        run_arguments[1] = long_argument; retry = !strcmp(mode, "dev-retry");
    } else if (!strcmp(mode, "dev-run-program-long")) dev.run_program = long_program;
    else if (!strcmp(mode, "dev-empty")) dev.include_configure = dev.include_build = dev.include_tests = dev.include_run = 0;
    else if (!strcmp(mode, "delivery-filter-long")) delivery.test_filter = long_argument;
    else if (!strcmp(mode, "delivery-prefix-long")) delivery.install_prefix = long_argument;
    else if (!strcmp(mode, "delivery-generator-long") || !strcmp(mode, "delivery-retry")) {
        delivery.package_generator = long_argument; retry = !strcmp(mode, "delivery-retry");
    } else if (!strcmp(mode, "delivery-invalid-flags")) delivery.stage_flags = UINT32_C(0x80000000);
    else if (!strcmp(mode, "delivery-version")) delivery.api_version++;
    else if (!strcmp(mode, "delivery-struct")) delivery.struct_size = 0U;
    else if (!strcmp(mode, "runtime-success")) success = 1;
    else if (!strcmp(mode, "dev-capacity") || !strcmp(mode, "delivery-capacity")) Fill(pipeline, 255U);
    else if (!strcmp(mode, "dev-dependencies") || !strcmp(mode, "delivery-dependencies")) Edges(pipeline);
    else if (!strcmp(mode, "dev-collision") || !strcmp(mode, "delivery-collision"))
        Add(pipeline, is_delivery ? "draft.package" : "draft.run");
    else if (!strcmp(mode, "allocation-dev") || !strcmp(mode, "allocation-delivery")) {}
    else { umi_developer_runtime_destroy(runtime); return 2; }
    Capture *before = Take(runtime);
    UmiDeveloperCMakePlanSnapshot result, sentinel;
    UmiDeveloperCMakeDeliveryPlanSnapshot delivered, delivered_sentinel;
    memset(&result, 0x5a, sizeof(result)); memcpy(&sentinel, &result, sizeof(result));
    memset(&delivered, 0x6b, sizeof(delivered)); memcpy(&delivered_sentinel, &delivered, sizeof(delivered));
    refuse_draft = !strncmp(mode, "allocation-", 11U);
    UmiStatus status = is_delivery ? umi_developer_cmake_delivery_plan_submit(pipeline, &delivery, &delivered) :
        use_runtime ? umi_developer_runtime_submit_cmake_plan(runtime, &dev, &result) :
        umi_developer_cmake_plan_submit(pipeline, &dev, &result);
    refuse_draft = 0;
    if (!success) {
        assert(status != UMI_STATUS_OK);
        if (!strncmp(mode, "allocation-", 11U)) assert(status == UMI_STATUS_OUT_OF_MEMORY);
        assert(memcmp(&result, &sentinel, sizeof(result)) == 0);
        assert(memcmp(&delivered, &delivered_sentinel, sizeof(delivered)) == 0);
        Same(runtime, before);
        if (retry) {
            run_arguments[1] = "corrected"; delivery.package_generator = "ZIP";
            status = is_delivery ? umi_developer_cmake_delivery_plan_submit(pipeline, &delivery, &delivered) :
                umi_developer_cmake_plan_submit(pipeline, &dev, &result);
            assert(status == UMI_STATUS_OK);
            assert(umi_developer_pipeline_count(pipeline) == (is_delivery ? 6U : 5U));
        }
    } else {
        assert(status == UMI_STATUS_OK);
        const char *ids[5]; size_t count = 0U;
        if (is_delivery) {
            ids[count++] = delivered.configure_operation_id; ids[count++] = delivered.build_operation_id;
            if (delivered.test_operation_id[0]) ids[count++] = delivered.test_operation_id;
            if (delivered.install_operation_id[0]) ids[count++] = delivered.install_operation_id;
            if (delivered.package_operation_id[0]) ids[count++] = delivered.package_operation_id;
            assert(delivered.operation_count == count);
        } else {
            if (result.configure_operation_id[0]) ids[count++] = result.configure_operation_id;
            if (result.build_operation_id[0]) ids[count++] = result.build_operation_id;
            if (result.test_operation_id[0]) ids[count++] = result.test_operation_id;
            if (result.run_operation_id[0]) ids[count++] = result.run_operation_id;
            assert(result.operation_count == count);
        }
        Capture *after = Take(runtime);
        assert(after->runtime.pipeline.operation_count == 1U + count);
        assert(after->runtime.pipeline.dependency_count == count - 1U);
        assert(memcmp(&before->operations[0], &after->operations[0], sizeof(before->operations[0])) == 0);
        assert(after->runtime.journal.entry_count == before->runtime.journal.entry_count + (use_runtime ? count : 0U));
        if (use_runtime)
            for (size_t i = 0U; i < count; ++i) {
                const UmiDeveloperJournalEntry *event = &after->events[before->runtime.journal.entry_count + i];
                assert(event->kind == UMI_DEVELOPER_EVENT_SUBMITTED);
                assert(!strcmp(event->operation_id, ids[i]));
            }
        Ordered(pipeline, ids, count);
        Release(after);
    }
    Release(before); umi_developer_runtime_destroy(runtime);
    return 0;
}
