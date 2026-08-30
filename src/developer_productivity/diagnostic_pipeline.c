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

UmiStatus umi_developer_diagnostic_pipeline_create(
    UmiDeveloperDiagnosticPipeline **out_pipeline)
{
    UmiDeveloperDiagnosticPipeline *pipeline;
    UmiStatus status;

    if (out_pipeline == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_pipeline = NULL;

    pipeline = (UmiDeveloperDiagnosticPipeline *)calloc(
        1U, sizeof(*pipeline));
    if (pipeline == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    status = umi_developer_diagnostic_registry_create(&pipeline->parsers);
    if (status == UMI_STATUS_OK) {
        status = umi_developer_problem_store_create(&pipeline->problems);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_developer_builtin_diagnostics_register(
            pipeline->parsers);
    }

    if (status != UMI_STATUS_OK) {
        umi_developer_diagnostic_pipeline_destroy(pipeline);
        return status;
    }

    pipeline->revision = 1U;
    *out_pipeline = pipeline;
    return UMI_STATUS_OK;
}

void umi_developer_diagnostic_pipeline_destroy(
    UmiDeveloperDiagnosticPipeline *pipeline)
{
    if (pipeline == NULL) return;

    umi_developer_problem_store_destroy(pipeline->problems);
    umi_developer_diagnostic_registry_destroy(pipeline->parsers);
    free(pipeline);
}

UmiStatus umi_developer_diagnostic_pipeline_ingest_line(
    UmiDeveloperDiagnosticPipeline *pipeline,
    const char *line,
    int *out_matched)
{
    UmiDeveloperProblem problem;
    int matched = 0;
    UmiStatus status;

    if (pipeline == NULL || line == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_diagnostic_registry_parse(
        pipeline->parsers,
        line,
        &problem,
        &matched);
    if (status != UMI_STATUS_OK) return status;

    pipeline->lines_seen += 1U;

    if (matched) {
        status = umi_developer_problem_store_add(
            pipeline->problems,
            &problem);
        if (status != UMI_STATUS_OK) return status;

        pipeline->lines_matched += 1U;
    } else {
        pipeline->lines_unmatched += 1U;
    }

    pipeline->revision += 1U;
    if (out_matched != NULL) *out_matched = matched;
    return UMI_STATUS_OK;
}

UmiStatus umi_developer_diagnostic_pipeline_ingest_text(
    UmiDeveloperDiagnosticPipeline *pipeline,
    const char *text,
    size_t *out_problems_added)
{
    const char *cursor;
    size_t before;

    if (pipeline == NULL || text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    before = umi_developer_problem_store_count(pipeline->problems);
    cursor = text;

    while (*cursor != '\0') {
        const char *end = strchr(cursor, '\n');
        size_t length = end != NULL
            ? (size_t)(end - cursor)
            : strlen(cursor);
        char line[UMI_DEVELOPER_PRODUCTIVITY_TEXT_CAPACITY];
        UmiStatus status;

        if (length >= sizeof(line)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }

        (void)memcpy(line, cursor, length);
        line[length] = '\0';

        status = umi_developer_diagnostic_pipeline_ingest_line(
            pipeline, line, NULL);
        if (status != UMI_STATUS_OK) return status;

        if (end == NULL) break;
        cursor = end + 1;
    }

    if (out_problems_added != NULL) {
        *out_problems_added =
            umi_developer_problem_store_count(pipeline->problems) - before;
    }

    return UMI_STATUS_OK;
}

UmiDeveloperProblemStore *umi_developer_diagnostic_pipeline_problems(
    UmiDeveloperDiagnosticPipeline *pipeline)
{
    return pipeline != NULL ? pipeline->problems : NULL;
}
