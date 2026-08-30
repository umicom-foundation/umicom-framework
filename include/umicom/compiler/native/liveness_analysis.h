/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/liveness_analysis.h
 *
 * PURPOSE:
 *   Summarize per-block live-in value pressure for later register allocation work.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_NATIVE_LIVENESS_ANALYSIS_H
#define UMICOM_COMPILER_NATIVE_LIVENESS_ANALYSIS_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/compiler/native/ir_function.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiNativeBlockLiveness { uint32_t block_id; size_t live_in_count; size_t defined_count; size_t used_count; } UmiNativeBlockLiveness;
typedef struct UmiNativeLivenessAnalysis { UmiNativeBlockLiveness blocks[UMI_NC_MAX_IR_BLOCKS]; size_t block_count; size_t peak_live_in; } UmiNativeLivenessAnalysis;
UmiStatus umi_nc_liveness_analyze(const UmiNativeIrFunction *function,UmiNativeLivenessAnalysis *out_analysis);
#ifdef __cplusplus
}
#endif
#endif
