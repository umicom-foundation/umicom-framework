/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_diagnostic_bundle.c
 *
 * PURPOSE:
 *   Verify the diagnostic bundle runtime contract.
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
#include "umicom/test_runtime/diagnostic_bundle.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
/* Compare the documented default fields, not struct padding. */
static int RecordDefaultsEqual(const UmiTestRuntimeDiagnosticBundle *left, const UmiTestRuntimeDiagnosticBundle *right)
{
    return left->structure_size == right->structure_size &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->category, right->category, sizeof(left->category)) == 0 &&
        memcmp(left->detail, right->detail, sizeof(left->detail)) == 0 &&
        left->artifact_count == right->artifact_count &&
        left->generation == right->generation &&
        left->revision == right->revision &&
        left->active == right->active;
}
/* Public records may arrive from a caller's memory. Each fixed text field
 * must contain its own terminator; a later field cannot supply one for it. */
static int RecordRejectsUnterminatedFields(void)
{
    UmiTestRuntimeDiagnosticBundle value;
    umi_test_runtime_diagnostic_bundle_init(&value, "bounded-record");
    memset(value.id, 'x', sizeof(value.id));
    if (umi_test_runtime_diagnostic_bundle_validate(&value) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    umi_test_runtime_diagnostic_bundle_init(&value, "bounded-record");
    memset(value.category, 'x', sizeof(value.category));
    if (umi_test_runtime_diagnostic_bundle_validate(&value) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    umi_test_runtime_diagnostic_bundle_init(&value, "bounded-record");
    memset(value.detail, 'x', sizeof(value.detail));
    if (umi_test_runtime_diagnostic_bundle_validate(&value) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    return 0;
}
#define RECORD_TYPE UmiTestRuntimeDiagnosticBundle
#define RECORD_INIT umi_test_runtime_diagnostic_bundle_init
#define RECORD_VALIDATE umi_test_runtime_diagnostic_bundle_validate
#define RECORD_INIT_CHECKED umi_test_runtime_diagnostic_bundle_init_checked
#include "../record_integrity/record_construction_cases.h"

/* Each mutation refusal must preserve the complete accepted value. These
 * checks remain active in optimized builds where assert may be disabled. */
static int RecordMutationCases(void)
{
    UmiTestRuntimeDiagnosticBundle value;
    umi_test_runtime_diagnostic_bundle_init(&value, "mutation-record");
    unsigned char before[sizeof(value)];
    umi_test_runtime_diagnostic_bundle_init(&value, "mutation-record");
    {
        char oversized[sizeof(value.category) + 1U];
        memset(oversized, 'x', sizeof(oversized));
        oversized[sizeof(oversized) - 1U] = '\0';
        if (umi_test_runtime_diagnostic_bundle_set_category(&value, "retained") != UMI_STATUS_OK) return 1;
        memcpy(before, &value, sizeof(value));
        if (umi_test_runtime_diagnostic_bundle_set_category(&value, oversized) != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
        if (memcmp(before, &value, sizeof(value)) != 0) return 1;
        if (umi_test_runtime_diagnostic_bundle_set_category(&value, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
        if (memcmp(before, &value, sizeof(value)) != 0) return 1;
        const uint64_t revision = value.revision;
        if (umi_test_runtime_diagnostic_bundle_set_category(&value, value.category + 1U) != UMI_STATUS_OK) return 1;
        if (strcmp(value.category, "etained") != 0 || value.revision != revision + 1U) return 1;
        oversized[sizeof(value.category) - 1U] = '\0';
        if (umi_test_runtime_diagnostic_bundle_set_category(&value, oversized) != UMI_STATUS_OK) return 1;
        if (strcmp(value.category, oversized) != 0) return 1;
    }
    umi_test_runtime_diagnostic_bundle_init(&value, "exhausted-record");
    value.revision = UINT64_MAX;
    memcpy(before, &value, sizeof(value));
    if (umi_test_runtime_diagnostic_bundle_set_category(&value, "retained") != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
    if (memcmp(before, &value, sizeof(value)) != 0) return 1;
    umi_test_runtime_diagnostic_bundle_init(&value, "mutation-record");
    {
        char oversized[sizeof(value.detail) + 1U];
        memset(oversized, 'x', sizeof(oversized));
        oversized[sizeof(oversized) - 1U] = '\0';
        if (umi_test_runtime_diagnostic_bundle_set_detail(&value, "retained") != UMI_STATUS_OK) return 1;
        memcpy(before, &value, sizeof(value));
        if (umi_test_runtime_diagnostic_bundle_set_detail(&value, oversized) != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
        if (memcmp(before, &value, sizeof(value)) != 0) return 1;
        if (umi_test_runtime_diagnostic_bundle_set_detail(&value, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
        if (memcmp(before, &value, sizeof(value)) != 0) return 1;
        const uint64_t revision = value.revision;
        if (umi_test_runtime_diagnostic_bundle_set_detail(&value, value.detail + 1U) != UMI_STATUS_OK) return 1;
        if (strcmp(value.detail, "etained") != 0 || value.revision != revision + 1U) return 1;
        oversized[sizeof(value.detail) - 1U] = '\0';
        if (umi_test_runtime_diagnostic_bundle_set_detail(&value, oversized) != UMI_STATUS_OK) return 1;
        if (strcmp(value.detail, oversized) != 0) return 1;
    }
    umi_test_runtime_diagnostic_bundle_init(&value, "exhausted-record");
    value.revision = UINT64_MAX;
    memcpy(before, &value, sizeof(value));
    if (umi_test_runtime_diagnostic_bundle_set_detail(&value, "retained") != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
    if (memcmp(before, &value, sizeof(value)) != 0) return 1;
    umi_test_runtime_diagnostic_bundle_init(&value, "exhausted-record");
    value.revision = UINT64_MAX;
    memcpy(before, &value, sizeof(value));
    if (umi_test_runtime_diagnostic_bundle_set_artifact_count(&value, 7U) != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
    if (memcmp(before, &value, sizeof(value)) != 0) return 1;
    umi_test_runtime_diagnostic_bundle_init(&value, "exhausted-record");
    value.revision = UINT64_MAX;
    memcpy(before, &value, sizeof(value));
    if (umi_test_runtime_diagnostic_bundle_set_generation(&value, 7U) != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
    if (memcmp(before, &value, sizeof(value)) != 0) return 1;
    umi_test_runtime_diagnostic_bundle_init(&value, "exhausted-record");
    value.revision = UINT64_MAX;
    memcpy(before, &value, sizeof(value));
    if (umi_test_runtime_diagnostic_bundle_set_active(&value, true) != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
    if (memcmp(before, &value, sizeof(value)) != 0) return 1;
    UmiTestRuntimeDiagnosticBundle other;
    umi_test_runtime_diagnostic_bundle_init(&value, "identity-record");
    umi_test_runtime_diagnostic_bundle_init(&other, "identity-record");
    if (!umi_test_runtime_diagnostic_bundle_same_identity(&value, &other)) return 1;
    memcpy(before, &value, sizeof(value));
    memset(other.id, 'x', sizeof(other.id));
    if (umi_test_runtime_diagnostic_bundle_same_identity(&value, &other) || umi_test_runtime_diagnostic_bundle_same_identity(&other, &value)) return 1;
    if (memcmp(before, &value, sizeof(value)) != 0) return 1;
    return 0;
}

int main(void)
{
    if (RecordMutationCases() != 0) return 1;
    if (RecordConstructionCases() != 0) return 1;
    UmiTestRuntimeDiagnosticBundle value,same;
    uint64_t r;
    umi_test_runtime_diagnostic_bundle_init(&value,"test-runtime.diagnostic_bundle");
    assert(umi_test_runtime_diagnostic_bundle_validate(&value)==UMI_STATUS_OK);
    r=value.revision;
    assert(umi_test_runtime_diagnostic_bundle_set_category(&value,"regression")==UMI_STATUS_OK);
    assert(umi_test_runtime_diagnostic_bundle_set_detail(&value,"evidence")==UMI_STATUS_OK);
    assert(umi_test_runtime_diagnostic_bundle_set_artifact_count(&value,13U)==UMI_STATUS_OK);
    assert(umi_test_runtime_diagnostic_bundle_set_generation(&value,21U)==UMI_STATUS_OK);
    assert(umi_test_runtime_diagnostic_bundle_set_active(&value,false)==UMI_STATUS_OK);
    assert(value.revision>r);
    assert(value.artifact_count==13U&&value.generation==21U);
    umi_test_runtime_diagnostic_bundle_init(&same,"test-runtime.diagnostic_bundle");
    assert(umi_test_runtime_diagnostic_bundle_same_identity(&value,&same));
    return 0;
    }
