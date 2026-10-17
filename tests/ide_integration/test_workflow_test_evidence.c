/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ide_integration/test_workflow_test_evidence.c
 *
 * PURPOSE:
 *   Exercise readiness decisions against real Test Platform result registries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/workflow_evaluator.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum {
    REQUIRE_EXECUTION = 1U,
    REQUIRE_PASSES = 2U,
    NO_BINDING = 4U,
    NO_CONTEXT = 8U,
    RUNNING = 16U,
    STALE_COUNT = 32U
};

typedef struct EvidenceCase {
    const char *name;
    int first_outcome;
    int second_outcome;
    unsigned flags;
    UmiIdeWorkflowGateState expected;
    const char *detail;
} EvidenceCase;

/* Each case describes a user-visible policy boundary. A negative outcome means
 * no record, while zero is a real NOT_RUN record and must not count as a pass. */
static const EvidenceCase cases[] = {
    {"passed", 1, -1, REQUIRE_PASSES, UMI_IDE_GATE_PASS, "passed=1"},
    {"failed", 2, -1, REQUIRE_PASSES, UMI_IDE_GATE_BLOCK, "failed=1"},
    {"timed-out", 5, -1, REQUIRE_PASSES, UMI_IDE_GATE_BLOCK, "timed_out=1"},
    {"skipped", 3, -1, REQUIRE_PASSES, UMI_IDE_GATE_BLOCK, "skipped=1"},
    {"cancelled", 4, -1, REQUIRE_PASSES, UMI_IDE_GATE_BLOCK, "cancelled=1"},
    {"not-run", 0, -1, REQUIRE_PASSES, UMI_IDE_GATE_BLOCK, "not_run=1"},
    {"unknown", 99, -1, REQUIRE_PASSES, UMI_IDE_GATE_BLOCK, "unknown=1"},
    {"empty", -1, -1, REQUIRE_PASSES, UMI_IDE_GATE_BLOCK, "observed=0"},
    {"unbound", -1, -1, REQUIRE_PASSES | NO_BINDING | NO_CONTEXT,
        UMI_IDE_GATE_BLOCK, "not bound"},
    {"missing-binding", 1, -1, REQUIRE_PASSES | NO_BINDING,
        UMI_IDE_GATE_BLOCK, "not bound"},
    {"missing-context", 1, -1, REQUIRE_PASSES | NO_CONTEXT,
        UMI_IDE_GATE_BLOCK, "not bound"},
    {"running", 1, -1, REQUIRE_PASSES | RUNNING, UMI_IDE_GATE_BLOCK, "running=1"},
    {"stale-count", 1, -1, REQUIRE_PASSES | STALE_COUNT,
        UMI_IDE_GATE_BLOCK, "stale_count=1"},
    {"mixed-skipped", 1, 3, REQUIRE_PASSES, UMI_IDE_GATE_BLOCK, "skipped=1"},
    {"mixed-failed", 1, 2, REQUIRE_PASSES, UMI_IDE_GATE_BLOCK, "failed=1"},
    {"all-passed", 1, 1, REQUIRE_EXECUTION | REQUIRE_PASSES,
        UMI_IDE_GATE_PASS, "passed=2"},
    {"execution-empty", -1, -1, REQUIRE_EXECUTION, UMI_IDE_GATE_BLOCK, "observed=0"},
    {"execution-skipped", 3, -1, REQUIRE_EXECUTION, UMI_IDE_GATE_BLOCK, "skipped=1"},
    {"execution-failed", 2, -1, REQUIRE_EXECUTION, UMI_IDE_GATE_WARN, "failed=1"},
    {"execution-mixed", 1, 3, REQUIRE_EXECUTION, UMI_IDE_GATE_WARN, "skipped=1"},
    {"execution-running", 1, -1, REQUIRE_EXECUTION | RUNNING,
        UMI_IDE_GATE_BLOCK, "running=1"},
    {"optional-empty", -1, -1, 0U, UMI_IDE_GATE_WARN, "observed=0"},
    {"optional-skipped", 3, -1, 0U, UMI_IDE_GATE_WARN, "skipped=1"},
    {"optional-failed", 2, -1, 0U, UMI_IDE_GATE_WARN, "failed=1"},
    {"optional-running", 1, -1, RUNNING, UMI_IDE_GATE_WARN, "running=1"},
    {"optional-unbound", -1, -1, NO_BINDING | NO_CONTEXT, UMI_IDE_GATE_WARN, "not bound"},
    {"optional-stale", 1, -1, STALE_COUNT, UMI_IDE_GATE_WARN, "stale_count=1"},
    {"optional-passed", 1, -1, 0U, UMI_IDE_GATE_PASS, "passed=1"}
};

/* Conditions remain executable with NDEBUG: setup and checks must never vanish
 * from an optimized qualification build. Heap-owned aggregates avoid depending
 * on the smaller default Windows thread stack. */
