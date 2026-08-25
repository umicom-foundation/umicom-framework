/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/compilation_pipeline.h
 *
 * PURPOSE:
 *   Track deterministic compiler stages, status and readiness from source through link completion.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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
typedef enum UmiNativeStageState { UMI_NC_STAGE_PENDING=0, UMI_NC_STAGE_RUNNING=1, UMI_NC_STAGE_PASSED=2, UMI_NC_STAGE_FAILED=3, UMI_NC_STAGE_SKIPPED=4 } UmiNativeStageState;
typedef struct UmiNativeStageRecord { UmiNativeCompileStage stage; UmiNativeStageState state; UmiStatus status; } UmiNativeStageRecord;
typedef struct UmiNativeCompilationPipeline { UmiNativeStageRecord records[12U]; size_t count; bool link_required; } UmiNativeCompilationPipeline;
void umi_nc_compilation_pipeline_init(UmiNativeCompilationPipeline *pipeline,bool link_required);
UmiStatus umi_nc_compilation_pipeline_mark(UmiNativeCompilationPipeline *pipeline,UmiNativeCompileStage stage,UmiNativeStageState state,UmiStatus status);
const UmiNativeStageRecord *umi_nc_compilation_pipeline_next(const UmiNativeCompilationPipeline *pipeline);
bool umi_nc_compilation_pipeline_ready(const UmiNativeCompilationPipeline *pipeline);
#ifdef __cplusplus
}
#endif
#endif
