/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_productivity/diagnostic_pipeline.h
 *
 * PURPOSE:
 *   Parse multiline compiler/build/test output into the normalized Problems
 *   store while retaining unmatched output for the Output panel.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PRODUCTIVITY_DIAGNOSTIC_PIPELINE_H
#define UMICOM_DEVELOPER_PRODUCTIVITY_DIAGNOSTIC_PIPELINE_H

#include "umicom/developer_productivity/builtin_diagnostics.h"
#include "umicom/developer_productivity/problem_store.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDeveloperDiagnosticPipeline {
    UmiDeveloperDiagnosticRegistry *parsers;
    UmiDeveloperProblemStore *problems;
    size_t lines_seen;
    size_t lines_matched;
    size_t lines_unmatched;
    uint64_t revision;
} UmiDeveloperDiagnosticPipeline;

UmiStatus umi_developer_diagnostic_pipeline_create(
    UmiDeveloperDiagnosticPipeline **out_pipeline);

void umi_developer_diagnostic_pipeline_destroy(
    UmiDeveloperDiagnosticPipeline *pipeline);

UmiStatus umi_developer_diagnostic_pipeline_ingest_line(
    UmiDeveloperDiagnosticPipeline *pipeline,
    const char *line,
    int *out_matched);

UmiStatus umi_developer_diagnostic_pipeline_ingest_text(
    UmiDeveloperDiagnosticPipeline *pipeline,
    const char *text,
    size_t *out_problems_added);

UmiDeveloperProblemStore *umi_developer_diagnostic_pipeline_problems(
    UmiDeveloperDiagnosticPipeline *pipeline);

#ifdef __cplusplus
}
#endif

#endif
