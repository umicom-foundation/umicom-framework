/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_result.c
 *
 * PURPOSE:
 *   Verify the result contract, bounded text and revision behaviour.
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
#include "umicom/test_runtime/result.h"

/* A refused edit must preserve the whole record, including its observation
 * token. Capture bytes from this same object so padding is compared only for
 * an operation that promises to perform no writes. */
static int CheckRefusedEdits(void)
{
    UmiTestRuntimeResult value;
    umi_test_runtime_result_init(&value, "publication-check");
    unsigned char before[sizeof(value)];
    {
        char oversized[sizeof(value.detail) + 1U];
        memset(oversized, 'x', sizeof(oversized));
        oversized[sizeof(oversized) - 1U] = '\0';
        if (umi_test_runtime_result_set_detail(&value, "retained") != UMI_STATUS_OK) return 1;
        memcpy(before, &value, sizeof(value));
        if (umi_test_runtime_result_set_detail(&value, oversized) != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
        if (memcmp(before, &value, sizeof(value)) != 0) return 1;
        if (umi_test_runtime_result_set_detail(&value, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
        if (memcmp(before, &value, sizeof(value)) != 0) return 1;
        uint64_t observed = value.revision;
        if (umi_test_runtime_result_set_detail(&value, value.detail + 1) != UMI_STATUS_OK) return 1;
        if (strcmp(value.detail, "etained") != 0 || value.revision != observed + 1U) return 1;
    }
    {
        char oversized[sizeof(value.name) + 1U];
        memset(oversized, 'x', sizeof(oversized));
        oversized[sizeof(oversized) - 1U] = '\0';
        if (umi_test_runtime_result_set_name(&value, "retained") != UMI_STATUS_OK) return 1;
        memcpy(before, &value, sizeof(value));
        if (umi_test_runtime_result_set_name(&value, oversized) != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
        if (memcmp(before, &value, sizeof(value)) != 0) return 1;
        if (umi_test_runtime_result_set_name(&value, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
        if (memcmp(before, &value, sizeof(value)) != 0) return 1;
        uint64_t observed = value.revision;
        if (umi_test_runtime_result_set_name(&value, value.name + 1) != UMI_STATUS_OK) return 1;
        if (strcmp(value.name, "etained") != 0 || value.revision != observed + 1U) return 1;
    }
    /* The final valid edit may reach the boundary. Every later mutator must
     * refuse before changing any field, rather than wrapping the counter. */
    value.revision = UINT64_MAX - 1U;
    if (umi_test_runtime_result_set_name(&value, "Regression Runtime") != UMI_STATUS_OK || value.revision != UINT64_MAX) return 1;
    memcpy(before, &value, sizeof(value));
    if (umi_test_runtime_result_set_name(&value, "Regression Runtime") != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
    if (memcmp(before, &value, sizeof(value)) != 0) return 1;
    if (umi_test_runtime_result_set_detail(&value, "deterministic evidence") != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
    if (memcmp(before, &value, sizeof(value)) != 0) return 1;
    if (umi_test_runtime_result_set_duration_ms(&value, 7U) != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
    if (memcmp(before, &value, sizeof(value)) != 0) return 1;
    if (umi_test_runtime_result_set_exit_code(&value, 11U) != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
    if (memcmp(before, &value, sizeof(value)) != 0) return 1;
    if (umi_test_runtime_result_touch(&value, 1234U) != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
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
    UmiTestRuntimeResult value;
    umi_test_runtime_result_init(&value, "reviewed-record");
    uint64_t observed = value.revision;
    UmiTestRuntimeResult proposal = value;
    unsigned char before[sizeof(value)], proposed_before[sizeof(proposal)];
    memcpy(before, &value, sizeof(value));
    if (umi_test_runtime_result_set_detail(&proposal, "reviewed detail") != UMI_STATUS_OK) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    if (umi_test_runtime_result_set_name(&proposal, "reviewed name") != UMI_STATUS_OK) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    if (umi_test_runtime_result_set_duration_ms(&proposal, 7U) != UMI_STATUS_OK) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    if (umi_test_runtime_result_set_exit_code(&proposal, 11U) != UMI_STATUS_OK) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    if (umi_test_runtime_result_touch(&proposal, 1234U) != UMI_STATUS_OK) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    /* A proposal may have its own history; it cannot choose the live token. */
    proposal.revision = UINT64_MAX;
    memcpy(proposed_before, &proposal, sizeof(proposal));
    if (umi_test_runtime_result_replace_if_current(&value, observed, &proposal) != UMI_STATUS_OK) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    if (value.revision != observed + 1U) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    if (memcmp(proposed_before, &proposal, sizeof(proposal)) != 0) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    if (value.structure_size != proposal.structure_size) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    if (memcmp(value.id, proposal.id, sizeof(value.id)) != 0) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    if (memcmp(value.name, proposal.name, sizeof(value.name)) != 0) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    if (memcmp(value.detail, proposal.detail, sizeof(value.detail)) != 0) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    if (value.duration_ms != proposal.duration_ms) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    if (value.exit_code != proposal.exit_code) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    if (value.updated_at_ms != proposal.updated_at_ms) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    if (value.enabled != proposal.enabled) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    memcpy(before, &value, sizeof(value));
    if (umi_test_runtime_result_replace_if_current(&value, observed, &proposal) != UMI_STATUS_INVALID_STATE) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    if (memcmp(before, &value, sizeof(value)) != 0) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    observed = value.revision;
    proposal.id[0] = 'x';
    if (umi_test_runtime_result_replace_if_current(&value, observed, &proposal) != UMI_STATUS_INVALID_ARGUMENT) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    if (memcmp(before, &value, sizeof(value)) != 0) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    proposal = value;
    proposal.structure_size = 0U;
    if (umi_test_runtime_result_replace_if_current(&value, observed, &proposal) != UMI_STATUS_INVALID_ARGUMENT) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    if (memcmp(before, &value, sizeof(value)) != 0) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    proposal = value;
    memset(proposal.id, 'x', sizeof(proposal.id));
    if (umi_test_runtime_result_replace_if_current(&value, observed, &proposal) != UMI_STATUS_INVALID_ARGUMENT) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    if (memcmp(before, &value, sizeof(value)) != 0) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    proposal = value;
    memset(proposal.name, 'x', sizeof(proposal.name));
    if (umi_test_runtime_result_replace_if_current(&value, observed, &proposal) != UMI_STATUS_INVALID_ARGUMENT) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    if (memcmp(before, &value, sizeof(value)) != 0) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    proposal = value;
    memset(proposal.detail, 'x', sizeof(proposal.detail));
    if (umi_test_runtime_result_replace_if_current(&value, observed, &proposal) != UMI_STATUS_INVALID_ARGUMENT) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    if (memcmp(before, &value, sizeof(value)) != 0) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    if (umi_test_runtime_result_replace_if_current(&value, observed, NULL) != UMI_STATUS_INVALID_ARGUMENT) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    if (umi_test_runtime_result_replace_if_current(NULL, observed, &proposal) != UMI_STATUS_INVALID_ARGUMENT) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    if (memcmp(before, &value, sizeof(value)) != 0) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    /* Even an unchanged self-publication receives a fresh observation token. */
    if (umi_test_runtime_result_replace_if_current(&value, observed, &value) != UMI_STATUS_OK) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    if (value.revision != observed + 1U) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    value.revision = UINT64_MAX - 1U;
    proposal = value;
    if (umi_test_runtime_result_replace_if_current(&value, UINT64_MAX - 1U, &proposal) != UMI_STATUS_OK) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    if (value.revision != UINT64_MAX) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    memcpy(before, &value, sizeof(value));
    if (umi_test_runtime_result_replace_if_current(&value, UINT64_MAX, &proposal) != UMI_STATUS_CAPACITY_EXCEEDED) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    if (memcmp(before, &value, sizeof(value)) != 0) { fprintf(stderr, "%s:%d: reviewed record check failed\n", __FILE__, __LINE__); return 1; }
    return 0;
}

int main(void)
{
    if (CheckReviewedRecord() != 0) return 1;
    if (CheckRefusedEdits() != 0) return 1;
    UmiTestRuntimeResult value;
    UmiTestRuntimeResult same;
    uint64_t revision;
    umi_test_runtime_result_init(&value, "test-runtime.result");
    assert(value.structure_size == sizeof(value));
    assert(value.enabled);
    assert(umi_test_runtime_result_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_test_runtime_result_set_name(&value, "Regression Runtime") == UMI_STATUS_OK);
    assert(umi_test_runtime_result_set_detail(&value, "deterministic evidence") == UMI_STATUS_OK);
    assert(umi_test_runtime_result_set_duration_ms(&value, 7U) == UMI_STATUS_OK);
    assert(umi_test_runtime_result_set_exit_code(&value, 11U) == UMI_STATUS_OK);
    assert(umi_test_runtime_result_touch(&value, 1234U) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.duration_ms == 7U);
    assert(value.exit_code == 11U);
    assert(strcmp(value.name, "Regression Runtime") == 0);
    umi_test_runtime_result_init(&same, "test-runtime.result");
    assert(umi_test_runtime_result_same_identity(&value, &same));
    assert(umi_test_runtime_result_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
