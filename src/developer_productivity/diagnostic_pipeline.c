/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_productivity/diagnostic_pipeline.c
 *
 * PURPOSE:
 *   Implement normalized build/test diagnostic ingestion.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_productivity/diagnostic_pipeline.h"

#include <stdlib.h>
#include <string.h>

/*
 * Initialise developer diagnostic pipeline from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_developer_diagnostic_pipeline_create(
    UmiDeveloperDiagnosticPipeline **out_pipeline)
{
    UmiDeveloperDiagnosticPipeline *pipeline;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_pipeline == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_pipeline = NULL;

    pipeline = (UmiDeveloperDiagnosticPipeline *)calloc(
        1U, sizeof(*pipeline));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (pipeline == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    status = umi_developer_diagnostic_registry_create(&pipeline->parsers);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_developer_problem_store_create(&pipeline->problems);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_developer_builtin_diagnostics_register(
            pipeline->parsers);
    }

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        umi_developer_diagnostic_pipeline_destroy(pipeline);
        return status;
    }

    pipeline->revision = 1U;
    *out_pipeline = pipeline;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by developer diagnostic pipeline so the same storage can be
 * reused safely.
 */
void umi_developer_diagnostic_pipeline_destroy(
    UmiDeveloperDiagnosticPipeline *pipeline)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (pipeline == NULL) return;

    umi_developer_problem_store_destroy(pipeline->problems);
    umi_developer_diagnostic_registry_destroy(pipeline->parsers);
    free(pipeline);
}

/*
 * Provide the developer diagnostic pipeline ingest line operation used by this module and
 * its client applications.
 */
UmiStatus umi_developer_diagnostic_pipeline_ingest_line(
    UmiDeveloperDiagnosticPipeline *pipeline,
    const char *line,
    int *out_matched)
{
    UmiDeveloperProblem problem;
    int matched = 0;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (pipeline == NULL || line == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_diagnostic_registry_parse(
        pipeline->parsers,
        line,
        &problem,
        &matched);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    pipeline->lines_seen += 1U;

    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (matched) {
        status = umi_developer_problem_store_add(
            pipeline->problems,
            &problem);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;

        pipeline->lines_matched += 1U;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        pipeline->lines_unmatched += 1U;
    }

    pipeline->revision += 1U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_matched != NULL) *out_matched = matched;
    return UMI_STATUS_OK;
}

/*
 * Provide the developer diagnostic pipeline ingest text operation used by this module and
 * its client applications.
 */
UmiStatus umi_developer_diagnostic_pipeline_ingest_text(
    UmiDeveloperDiagnosticPipeline *pipeline,
    const char *text,
    size_t *out_problems_added)
{
    const char *cursor;
    size_t before;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (pipeline == NULL || text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    before = umi_developer_problem_store_count(pipeline->problems);
    cursor = text;

    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*cursor != '\0') {
        const char *end = strchr(cursor, '\n');
        size_t length = end != NULL
            ? (size_t)(end - cursor)
            : strlen(cursor);
        char line[UMI_DEVELOPER_PRODUCTIVITY_TEXT_CAPACITY];
        UmiStatus status;

        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (length >= sizeof(line)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }

        (void)memcpy(line, cursor, length);
        line[length] = '\0';

        status = umi_developer_diagnostic_pipeline_ingest_line(
            pipeline, line, NULL);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;

        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (end == NULL) break;
        cursor = end + 1;
    }

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_problems_added != NULL) {
        *out_problems_added =
            umi_developer_problem_store_count(pipeline->problems) - before;
    }

    return UMI_STATUS_OK;
}

/*
 * Provide the developer diagnostic pipeline problems operation used by this module and its
 * client applications.
 */
UmiDeveloperProblemStore *umi_developer_diagnostic_pipeline_problems(
    UmiDeveloperDiagnosticPipeline *pipeline)
{
    return pipeline != NULL ? pipeline->problems : NULL;
}