#define REQUIRE(expression) do { \
    if (!(expression)) { \
        fprintf(stderr, "%s line %d: %s\n", scenario->name, __LINE__, #expression); \
        goto done; \
    } \
} while (0)

static int RunCase(const EvidenceCase *scenario)
{
    UmiTestPlatformService *service = NULL;
    UmiIdeActiveContext *context = calloc(1U, sizeof(*context));
    UmiIdeWorkflowReport *report = calloc(1U, sizeof(*report));
    UmiIdeIntegrationBindings bindings;
    UmiIdeWorkflowPolicy policy;
    int result = EXIT_FAILURE;
    const int required = (scenario->flags & (REQUIRE_EXECUTION | REQUIRE_PASSES)) != 0U;
    const UmiIdeWorkflowGate *gate = NULL;

    REQUIRE(context != NULL && report != NULL);
    REQUIRE(umi_test_platform_service_create(&service) == UMI_STATUS_OK);
    umi_ide_integration_bindings_init(&bindings);
    bindings.tests = (scenario->flags & NO_BINDING) != 0U ? NULL : service;
    context->has_tests = (scenario->flags & NO_CONTEXT) == 0U;

    /* Keep other domains optional to isolate whether test evidence changes the
     * aggregate ready flag. Their warnings are still retained in the report. */
    umi_ide_workflow_policy_init(&policy);
    policy.require_workspace = 0;
    policy.require_no_errors = 0;
    policy.require_no_fatals = 0;
    policy.require_source_control = 0;
    policy.require_debugger_idle = 0;
    policy.require_no_pending_ai_approval = 0;
    policy.require_tests = (scenario->flags & REQUIRE_EXECUTION) != 0U;
    policy.require_all_tests_passed = (scenario->flags & REQUIRE_PASSES) != 0U;

    const int outcomes[] = {scenario->first_outcome, scenario->second_outcome};
    for (size_t index = 0U; index < sizeof(outcomes) / sizeof(outcomes[0]); ++index) {
        if (outcomes[index] < 0) continue;
        UmiTestPlatformResultSnapshot item;
        memset(&item, 0, sizeof(item));
        item.struct_size = (uint32_t)sizeof(item);
        item.api_version = UMI_TEST_PLATFORM_RESULT_API_VERSION;
        (void)snprintf(item.id, sizeof(item.id), "result-%zu", index);
        (void)snprintf(item.item_id, sizeof(item.item_id), "test-%zu", index);
        (void)snprintf(item.session_id, sizeof(item.session_id), "qualification");
        item.outcome = outcomes[index];
        REQUIRE(umi_test_platform_result_registry_upsert(
            umi_test_platform_service_result(service), &item) == UMI_STATUS_OK);
    }
    REQUIRE(umi_test_platform_service_snapshot(service, &context->tests) == UMI_STATUS_OK);
    /* The evaluator consumes supplied snapshots. These deliberate variations
     * exercise a running observation and a context captured before new results. */
    context->tests.operation_running = (scenario->flags & RUNNING) != 0U;
    if ((scenario->flags & STALE_COUNT) != 0U) context->tests.result_count = 0U;

    const uint64_t registry_revision = umi_test_platform_result_registry_revision(
        umi_test_platform_service_result(service));
    REQUIRE(umi_ide_workflow_evaluate(&bindings, context, &policy, report) == UMI_STATUS_OK);
    for (size_t index = 0U; index < report->gate_count; ++index) {
        if (strcmp(report->gates[index].gate_id, "tests") == 0) {
            gate = &report->gates[index];
            break;
        }
    }
    REQUIRE(gate != NULL);
    REQUIRE(gate->required == required);
    REQUIRE(gate->state == scenario->expected);
    REQUIRE(strstr(gate->detail, scenario->detail) != NULL);
    REQUIRE(report->ready == !(required && scenario->expected == UMI_IDE_GATE_BLOCK));
    /* Readiness inspection must not rewrite the results to obtain a pass. */
    REQUIRE(umi_test_platform_result_registry_revision(
        umi_test_platform_service_result(service)) == registry_revision);
    result = EXIT_SUCCESS;

done:
    umi_test_platform_service_destroy(service);
    free(report);
    free(context);
    return result;
}

/* A named case supports isolated CTest diagnostics; no argument runs the same
 * boundaries together for a contributor investigating policy behavior. */
int main(int argc, char **argv)
{
    if (argc > 2) return EXIT_FAILURE;
    for (size_t index = 0U; index < sizeof(cases) / sizeof(cases[0]); ++index) {
        if (argc == 2 && strcmp(argv[1], cases[index].name) != 0) continue;
        if (RunCase(&cases[index]) != EXIT_SUCCESS) return EXIT_FAILURE;
        if (argc == 2) return EXIT_SUCCESS;
    }
    return argc == 1 ? EXIT_SUCCESS : EXIT_FAILURE;
}
