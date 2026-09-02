/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/compilation_pipeline.h
 *
 * PURPOSE:
 *   Track deterministic compiler stages, status and readiness from source through link completion.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_NATIVE_COMPILATION_PIPELINE_H
#define UMICOM_COMPILER_NATIVE_COMPILATION_PIPELINE_H
#include <stdbool.h>
#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/compiler/native/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * List the named native stage state values accepted by this public contract.
 */
typedef enum UmiNativeStageState { UMI_NC_STAGE_PENDING=0, UMI_NC_STAGE_RUNNING=1, UMI_NC_STAGE_PASSED=2, UMI_NC_STAGE_FAILED=3, UMI_NC_STAGE_SKIPPED=4 } UmiNativeStageState;
/**
 * Represent the native stage record data shared with callers of this public contract.
 */
typedef struct UmiNativeStageRecord { UmiNativeCompileStage stage; UmiNativeStageState state; UmiStatus status; } UmiNativeStageRecord;
/**
 * Represent the native compilation pipeline data shared with callers of this public
 * contract.
 */
typedef struct UmiNativeCompilationPipeline { UmiNativeStageRecord records[12U]; size_t count; bool link_required; } UmiNativeCompilationPipeline;
/**
 * Initialise nc compilation pipeline from caller-provided values so later operations
 * receive a known state.
 */
void umi_nc_compilation_pipeline_init(UmiNativeCompilationPipeline *pipeline,bool link_required);
/**
 * Provide the nc compilation pipeline mark operation used by this module and its client
 * applications.
 */
UmiStatus umi_nc_compilation_pipeline_mark(UmiNativeCompilationPipeline *pipeline,UmiNativeCompileStage stage,UmiNativeStageState state,UmiStatus status);
/**
 * Provide the nc compilation pipeline next operation used by this module and its client
 * applications.
 */
const UmiNativeStageRecord *umi_nc_compilation_pipeline_next(const UmiNativeCompilationPipeline *pipeline);
/**
 * Provide the nc compilation pipeline ready operation used by this module and its client
 * applications.
 */
bool umi_nc_compilation_pipeline_ready(const UmiNativeCompilationPipeline *pipeline);
#ifdef __cplusplus
}
#endif
#endif
