/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_platform/evidence.c
 * PURPOSE: Keep selected-test evidence ordering and ownership in Framework.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/test_platform/evidence.h"
#include <stdlib.h>
#include <string.h>

struct UmiTestEvidence {
    UmiTestEvidenceSummary summary;
    UmiTestPlatformResultSnapshot results[UMI_TEST_EVIDENCE_RESULT_LIMIT];
    UmiTestPlatformOutputSnapshot output[UMI_TEST_EVIDENCE_OUTPUT_LIMIT];
};

/* Sort small keys first, then copy only the retained payloads. This bounds
 * temporary memory and avoids repeatedly moving large output strings. */
typedef struct EvidenceKey {
    size_t index;
    uint64_t order;
    uint64_t revision;
    char id[128];
} EvidenceKey;

static int ValidId(const char *id, int allow_empty)
{
    if (id == NULL) return 0;
    for (size_t i = 0U; i < 128U; ++i) {
        if (id[i] == '\0') return allow_empty || i != 0U;
    }
    return 0;
}

static int CompareKeys(const void *left, const void *right)
{
    const EvidenceKey *a = left;
    const EvidenceKey *b = right;
    if (a->order != b->order) return a->order > b->order ? -1 : 1;
    if (a->revision != b->revision) return a->revision > b->revision ? -1 : 1;
    return strcmp(a->id, b->id);
}

static int Matches(const UmiTestEvidenceSummary *summary,
    const char *item_id, const char *session_id)
{
    return strcmp(summary->item_id, item_id) == 0 &&
        (summary->session_id[0] == '\0' ||
         strcmp(summary->session_id, session_id) == 0);
}

UmiStatus UmiTestEvidenceCreate(const UmiTestPlatformResultRegistry *results,
    const UmiTestPlatformOutputRegistry *output, const char *item_id,
    const char *session_id, UmiTestEvidence **out_evidence)
{
    if (out_evidence == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_evidence = NULL;
    if (session_id == NULL) session_id = "";
    if (results == NULL || output == NULL || !ValidId(item_id, 0) ||
        !ValidId(session_id, 1)) return UMI_STATUS_INVALID_ARGUMENT;
    size_t result_count = umi_test_platform_result_registry_count(results);
    size_t output_count = umi_test_platform_output_registry_count(output);
    if (result_count > UMI_TEST_PLATFORM_RESULT_CAPACITY ||
        output_count > UMI_TEST_PLATFORM_OUTPUT_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    size_t key_count = result_count > output_count ? result_count : output_count;
    UmiTestEvidence *capture = calloc(1U, sizeof(*capture));
    EvidenceKey *keys = calloc(key_count != 0U ? key_count : 1U, sizeof(*keys));
    if (capture == NULL || keys == NULL) {
        free(capture);
        free(keys);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    strcpy(capture->summary.item_id, item_id);
    strcpy(capture->summary.session_id, session_id);
    capture->summary.result_revision = umi_test_platform_result_registry_revision(results);
    capture->summary.output_revision = umi_test_platform_output_registry_revision(output);
    UmiStatus status = UMI_STATUS_OK;
    size_t matched = 0U;
    for (size_t i = 0U; i < result_count; ++i) {
        UmiTestPlatformResultSnapshot row;
        status = umi_test_platform_result_registry_at(results, i, &row);
        if (status != UMI_STATUS_OK) goto failure;
        if (!Matches(&capture->summary, row.item_id, row.session_id)) continue;
        keys[matched].index = i;
        keys[matched].order = row.sequence;
        keys[matched].revision = row.revision;
        strcpy(keys[matched++].id, row.id);
    }
    qsort(keys, matched, sizeof(*keys), CompareKeys);
    capture->summary.matching_results = matched;
    capture->summary.retained_results = matched < UMI_TEST_EVIDENCE_RESULT_LIMIT
        ? matched : UMI_TEST_EVIDENCE_RESULT_LIMIT;
    for (size_t i = 0U; i < capture->summary.retained_results; ++i) {
        status = umi_test_platform_result_registry_at(results, keys[i].index, &capture->results[i]);
        if (status != UMI_STATUS_OK) goto failure;
    }
    matched = 0U;
    for (size_t i = 0U; i < output_count; ++i) {
        UmiTestPlatformOutputSnapshot row;
        status = umi_test_platform_output_registry_at(output, i, &row);
        if (status != UMI_STATUS_OK) goto failure;
        if (!Matches(&capture->summary, row.item_id, row.session_id)) continue;
        keys[matched].index = i;
        keys[matched].order = row.timestamp;
        keys[matched].revision = row.revision;
        strcpy(keys[matched++].id, row.id);
    }
    qsort(keys, matched, sizeof(*keys), CompareKeys);
    capture->summary.matching_output = matched;
    capture->summary.retained_output = matched < UMI_TEST_EVIDENCE_OUTPUT_LIMIT
        ? matched : UMI_TEST_EVIDENCE_OUTPUT_LIMIT;
    for (size_t i = 0U; i < capture->summary.retained_output; ++i) {
        status = umi_test_platform_output_registry_at(output, keys[i].index, &capture->output[i]);
        if (status != UMI_STATUS_OK) goto failure;
    }
    if (capture->summary.result_revision != umi_test_platform_result_registry_revision(results) ||
        capture->summary.output_revision != umi_test_platform_output_registry_revision(output)) {
        status = UMI_STATUS_INVALID_STATE;
        goto failure;
    }
    free(keys);
    *out_evidence = capture;
    return UMI_STATUS_OK;
failure:
    free(keys);
    free(capture);
    return status;
}

void UmiTestEvidenceDestroy(UmiTestEvidence *evidence) { free(evidence); }
UmiStatus UmiTestEvidenceGetSummary(const UmiTestEvidence *evidence,
    UmiTestEvidenceSummary *out_summary)
{
    if (evidence == NULL || out_summary == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_summary = evidence->summary;
    return UMI_STATUS_OK;
}
UmiStatus UmiTestEvidenceResultAt(const UmiTestEvidence *evidence, size_t index,
    UmiTestPlatformResultSnapshot *out_result)
{
    if (evidence == NULL || out_result == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= evidence->summary.retained_results) return UMI_STATUS_NOT_FOUND;
    *out_result = evidence->results[index];
    return UMI_STATUS_OK;
}
UmiStatus UmiTestEvidenceOutputAt(const UmiTestEvidence *evidence, size_t index,
    UmiTestPlatformOutputSnapshot *out_output)
{
    if (evidence == NULL || out_output == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= evidence->summary.retained_output) return UMI_STATUS_NOT_FOUND;
    *out_output = evidence->output[index];
    return UMI_STATUS_OK;
}
