/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/use_analysis.h
 *
 * PURPOSE:
 *   Count IR value uses to support dead-code elimination and copy propagation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_NATIVE_USE_ANALYSIS_H
#define UMICOM_COMPILER_NATIVE_USE_ANALYSIS_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/compiler/native/ir_function.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_NC_MAX_ANALYZED_VALUES 4096U
typedef struct UmiNativeUseCount { uint32_t value_id; size_t uses; } UmiNativeUseCount;
typedef struct UmiNativeUseAnalysis { UmiNativeUseCount items[UMI_NC_MAX_ANALYZED_VALUES]; size_t count; } UmiNativeUseAnalysis;
UmiStatus umi_nc_use_analyze(const UmiNativeIrFunction *function,UmiNativeUseAnalysis *out_analysis);
size_t umi_nc_use_count(const UmiNativeUseAnalysis *analysis,uint32_t value_id);
#ifdef __cplusplus
}
#endif
#endif
