/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_platform/evidence_runs.c
 * PURPOSE: Share run discovery and checked evidence selection across applications.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/test_platform/evidence_runs.h"
#include <stdlib.h>
#include <string.h>

struct UmiTestEvidenceRuns {
    UmiTestEvidenceRunsSummary summary;
    const UmiTestPlatformResultRegistry *results;
    const UmiTestPlatformOutputRegistry *output;
    UmiTestEvidenceRun *rows;
};

static int RunIdValid(const char *id, int empty)
{
    if (id == NULL) return 0;
    for (size_t i = 0U; i < 128U; ++i)
        if (id[i] == '\0') return empty || i != 0U;
    return 0;
}

static int RunCompare(const void *left, const void *right)
{
    return strcmp(((const UmiTestEvidenceRun *)left)->session_id,
        ((const UmiTestEvidenceRun *)right)->session_id);
}

UmiStatus UmiTestEvidenceRunsValidate(const UmiTestEvidenceRuns *runs,
    const UmiTestPlatformResultRegistry *results,
    const UmiTestPlatformOutputRegistry *output)
{
    if (runs == NULL || results == NULL || output == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (runs->results != results || runs->output != output ||
        runs->summary.result_revision != umi_test_platform_result_registry_revision(results) ||
        runs->summary.output_revision != umi_test_platform_output_registry_revision(output))
        return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}

UmiStatus UmiTestEvidenceRunsCreate(const UmiTestPlatformResultRegistry *results,
    const UmiTestPlatformOutputRegistry *output, const char *item_id,
    UmiTestEvidenceRuns **out_runs)
{
    if (out_runs == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_runs = NULL;
    if (results == NULL || output == NULL || !RunIdValid(item_id, 0))
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t nr = umi_test_platform_result_registry_count(results);
    size_t no = umi_test_platform_output_registry_count(output);
    if (nr > UMI_TEST_PLATFORM_RESULT_CAPACITY || no > UMI_TEST_PLATFORM_OUTPUT_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiTestEvidenceRuns *runs = calloc(1U, sizeof *runs);
    if (runs == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    /* Allocate compact session keys, never an array of large output payloads.
     * Registry capacity checks bound the addition and multiplication here. */
    runs->rows = calloc(nr + no != 0U ? nr + no : 1U, sizeof *runs->rows);
    if (runs->rows == NULL) { free(runs); return UMI_STATUS_OUT_OF_MEMORY; }
    runs->results = results;
    runs->output = output;
    strcpy(runs->summary.item_id, item_id);
    runs->summary.result_revision = umi_test_platform_result_registry_revision(results);
    runs->summary.output_revision = umi_test_platform_output_registry_revision(output);
    UmiStatus status = UMI_STATUS_OK;
    size_t keys = 0U;
    for (size_t i = 0U; i < nr; ++i) {
        UmiTestPlatformResultSnapshot row;
        status = umi_test_platform_result_registry_at(results, i, &row);
        if (status != UMI_STATUS_OK) goto failure;
        if (strcmp(row.item_id, item_id) != 0) continue;
        ++runs->summary.matching_results;
        if (row.session_id[0] == '\0') { ++runs->summary.unspecified_results; continue; }
        strcpy(runs->rows[keys].session_id, row.session_id);
        runs->rows[keys++].results = 1U;
    }
    for (size_t i = 0U; i < no; ++i) {
        UmiTestPlatformOutputSnapshot row;
        status = umi_test_platform_output_registry_at(output, i, &row);
        if (status != UMI_STATUS_OK) goto failure;
        if (strcmp(row.item_id, item_id) != 0) continue;
        ++runs->summary.matching_output;
        if (row.session_id[0] == '\0') { ++runs->summary.unspecified_output; continue; }
        strcpy(runs->rows[keys].session_id, row.session_id);
        runs->rows[keys++].output = 1U;
    }
    qsort(runs->rows, keys, sizeof *runs->rows, RunCompare);
    /* Compact equal IDs only after sorting. Record timestamps and result
     * sequence values have different meanings and cannot order whole runs. */
    for (size_t i = 0U; i < keys; ++i) {
        size_t n = runs->summary.runs;
        if (n != 0U && strcmp(runs->rows[n - 1U].session_id, runs->rows[i].session_id) == 0) {
            runs->rows[n - 1U].results += runs->rows[i].results;
            runs->rows[n - 1U].output += runs->rows[i].output;
        } else {
            runs->rows[n] = runs->rows[i];
            ++runs->summary.runs;
        }
    }
    status = UmiTestEvidenceRunsValidate(runs, results, output);
    if (status != UMI_STATUS_OK) goto failure;
    *out_runs = runs;
    return UMI_STATUS_OK;
failure:
    UmiTestEvidenceRunsDestroy(runs);
    return status;
}

void UmiTestEvidenceRunsDestroy(UmiTestEvidenceRuns *runs)
{
    if (runs != NULL) { free(runs->rows); free(runs); }
}

UmiStatus UmiTestEvidenceRunsRead(const UmiTestEvidenceRuns *runs,
    UmiTestEvidenceRunsSummary *out_summary)
{
    if (runs == NULL || out_summary == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_summary = runs->summary;
    return UMI_STATUS_OK;
}

UmiStatus UmiTestEvidenceRunsAt(const UmiTestEvidenceRuns *runs, size_t index,
    UmiTestEvidenceRun *out_run)
{
    if (runs == NULL || out_run == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= runs->summary.runs) return UMI_STATUS_NOT_FOUND;
    *out_run = runs->rows[index];
    return UMI_STATUS_OK;
}

UmiStatus UmiTestEvidenceRunsCapture(const UmiTestEvidenceRuns *runs,
    const UmiTestPlatformResultRegistry *results,
    const UmiTestPlatformOutputRegistry *output, const char *session_id,
    UmiTestEvidence **out_evidence)
{
    if (out_evidence == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_evidence = NULL;
    if (session_id != NULL && !RunIdValid(session_id, 1))
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiTestEvidenceRunsValidate(runs, results, output);
    if (status != UMI_STATUS_OK) return status;
    if (session_id != NULL && session_id[0] != '\0') {
        UmiTestEvidenceRun key = {0};
        strcpy(key.session_id, session_id);
        if (bsearch(&key, runs->rows, runs->summary.runs,
                sizeof *runs->rows, RunCompare) == NULL) return UMI_STATUS_NOT_FOUND;
    }
    return UmiTestEvidenceCreate(results, output, runs->summary.item_id, session_id, out_evidence);
}
