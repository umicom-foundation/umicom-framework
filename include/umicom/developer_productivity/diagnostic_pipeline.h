/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_productivity/diagnostic_pipeline.h
 *
 * PURPOSE:
 *   Parse multiline compiler/build/test output into the normalized Problems
 *   store while retaining unmatched output for the Output panel.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PRODUCTIVITY_DIAGNOSTIC_PIPELINE_H
#define UMICOM_DEVELOPER_PRODUCTIVITY_DIAGNOSTIC_PIPELINE_H

#include "umicom/developer_productivity/builtin_diagnostics.h"
#include "umicom/developer_productivity/problem_store.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the developer diagnostic pipeline data shared with callers of this public
 * contract.
 */
typedef struct UmiDeveloperDiagnosticPipeline {
    UmiDeveloperDiagnosticRegistry *parsers;
    UmiDeveloperProblemStore *problems;
    size_t lines_seen;
    size_t lines_matched;
    size_t lines_unmatched;
    uint64_t revision;
} UmiDeveloperDiagnosticPipeline;

/**
 * Initialise developer diagnostic pipeline from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_developer_diagnostic_pipeline_create(
    UmiDeveloperDiagnosticPipeline **out_pipeline);

/**
 * Release or reset state held by developer diagnostic pipeline so the same storage can be
 * reused safely.
 */
void umi_developer_diagnostic_pipeline_destroy(
    UmiDeveloperDiagnosticPipeline *pipeline);

/**
 * Provide the developer diagnostic pipeline ingest line operation used by this module and
 * its client applications.
 */
UmiStatus umi_developer_diagnostic_pipeline_ingest_line(
    UmiDeveloperDiagnosticPipeline *pipeline,
    const char *line,
    int *out_matched);

/**
 * Provide the developer diagnostic pipeline ingest text operation used by this module and
 * its client applications.
 */
UmiStatus umi_developer_diagnostic_pipeline_ingest_text(
    UmiDeveloperDiagnosticPipeline *pipeline,
    const char *text,
    size_t *out_problems_added);

/**
 * Provide the developer diagnostic pipeline problems operation used by this module and its
 * client applications.
 */
UmiDeveloperProblemStore *umi_developer_diagnostic_pipeline_problems(
    UmiDeveloperDiagnosticPipeline *pipeline);

#ifdef __cplusplus
}
#endif

#endif
