/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/diagnostics/pipeline.c
 *
 * PURPOSE:
 *   Implement the pipeline behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/diagnostics/pipeline.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "diagnostic_lock.h"
#include "umicom/diagnostics/compiler_parser.h"
#include "umicom/diagnostics/runtime_parser.h"

struct UmiDiagnosticPipeline {
    UmiDiagnosticModel *model;
    UmiOutputBuffer *output;
    UmiDiagnosticParserRegistry *parsers;
    UmiDiagnosticEventStream *events;
    int mirror_diagnostics_to_output;
    uint64_t lines_ingested;
    uint64_t parse_matches;
    uint64_t parse_misses;
    uint64_t revision;
    UmiDiagnosticLock lock;
};

/*
 * Provide the pipeline timestamp ns operation used by this module and its client
 * applications.
 */
static uint64_t pipeline_timestamp_ns(void)
{
    struct timespec timestamp;
    /* Apply this branch only when its contract condition is satisfied. */
    if (timespec_get(&timestamp, TIME_UTC) != TIME_UTC) return 0U;
    return (uint64_t)timestamp.tv_sec * UINT64_C(1000000000) + (uint64_t)timestamp.tv_nsec;
}

/* Provide the pipeline hash operation used by this module and its client applications. */
static uint64_t pipeline_hash(const char *text)
{
    uint64_t hash = UINT64_C(1469598103934665603);
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (text != NULL && *text != '\0') {
        hash ^= (uint64_t)(unsigned char)*text++;
        hash *= UINT64_C(1099511628211);
    }
    return hash;
}

/* Provide the emit event operation used by this module and its client applications. */
static void emit_event(UmiDiagnosticPipeline *pipeline,
                       UmiDiagnosticEventKind kind,
                       const char *diagnostic_id,
                       const char *channel_id,
                       const char *detail,
                       uint64_t correlation_id)
{
    UmiDiagnosticEvent event;
    (void)memset(&event, 0, sizeof(event));
    event.kind = kind;
    event.timestamp_ns = pipeline_timestamp_ns();
    event.correlation_id = correlation_id;
    (void)snprintf(event.diagnostic_id, sizeof(event.diagnostic_id), "%s",
                   diagnostic_id != NULL ? diagnostic_id : "");
    (void)snprintf(event.channel_id, sizeof(event.channel_id), "%s",
                   channel_id != NULL ? channel_id : "");
    (void)snprintf(event.detail, sizeof(event.detail), "%s", detail != NULL ? detail : "");
    (void)umi_diagnostic_event_stream_append(pipeline->events, &event);
}

/*
 * Provide the diagnostic pipeline config default operation used by this module and its
 * client applications.
 */
UmiDiagnosticPipelineConfig umi_diagnostic_pipeline_config_default(void)
{
    UmiDiagnosticPipelineConfig config;
    config.diagnostic_capacity = UMI_DIAGNOSTIC_MODEL_DEFAULT_CAPACITY;
    config.output_capacity = UMI_OUTPUT_BUFFER_DEFAULT_CAPACITY;
    config.install_builtin_parsers = 1;
    config.mirror_diagnostics_to_output = 1;
    return config;
}

/*
 * Initialise diagnostic pipeline from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_diagnostic_pipeline_create(const UmiDiagnosticPipelineConfig *config,
                                         UmiDiagnosticPipeline **out_pipeline)
{
    UmiDiagnosticPipelineConfig effective;
    UmiDiagnosticModelConfig model_config;
    UmiOutputBufferConfig output_config;
    UmiDiagnosticPipeline *pipeline;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_pipeline == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_pipeline = NULL;
    effective = config != NULL ? *config : umi_diagnostic_pipeline_config_default();
    pipeline = (UmiDiagnosticPipeline *)calloc(1U, sizeof(*pipeline));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (pipeline == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    umi_diagnostic_lock_init(&pipeline->lock);
    model_config.capacity = effective.diagnostic_capacity;
    output_config.capacity = effective.output_capacity;
    status = umi_diagnostic_model_create(&model_config, &pipeline->model);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_output_buffer_create(&output_config, &pipeline->output);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_diagnostic_parser_registry_create(&pipeline->parsers);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_diagnostic_event_stream_create(&pipeline->events);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        umi_diagnostic_pipeline_destroy(pipeline);
        return status;
    }
    pipeline->mirror_diagnostics_to_output = effective.mirror_diagnostics_to_output != 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (effective.install_builtin_parsers != 0) {
        UmiDiagnosticParser compiler = umi_compiler_diagnostic_parser();
        UmiDiagnosticParser runtime = umi_runtime_diagnostic_parser();
        status = umi_diagnostic_parser_registry_add(pipeline->parsers, &compiler);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) status = umi_diagnostic_parser_registry_add(pipeline->parsers, &runtime);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            umi_diagnostic_pipeline_destroy(pipeline);
            return status;
        }
    }
    *out_pipeline = pipeline;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by diagnostic pipeline so the same storage can be reused
 * safely.
 */
