/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/preprocess_plan.h
 *
 * PURPOSE:
 *   Scan preprocessor structure, count directives and validate conditional nesting before expansion.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_NATIVE_PREPROCESS_PLAN_H
#define UMICOM_COMPILER_NATIVE_PREPROCESS_PLAN_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/compiler/native/directive.h"
#include "umicom/compiler/native/source_buffer.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiNativePreprocessPlan { size_t directives; size_t defines; size_t includes; size_t conditionals; size_t max_conditional_depth; size_t errors; uint64_t source_fingerprint; } UmiNativePreprocessPlan;
UmiStatus umi_nc_preprocess_plan_scan(const UmiNativeSourceBuffer *source,UmiNativePreprocessPlan *out_plan);
bool umi_nc_preprocess_plan_ready(const UmiNativePreprocessPlan *plan);
#ifdef __cplusplus
}
#endif
#endif
