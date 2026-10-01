/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_rerun_plan.c
 *
 * PURPOSE:
 *   Verify the rerun plan runtime contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/test_runtime/rerun_plan.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
/* Compare the documented default fields, not struct padding. */
static int RecordDefaultsEqual(const UmiTestRuntimeRerunPlan *left, const UmiTestRuntimeRerunPlan *right)
{
    return left->structure_size == right->structure_size &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->detail, right->detail, sizeof(left->detail)) == 0 &&
        left->test_count == right->test_count &&
        left->attempt == right->attempt &&
        left->revision == right->revision &&
        left->enabled == right->enabled;
}
/* Public records may arrive from a caller's memory. Each fixed text field
 * must contain its own terminator; a later field cannot supply one for it. */
static int RecordRejectsUnterminatedFields(void)
{
    UmiTestRuntimeRerunPlan value;
    umi_test_runtime_rerun_plan_init(&value, "bounded-record");
    memset(value.id, 'x', sizeof(value.id));
    if (umi_test_runtime_rerun_plan_validate(&value) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    umi_test_runtime_rerun_plan_init(&value, "bounded-record");
    memset(value.detail, 'x', sizeof(value.detail));
    if (umi_test_runtime_rerun_plan_validate(&value) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    return 0;
}
#define RECORD_TYPE UmiTestRuntimeRerunPlan
#define RECORD_INIT umi_test_runtime_rerun_plan_init
#define RECORD_VALIDATE umi_test_runtime_rerun_plan_validate
#define RECORD_INIT_CHECKED umi_test_runtime_rerun_plan_init_checked
#include "../record_integrity/record_construction_cases.h"

/* Each mutation refusal must preserve the complete accepted value. These
 * checks remain active in optimized builds where assert may be disabled. */
static int RecordMutationCases(void)
{
    UmiTestRuntimeRerunPlan value;
    umi_test_runtime_rerun_plan_init(&value, "mutation-record");
    unsigned char before[sizeof(value)];
    umi_test_runtime_rerun_plan_init(&value, "mutation-record");
    {
        char oversized[sizeof(value.detail) + 1U];
        memset(oversized, 'x', sizeof(oversized));
        oversized[sizeof(oversized) - 1U] = '\0';
        if (umi_test_runtime_rerun_plan_set_detail(&value, "retained") != UMI_STATUS_OK) return 1;
        memcpy(before, &value, sizeof(value));
        if (umi_test_runtime_rerun_plan_set_detail(&value, oversized) != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
        if (memcmp(before, &value, sizeof(value)) != 0) return 1;
        if (umi_test_runtime_rerun_plan_set_detail(&value, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
        if (memcmp(before, &value, sizeof(value)) != 0) return 1;
        const uint64_t revision = value.revision;
        if (umi_test_runtime_rerun_plan_set_detail(&value, value.detail + 1U) != UMI_STATUS_OK) return 1;
        if (strcmp(value.detail, "etained") != 0 || value.revision != revision + 1U) return 1;
        oversized[sizeof(value.detail) - 1U] = '\0';
        if (umi_test_runtime_rerun_plan_set_detail(&value, oversized) != UMI_STATUS_OK) return 1;
        if (strcmp(value.detail, oversized) != 0) return 1;
    }
    umi_test_runtime_rerun_plan_init(&value, "exhausted-record");
    value.revision = UINT64_MAX;
    memcpy(before, &value, sizeof(value));
    if (umi_test_runtime_rerun_plan_set_detail(&value, "retained") != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
    if (memcmp(before, &value, sizeof(value)) != 0) return 1;
    umi_test_runtime_rerun_plan_init(&value, "exhausted-record");
    value.revision = UINT64_MAX;
    memcpy(before, &value, sizeof(value));
    if (umi_test_runtime_rerun_plan_set_test_count(&value, 7U) != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
    if (memcmp(before, &value, sizeof(value)) != 0) return 1;
    umi_test_runtime_rerun_plan_init(&value, "exhausted-record");
    value.revision = UINT64_MAX;
    memcpy(before, &value, sizeof(value));
    if (umi_test_runtime_rerun_plan_set_attempt(&value, 7U) != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
    if (memcmp(before, &value, sizeof(value)) != 0) return 1;
    UmiTestRuntimeRerunPlan other;
    umi_test_runtime_rerun_plan_init(&value, "identity-record");
    umi_test_runtime_rerun_plan_init(&other, "identity-record");
    if (!umi_test_runtime_rerun_plan_same_identity(&value, &other)) return 1;
    memcpy(before, &value, sizeof(value));
    memset(other.id, 'x', sizeof(other.id));
    if (umi_test_runtime_rerun_plan_same_identity(&value, &other) || umi_test_runtime_rerun_plan_same_identity(&other, &value)) return 1;
    if (memcmp(before, &value, sizeof(value)) != 0) return 1;
    return 0;
}

int main(void)
{
    if (RecordMutationCases() != 0) return 1;
    if (RecordConstructionCases() != 0) return 1;
    UmiTestRuntimeRerunPlan v,s;
    umi_test_runtime_rerun_plan_init(&v,"test-runtime.rerun_plan");
    assert(umi_test_runtime_rerun_plan_validate(&v)==UMI_STATUS_OK);
    assert(umi_test_runtime_rerun_plan_set_detail(&v,"regression evidence")==UMI_STATUS_OK);
    assert(umi_test_runtime_rerun_plan_set_test_count(&v,34U)==UMI_STATUS_OK);
    assert(umi_test_runtime_rerun_plan_set_attempt(&v,55U)==UMI_STATUS_OK);
    umi_test_runtime_rerun_plan_init(&s,"test-runtime.rerun_plan");
    assert(umi_test_runtime_rerun_plan_same_identity(&v,&s));
    return 0;
    }