void umi_diagnostic_pipeline_destroy(UmiDiagnosticPipeline *pipeline)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (pipeline == NULL) return;
    umi_diagnostic_event_stream_destroy(pipeline->events);
    umi_diagnostic_parser_registry_destroy(pipeline->parsers);
    umi_output_buffer_destroy(pipeline->output);
    umi_diagnostic_model_destroy(pipeline->model);
    free(pipeline);
}

/*
 * Release or reset state held by diagnostic pipeline so the same storage can be reused
 * safely.
 */
void umi_diagnostic_pipeline_clear(UmiDiagnosticPipeline *pipeline)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (pipeline == NULL) return;
    umi_diagnostic_model_clear(pipeline->model);
    umi_output_buffer_clear(pipeline->output);
    umi_diagnostic_event_stream_clear(pipeline->events);
    umi_diagnostic_lock_acquire(&pipeline->lock);
    ++pipeline->revision;
    umi_diagnostic_lock_release(&pipeline->lock);
    emit_event(pipeline, UMI_DIAGNOSTIC_EVENT_CLEARED, NULL, NULL,
               "diagnostics and output cleared", 0U);
}

/*
 * Provide the diagnostic pipeline ingest diagnostic operation used by this module and its
 * client applications.
 */
UmiStatus umi_diagnostic_pipeline_ingest_diagnostic(UmiDiagnosticPipeline *pipeline,
                                                    const UmiDiagnosticSnapshot *diagnostic)
{
    UmiDiagnosticSnapshot existing;
    UmiStatus status;
    int is_update;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (pipeline == NULL || diagnostic == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    is_update = umi_diagnostic_model_find(pipeline->model, diagnostic->id, &existing) == UMI_STATUS_OK;
    status = umi_diagnostic_model_upsert(pipeline->model, diagnostic);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    umi_diagnostic_lock_acquire(&pipeline->lock);
    ++pipeline->revision;
    umi_diagnostic_lock_release(&pipeline->lock);
    emit_event(pipeline,
               diagnostic->resolved != 0 ? UMI_DIAGNOSTIC_EVENT_DIAGNOSTIC_RESOLVED :
               (is_update != 0 ? UMI_DIAGNOSTIC_EVENT_DIAGNOSTIC_UPDATED :
                                 UMI_DIAGNOSTIC_EVENT_DIAGNOSTIC_CREATED),
               diagnostic->id, NULL, diagnostic->message, diagnostic->correlation_id);
    /* Apply this branch only when its contract condition is satisfied. */
    if (pipeline->mirror_diagnostics_to_output != 0) {
        UmiOutputRecord record;
        UmiOutputStream stream = diagnostic->severity >= UMI_DIAGNOSTIC_ERROR ?
                                 UMI_OUTPUT_STREAM_ERROR : UMI_OUTPUT_STREAM_SYSTEM;
        status = umi_output_record_init(&record, "diagnostics", "Diagnostics",
                                        diagnostic->source, stream, diagnostic->message);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) {
            record.correlation_id = diagnostic->correlation_id;
            record.timestamp_ns = diagnostic->timestamp_ns;
            status = umi_output_buffer_append(pipeline->output, &record);
        }
    }
    return status;
}

/*
 * Provide the diagnostic pipeline ingest output operation used by this module and its
 * client applications.
 */
UmiStatus umi_diagnostic_pipeline_ingest_output(UmiDiagnosticPipeline *pipeline,
                                                const UmiOutputRecord *output)
{
    UmiDiagnosticSnapshot diagnostic;
    UmiStatus status;
    int matched = 0;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (pipeline == NULL || output == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_output_buffer_append(pipeline->output, output);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    emit_event(pipeline, UMI_DIAGNOSTIC_EVENT_OUTPUT_ACCEPTED, NULL,
               output->channel_id, output->text, output->correlation_id);
    status = umi_diagnostic_parser_registry_parse(pipeline->parsers, output,
                                                  &diagnostic, &matched);
    umi_diagnostic_lock_acquire(&pipeline->lock);
    ++pipeline->lines_ingested;
    ++pipeline->revision;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (matched != 0) ++pipeline->parse_matches;
    /* Use this fallback path when the earlier condition does not apply. */
    else ++pipeline->parse_misses;
    umi_diagnostic_lock_release(&pipeline->lock);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        emit_event(pipeline, UMI_DIAGNOSTIC_EVENT_DROPPED, NULL,
                   output->channel_id, "diagnostic parser failed", output->correlation_id);
        return status;
    }
    return matched != 0 ? umi_diagnostic_pipeline_ingest_diagnostic(pipeline, &diagnostic) :
                          UMI_STATUS_OK;
}

