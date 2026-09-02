/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/diagnostics/pipeline.h
 *
 * PURPOSE:
 *   Publish the public pipeline contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DIAGNOSTICS_PIPELINE_H
#define UMICOM_DIAGNOSTICS_PIPELINE_H

#include "umicom/diagnostics/event.h"
#include "umicom/diagnostics/model.h"
#include "umicom/diagnostics/output_buffer.h"
#include "umicom/diagnostics/parser.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the diagnostic pipeline data shared with callers of this public contract.
 */
typedef struct UmiDiagnosticPipeline UmiDiagnosticPipeline;

/**
 * Represent the diagnostic pipeline config data shared with callers of this public
 * contract.
 */
typedef struct UmiDiagnosticPipelineConfig {
    size_t diagnostic_capacity;
    size_t output_capacity;
    int install_builtin_parsers;
    int mirror_diagnostics_to_output;
} UmiDiagnosticPipelineConfig;

/**
 * Represent the diagnostic pipeline snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiDiagnosticPipelineSnapshot {
    UmiDiagnosticModelSummary diagnostics;
    UmiOutputBufferSummary output;
    size_t parser_count;
    size_t event_count;
    uint64_t lines_ingested;
    uint64_t parse_matches;
    uint64_t parse_misses;
    uint64_t revision;
} UmiDiagnosticPipelineSnapshot;

/**
 * Provide the diagnostic pipeline config default operation used by this module and its
 * client applications.
 */
UmiDiagnosticPipelineConfig umi_diagnostic_pipeline_config_default(void);
/**
 * Initialise diagnostic pipeline from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_diagnostic_pipeline_create(
    const UmiDiagnosticPipelineConfig *config,
    UmiDiagnosticPipeline **out_pipeline);
/**
 * Release or reset state held by diagnostic pipeline so the same storage can be reused
 * safely.
 */
void umi_diagnostic_pipeline_destroy(UmiDiagnosticPipeline *pipeline);
/**
 * Release or reset state held by diagnostic pipeline so the same storage can be reused
 * safely.
 */
void umi_diagnostic_pipeline_clear(UmiDiagnosticPipeline *pipeline);
/**
 * Provide the diagnostic pipeline ingest output operation used by this module and its
 * client applications.
 */
UmiStatus umi_diagnostic_pipeline_ingest_output(
    UmiDiagnosticPipeline *pipeline,
    const UmiOutputRecord *output);
/**
 * Provide the diagnostic pipeline ingest line operation used by this module and its client
 * applications.
 */
UmiStatus umi_diagnostic_pipeline_ingest_line(
    UmiDiagnosticPipeline *pipeline,
    const char *channel_id,
    const char *channel_name,
    const char *source,
    UmiOutputStream stream,
    const char *line,
    uint64_t correlation_id);
/**
 * Provide the diagnostic pipeline ingest diagnostic operation used by this module and its
 * client applications.
 */
UmiStatus umi_diagnostic_pipeline_ingest_diagnostic(
    UmiDiagnosticPipeline *pipeline,
    const UmiDiagnosticSnapshot *diagnostic);
/**
 * Provide the diagnostic pipeline snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_diagnostic_pipeline_snapshot(
    const UmiDiagnosticPipeline *pipeline,
    UmiDiagnosticPipelineSnapshot *out_snapshot);
/**
 * Provide the diagnostic pipeline sink operation used by this module and its client
 * applications.
 */
void umi_diagnostic_pipeline_sink(const UmiDiagnostic *diagnostic,
                                  void *user_data);
/**
 * Provide the diagnostic pipeline model operation used by this module and its client
 * applications.
 */
UmiDiagnosticModel *umi_diagnostic_pipeline_model(UmiDiagnosticPipeline *pipeline);
/**
 * Provide the diagnostic pipeline output operation used by this module and its client
 * applications.
 */
UmiOutputBuffer *umi_diagnostic_pipeline_output(UmiDiagnosticPipeline *pipeline);
/**
 * Provide the diagnostic pipeline parsers operation used by this module and its client
 * applications.
 */
UmiDiagnosticParserRegistry *umi_diagnostic_pipeline_parsers(
    UmiDiagnosticPipeline *pipeline);
/**
 * Provide the diagnostic pipeline events operation used by this module and its client
 * applications.
 */
UmiDiagnosticEventStream *umi_diagnostic_pipeline_events(
    UmiDiagnosticPipeline *pipeline);

#ifdef __cplusplus
}
#endif
#endif
