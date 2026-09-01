/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/pipeline.h
 *
 * PURPOSE:
 *   Provide a deterministic dependency-aware pipeline for developer operations.
 *   The pipeline validates state transitions, prevents dependency cycles,
 *   exposes ready work in submission order, and supports bounded retry,
 *   cancellation and progress reporting.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The pipeline does not start threads or execute programs itself.  It owns
 * operation state and dependency rules.  An executor or application worker
 * asks for ready work, starts it, then records completion, failure or
 * cancellation.  This separation keeps the state machine deterministic and
 * makes the same pipeline usable by Studio, CI tools, web frontends and tests.
 */
#ifndef UMICOM_DEVELOPER_PIPELINE_H
#define UMICOM_DEVELOPER_PIPELINE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/developer/operation.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_PIPELINE_OPERATION_CAPACITY 256U
#define UMI_DEVELOPER_PIPELINE_DEPENDENCY_CAPACITY 1024U
#define UMI_DEVELOPER_PIPELINE_API_VERSION 1U

typedef struct UmiDeveloperPipelineSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t operation_count;
    size_t dependency_count;
    size_t queued_count;
    size_t ready_count;
    size_t running_count;
    size_t succeeded_count;
    size_t failed_count;
    size_t cancelled_count;
    size_t blocked_count;
    uint64_t revision;
} UmiDeveloperPipelineSnapshot;

typedef struct UmiDeveloperPipeline UmiDeveloperPipeline;

UmiStatus umi_developer_pipeline_create(UmiDeveloperPipeline **out_pipeline);
void umi_developer_pipeline_destroy(UmiDeveloperPipeline *pipeline);

UmiStatus umi_developer_pipeline_submit(
    UmiDeveloperPipeline *pipeline,
    const UmiDeveloperOperationSnapshot *operation);

UmiStatus umi_developer_pipeline_add_dependency(
    UmiDeveloperPipeline *pipeline,
    const char *operation_id,
    const char *depends_on_operation_id);

UmiStatus umi_developer_pipeline_find(
    const UmiDeveloperPipeline *pipeline,
    const char *operation_id,
    UmiDeveloperOperationSnapshot *out_operation);

UmiStatus umi_developer_pipeline_at(
    const UmiDeveloperPipeline *pipeline,
    size_t index,
    UmiDeveloperOperationSnapshot *out_operation);

size_t umi_developer_pipeline_count(const UmiDeveloperPipeline *pipeline);
uint64_t umi_developer_pipeline_revision(const UmiDeveloperPipeline *pipeline);

UmiStatus umi_developer_pipeline_next_ready(
    UmiDeveloperPipeline *pipeline,
    UmiDeveloperOperationSnapshot *out_operation);

UmiStatus umi_developer_pipeline_start(
    UmiDeveloperPipeline *pipeline,
    const char *operation_id);

UmiStatus umi_developer_pipeline_set_progress(
    UmiDeveloperPipeline *pipeline,
    const char *operation_id,
    uint32_t progress_basis_points,
    const char *summary);

UmiStatus umi_developer_pipeline_complete(
    UmiDeveloperPipeline *pipeline,
    const char *operation_id,
    int exit_code,
    const char *summary);

UmiStatus umi_developer_pipeline_fail(
    UmiDeveloperPipeline *pipeline,
    const char *operation_id,
    int exit_code,
    const char *summary);

UmiStatus umi_developer_pipeline_cancel(
    UmiDeveloperPipeline *pipeline,
    const char *operation_id,
    const char *summary);

UmiStatus umi_developer_pipeline_retry(
    UmiDeveloperPipeline *pipeline,
    const char *operation_id);

UmiStatus umi_developer_pipeline_refresh(UmiDeveloperPipeline *pipeline);

UmiStatus umi_developer_pipeline_snapshot(
    UmiDeveloperPipeline *pipeline,
    UmiDeveloperPipelineSnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif

#endif