/*
 * Provide the diagnostic pipeline ingest line operation used by this module and its client
 * applications.
 */
UmiStatus umi_diagnostic_pipeline_ingest_line(UmiDiagnosticPipeline *pipeline,
                                              const char *channel_id,
                                              const char *channel_name,
                                              const char *source,
                                              UmiOutputStream stream,
                                              const char *line,
                                              uint64_t correlation_id)
{
    UmiOutputRecord record;
    UmiStatus status = umi_output_record_init(&record, channel_id, channel_name,
                                              source, stream, line);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    record.correlation_id = correlation_id;
    record.timestamp_ns = pipeline_timestamp_ns();
    return umi_diagnostic_pipeline_ingest_output(pipeline, &record);
}

/*
 * Provide the diagnostic pipeline snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_diagnostic_pipeline_snapshot(const UmiDiagnosticPipeline *pipeline,
                                           UmiDiagnosticPipelineSnapshot *out_snapshot)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (pipeline == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    status = umi_diagnostic_model_summary(pipeline->model, &out_snapshot->diagnostics);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_output_buffer_summary(pipeline->output, &out_snapshot->output);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    out_snapshot->parser_count = umi_diagnostic_parser_registry_count(pipeline->parsers);
    out_snapshot->event_count = umi_diagnostic_event_stream_count(pipeline->events);
    umi_diagnostic_lock_acquire(&pipeline->lock);
    out_snapshot->lines_ingested = pipeline->lines_ingested;
    out_snapshot->parse_matches = pipeline->parse_matches;
    out_snapshot->parse_misses = pipeline->parse_misses;
    out_snapshot->revision = pipeline->revision;
    umi_diagnostic_lock_release(&pipeline->lock);
    return UMI_STATUS_OK;
}

/*
 * Provide the diagnostic pipeline sink operation used by this module and its client
 * applications.
 */
void umi_diagnostic_pipeline_sink(const UmiDiagnostic *diagnostic, void *user_data)
{
    UmiDiagnosticPipeline *pipeline = (UmiDiagnosticPipeline *)user_data;
    UmiDiagnosticSnapshot snapshot;
    char identifier[UMI_DIAGNOSTIC_ID_CAPACITY];
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (pipeline == NULL || diagnostic == NULL) return;
    (void)snprintf(identifier, sizeof(identifier), "legacy-%016" PRIx64,
                   pipeline_hash(diagnostic->message) ^ diagnostic->correlation_id);
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (umi_diagnostic_snapshot_init(&snapshot, identifier, diagnostic->severity,
                                     UMI_DIAGNOSTIC_KIND_GENERAL,
                                     diagnostic->source, diagnostic->message) == UMI_STATUS_OK) {
        snapshot.correlation_id = diagnostic->correlation_id;
        snapshot.timestamp_ns = pipeline_timestamp_ns();
        (void)umi_diagnostic_pipeline_ingest_diagnostic(pipeline, &snapshot);
    }
}

/*
 * Provide the diagnostic pipeline model operation used by this module and its client
 * applications.
 */
UmiDiagnosticModel *umi_diagnostic_pipeline_model(UmiDiagnosticPipeline *pipeline)
{
    return pipeline != NULL ? pipeline->model : NULL;
}

/*
 * Provide the diagnostic pipeline output operation used by this module and its client
 * applications.
 */
UmiOutputBuffer *umi_diagnostic_pipeline_output(UmiDiagnosticPipeline *pipeline)
{
    return pipeline != NULL ? pipeline->output : NULL;
}

/*
 * Provide the diagnostic pipeline parsers operation used by this module and its client
 * applications.
 */
UmiDiagnosticParserRegistry *umi_diagnostic_pipeline_parsers(UmiDiagnosticPipeline *pipeline)
{
    return pipeline != NULL ? pipeline->parsers : NULL;
}

/*
 * Provide the diagnostic pipeline events operation used by this module and its client
 * applications.
 */
UmiDiagnosticEventStream *umi_diagnostic_pipeline_events(UmiDiagnosticPipeline *pipeline)
{
    return pipeline != NULL ? pipeline->events : NULL;
}
