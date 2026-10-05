/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_runtime_dependency.c
 *
 * PURPOSE:
 *   Verify the runtime dependency contract, bounded text and revision behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "umicom/test_runtime/runtime_dependency.h"

/* A refused edit must preserve the whole record, including its observation
 * token. Capture bytes from this same object so padding is compared only for
 * an operation that promises to perform no writes. */
static int CheckRefusedEdits(void)
{
    UmiTestRuntimeRuntimeDependency value;
    umi_test_runtime_runtime_dependency_init(&value, "publication-check");
    unsigned char before[sizeof(value)];
    {
        char oversized[sizeof(value.detail) + 1U];
        memset(oversized, 'x', sizeof(oversized));
        oversized[sizeof(oversized) - 1U] = '\0';
        if (umi_test_runtime_runtime_dependency_set_detail(&value, "retained") != UMI_STATUS_OK) return 1;
        memcpy(before, &value, sizeof(value));
        if (umi_test_runtime_runtime_dependency_set_detail(&value, oversized) != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
        if (memcmp(before, &value, sizeof(value)) != 0) return 1;
        if (umi_test_runtime_runtime_dependency_set_detail(&value, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
        if (memcmp(before, &value, sizeof(value)) != 0) return 1;
        uint64_t observed = value.revision;
        if (umi_test_runtime_runtime_dependency_set_detail(&value, value.detail + 1) != UMI_STATUS_OK) return 1;
        if (strcmp(value.detail, "etained") != 0 || value.revision != observed + 1U) return 1;
    }
    {
        char oversized[sizeof(value.name) + 1U];
        memset(oversized, 'x', sizeof(oversized));
        oversized[sizeof(oversized) - 1U] = '\0';
        if (umi_test_runtime_runtime_dependency_set_name(&value, "retained") != UMI_STATUS_OK) return 1;
        memcpy(before, &value, sizeof(value));
        if (umi_test_runtime_runtime_dependency_set_name(&value, oversized) != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
        if (memcmp(before, &value, sizeof(value)) != 0) return 1;
        if (umi_test_runtime_runtime_dependency_set_name(&value, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
        if (memcmp(before, &value, sizeof(value)) != 0) return 1;
        uint64_t observed = value.revision;
        if (umi_test_runtime_runtime_dependency_set_name(&value, value.name + 1) != UMI_STATUS_OK) return 1;
        if (strcmp(value.name, "etained") != 0 || value.revision != observed + 1U) return 1;
    }
    /* The final valid edit may reach the boundary. Every later mutator must
     * refuse before changing any field, rather than wrapping the counter. */
    value.revision = UINT64_MAX - 1U;
    if (umi_test_runtime_runtime_dependency_set_name(&value, "Regression Runtime") != UMI_STATUS_OK || value.revision != UINT64_MAX) return 1;
    memcpy(before, &value, sizeof(value));
    if (umi_test_runtime_runtime_dependency_set_name(&value, "Regression Runtime") != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
    if (memcmp(before, &value, sizeof(value)) != 0) return 1;
    if (umi_test_runtime_runtime_dependency_set_detail(&value, "deterministic evidence") != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
    if (memcmp(before, &value, sizeof(value)) != 0) return 1;
    if (umi_test_runtime_runtime_dependency_set_required(&value, 7U) != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
    if (memcmp(before, &value, sizeof(value)) != 0) return 1;
    if (umi_test_runtime_runtime_dependency_set_resolved(&value, 11U) != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
    if (memcmp(before, &value, sizeof(value)) != 0) return 1;
    if (umi_test_runtime_runtime_dependency_touch(&value, 1234U) != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
    if (memcmp(before, &value, sizeof(value)) != 0) return 1;
    return 0;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */


/* A review edits a private record before it reaches the live owner. Check
 * stale proposals, identity mistakes and invalid fields without using assert,
 * so these checks also execute when the test is built with NDEBUG. */
static int CheckReviewedRecord(void)
{
    UmiTestRuntimeRuntimeDependency value;
    umi_test_runtime_runtime_dependency_init(&value, "reviewed-record");
    uint64_t observed = value.revision;
    UmiTestRuntimeRuntimeDependency proposal = value;
    unsigned char before[sizeof(value)], proposed_before[sizeof(proposal)];
    memcpy(before, &value, sizeof(value));
    if (umi_test_runtime_runtime_dependency_set_detail(&proposal, "reviewed detail") != UMI_STATUS_OK) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    if (umi_test_runtime_runtime_dependency_set_name(&proposal, "reviewed name") != UMI_STATUS_OK) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    if (umi_test_runtime_runtime_dependency_set_required(&proposal, 7U) != UMI_STATUS_OK) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    if (umi_test_runtime_runtime_dependency_set_resolved(&proposal, 11U) != UMI_STATUS_OK) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    if (umi_test_runtime_runtime_dependency_touch(&proposal, 1234U) != UMI_STATUS_OK) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    /* A proposal may have its own history; it cannot choose the live token. */
    proposal.revision = UINT64_MAX;
    memcpy(proposed_before, &proposal, sizeof(proposal));
    if (umi_test_runtime_runtime_dependency_replace_if_current(&value, observed, &proposal) != UMI_STATUS_OK) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    if (value.revision != observed + 1U) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    if (memcmp(proposed_before, &proposal, sizeof(proposal)) != 0) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    if (value.structure_size != proposal.structure_size) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    if (memcmp(value.id, proposal.id, sizeof(value.id)) != 0) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    if (memcmp(value.name, proposal.name, sizeof(value.name)) != 0) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    if (memcmp(value.detail, proposal.detail, sizeof(value.detail)) != 0) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    if (value.required != proposal.required) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    if (value.resolved != proposal.resolved) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    if (value.updated_at_ms != proposal.updated_at_ms) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    if (value.enabled != proposal.enabled) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    memcpy(before, &value, sizeof(value));
    if (umi_test_runtime_runtime_dependency_replace_if_current(&value, observed, &proposal) != UMI_STATUS_INVALID_STATE) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    if (memcmp(before, &value, sizeof(value)) != 0) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    observed = value.revision;
    proposal.id[0] = 'x';
    if (umi_test_runtime_runtime_dependency_replace_if_current(&value, observed, &proposal) != UMI_STATUS_INVALID_ARGUMENT) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    if (memcmp(before, &value, sizeof(value)) != 0) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    proposal = value;
    proposal.structure_size = 0U;
    if (umi_test_runtime_runtime_dependency_replace_if_current(&value, observed, &proposal) != UMI_STATUS_INVALID_ARGUMENT) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    if (memcmp(before, &value, sizeof(value)) != 0) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    proposal = value;
    memset(proposal.id, 'x', sizeof(proposal.id));
    if (umi_test_runtime_runtime_dependency_replace_if_current(&value, observed, &proposal) != UMI_STATUS_INVALID_ARGUMENT) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    if (memcmp(before, &value, sizeof(value)) != 0) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    proposal = value;
    memset(proposal.name, 'x', sizeof(proposal.name));
    if (umi_test_runtime_runtime_dependency_replace_if_current(&value, observed, &proposal) != UMI_STATUS_INVALID_ARGUMENT) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    if (memcmp(before, &value, sizeof(value)) != 0) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    proposal = value;
    memset(proposal.detail, 'x', sizeof(proposal.detail));
    if (umi_test_runtime_runtime_dependency_replace_if_current(&value, observed, &proposal) != UMI_STATUS_INVALID_ARGUMENT) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    if (memcmp(before, &value, sizeof(value)) != 0) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    if (umi_test_runtime_runtime_dependency_replace_if_current(&value, observed, NULL) != UMI_STATUS_INVALID_ARGUMENT) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    if (umi_test_runtime_runtime_dependency_replace_if_current(NULL, observed, &proposal) != UMI_STATUS_INVALID_ARGUMENT) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    if (memcmp(before, &value, sizeof(value)) != 0) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    /* Even an unchanged self-publication receives a fresh observation token. */
    if (umi_test_runtime_runtime_dependency_replace_if_current(&value, observed, &value) != UMI_STATUS_OK) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    if (value.revision != observed + 1U) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    value.revision = UINT64_MAX - 1U;
    proposal = value;
    if (umi_test_runtime_runtime_dependency_replace_if_current(&value, UINT64_MAX - 1U, &proposal) != UMI_STATUS_OK) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    if (value.revision != UINT64_MAX) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    memcpy(before, &value, sizeof(value));
    if (umi_test_runtime_runtime_dependency_replace_if_current(&value, UINT64_MAX, &proposal) != UMI_STATUS_CAPACITY_EXCEEDED) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    if (memcmp(before, &value, sizeof(value)) != 0) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    return 0;
}

/* Compare the documented default fields, not struct padding. */
static int RecordDefaultsEqual(const UmiTestRuntimeRuntimeDependency *left, const UmiTestRuntimeRuntimeDependency *right)
{
    return left->structure_size == right->structure_size &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->name, right->name, sizeof(left->name)) == 0 &&
        memcmp(left->detail, right->detail, sizeof(left->detail)) == 0 &&
        left->required == right->required &&
        left->resolved == right->resolved &&
        left->updated_at_ms == right->updated_at_ms &&
        left->revision == right->revision &&
        left->enabled == right->enabled;
}
/* Public records may arrive from a caller's memory. Each fixed text field
 * must contain its own terminator; a later field cannot supply one for it. */
static int RecordRejectsUnterminatedFields(void)
{
    UmiTestRuntimeRuntimeDependency value;
    umi_test_runtime_runtime_dependency_init(&value, "bounded-record");
    memset(value.id, 'x', sizeof(value.id));
    if (umi_test_runtime_runtime_dependency_validate(&value) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    umi_test_runtime_runtime_dependency_init(&value, "bounded-record");
    memset(value.name, 'x', sizeof(value.name));
    if (umi_test_runtime_runtime_dependency_validate(&value) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    umi_test_runtime_runtime_dependency_init(&value, "bounded-record");
    memset(value.detail, 'x', sizeof(value.detail));
    if (umi_test_runtime_runtime_dependency_validate(&value) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    return 0;
}
#define RECORD_TYPE UmiTestRuntimeRuntimeDependency
#define RECORD_INIT umi_test_runtime_runtime_dependency_init
#define RECORD_VALIDATE umi_test_runtime_runtime_dependency_validate
#define RECORD_INIT_CHECKED umi_test_runtime_runtime_dependency_init_checked
#include "../record_integrity/record_construction_cases.h"

/* Each mutation refusal must preserve the complete accepted value. These
 * checks remain active in optimized builds where assert may be disabled. */
static int RecordMutationCases(void)
{
    UmiTestRuntimeRuntimeDependency value;
    umi_test_runtime_runtime_dependency_init(&value, "mutation-record");
    unsigned char before[sizeof(value)];
    UmiTestRuntimeRuntimeDependency other;
    umi_test_runtime_runtime_dependency_init(&value, "identity-record");
    umi_test_runtime_runtime_dependency_init(&other, "identity-record");
    if (!umi_test_runtime_runtime_dependency_same_identity(&value, &other)) return 1;
    memcpy(before, &value, sizeof(value));
    memset(other.id, 'x', sizeof(other.id));
    if (umi_test_runtime_runtime_dependency_same_identity(&value, &other) || umi_test_runtime_runtime_dependency_same_identity(&other, &value)) return 1;
    if (memcmp(before, &value, sizeof(value)) != 0) return 1;
    return 0;
}

/* Use nonzero domain fields to expose a codec that accidentally drops
 * values. The existing initializer supplies required compatibility metadata. */
static UmiTestRuntimeRuntimeDependency ArchiveSample(void)
{
    UmiTestRuntimeRuntimeDependency value;
    umi_test_runtime_runtime_dependency_init(&value, "archive-record");
    value.name[0] = 'a';
    value.detail[0] = 'a';
    value.required = (uint64_t)6U;
    value.resolved = (uint64_t)7U;
    value.updated_at_ms = (uint64_t)8U;
    value.revision = (uint64_t)9U;
    value.enabled = true;
    return value;
}
/* Unused tails are not part of a C string and must not enter saved bytes. */
static void ArchiveFillUnusedText(UmiTestRuntimeRuntimeDependency *value)
{
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->name) + 1U;
        memset(value->name + used, 0xa5, sizeof(value->name) - used);
    }
    {
        size_t used = strlen(value->detail) + 1U;
        memset(value->detail + used, 0xa5, sizeof(value->detail) - used);
    }
}
#define ARCHIVE_TYPE UmiTestRuntimeRuntimeDependency
#define ARCHIVE_ENCODE umi_test_runtime_runtime_dependency_archive_encode
#define ARCHIVE_DECODE umi_test_runtime_runtime_dependency_archive_decode
#define ARCHIVE_EQUAL RecordDefaultsEqual
#include "../value_archive/record_cases.h"

int main(void)
{
    if (ArchiveRecordCases() != 0) return 1;
    if (RecordMutationCases() != 0) return 1;
    if (RecordConstructionCases() != 0) return 1;
    if (CheckReviewedRecord() != 0) return 1;
    if (CheckRefusedEdits() != 0) return 1;
    UmiTestRuntimeRuntimeDependency value;
    UmiTestRuntimeRuntimeDependency same;
    uint64_t revision;
    umi_test_runtime_runtime_dependency_init(&value, "test-runtime.runtime_dependency");
    assert(value.structure_size == sizeof(value));
    assert(value.enabled);
    assert(umi_test_runtime_runtime_dependency_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_test_runtime_runtime_dependency_set_name(&value, "Regression Runtime") == UMI_STATUS_OK);
    assert(umi_test_runtime_runtime_dependency_set_detail(&value, "deterministic evidence") == UMI_STATUS_OK);
    assert(umi_test_runtime_runtime_dependency_set_required(&value, 7U) == UMI_STATUS_OK);
    assert(umi_test_runtime_runtime_dependency_set_resolved(&value, 11U) == UMI_STATUS_OK);
    assert(umi_test_runtime_runtime_dependency_touch(&value, 1234U) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.required == 7U);
    assert(value.resolved == 11U);
    assert(strcmp(value.name, "Regression Runtime") == 0);
    umi_test_runtime_runtime_dependency_init(&same, "test-runtime.runtime_dependency");
    assert(umi_test_runtime_runtime_dependency_same_identity(&value, &same));
    assert(umi_test_runtime_runtime_dependency_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
