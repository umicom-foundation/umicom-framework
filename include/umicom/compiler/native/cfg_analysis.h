/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/cfg_analysis.h
 *
 * PURPOSE:
 *   Compute reachable basic blocks and edge counts from the native IR control-flow graph.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_NATIVE_CFG_ANALYSIS_H
#define UMICOM_COMPILER_NATIVE_CFG_ANALYSIS_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/compiler/native/ir_function.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the native cfg analysis data shared with callers of this public contract.
 */
typedef struct UmiNativeCfgAnalysis { uint32_t reachable_ids[UMI_NC_MAX_IR_BLOCKS]; size_t reachable_count; size_t edge_count; size_t unreachable_count; } UmiNativeCfgAnalysis;
/**
 * Provide the nc cfg analyze operation used by this module and its client applications.
 */
UmiStatus umi_nc_cfg_analyze(const UmiNativeIrFunction *function,UmiNativeCfgAnalysis *out_analysis);
/**
 * Provide the nc cfg is reachable operation used by this module and its client
 * applications.
 */
bool umi_nc_cfg_is_reachable(const UmiNativeCfgAnalysis *analysis,uint32_t block_id);
#ifdef __cplusplus
}
#endif
#endif
