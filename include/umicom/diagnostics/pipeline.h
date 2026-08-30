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

typedef struct UmiDiagnosticPipeline UmiDiagnosticPipeline;

typedef struct UmiDiagnosticPipelineConfig {
    size_t diagnostic_capacity;
    size_t output_capacity;
    int install_builtin_parsers;
    int mirror_diagnostics_to_output;
} UmiDiagnosticPipelineConfig;

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

UmiDiagnosticPipelineConfig umi_diagnostic_pipeline_config_default(void);
UmiStatus umi_diagnostic_pipeline_create(
    const UmiDiagnosticPipelineConfig *config,
    UmiDiagnosticPipeline **out_pipeline);
void umi_diagnostic_pipeline_destroy(UmiDiagnosticPipeline *pipeline);
void umi_diagnostic_pipeline_clear(UmiDiagnosticPipeline *pipeline);
UmiStatus umi_diagnostic_pipeline_ingest_output(
    UmiDiagnosticPipeline *pipeline,
    const UmiOutputRecord *output);
UmiStatus umi_diagnostic_pipeline_ingest_line(
    UmiDiagnosticPipeline *pipeline,
    const char *channel_id,
    const char *channel_name,
    const char *source,
    UmiOutputStream stream,
    const char *line,
    uint64_t correlation_id);
UmiStatus umi_diagnostic_pipeline_ingest_diagnostic(
    UmiDiagnosticPipeline *pipeline,
    const UmiDiagnosticSnapshot *diagnostic);
UmiStatus umi_diagnostic_pipeline_snapshot(
    const UmiDiagnosticPipeline *pipeline,
    UmiDiagnosticPipelineSnapshot *out_snapshot);
void umi_diagnostic_pipeline_sink(const UmiDiagnostic *diagnostic,
                                  void *user_data);
UmiDiagnosticModel *umi_diagnostic_pipeline_model(UmiDiagnosticPipeline *pipeline);
UmiOutputBuffer *umi_diagnostic_pipeline_output(UmiDiagnosticPipeline *pipeline);
UmiDiagnosticParserRegistry *umi_diagnostic_pipeline_parsers(
    UmiDiagnosticPipeline *pipeline);
UmiDiagnosticEventStream *umi_diagnostic_pipeline_events(
    UmiDiagnosticPipeline *pipeline);

#ifdef __cplusplus
}
#endif
#endif
