/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/native_web/patch_budget.h
 *
 * PURPOSE:
 *   Apply patch-count and payload-size budgets to protect interactive sessions from runaway renders.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_PATCH_BUDGET_H
#define UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_PATCH_BUDGET_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/frontend/native_web/patch_batch.h"
typedef struct UmiNativeWebPatchBudget { size_t max_patches; size_t max_estimated_bytes; } UmiNativeWebPatchBudget;
/* Estimate whether a patch batch is safe to deliver under configured limits. */
UmiStatus umi_native_web_patch_budget_check(const UmiNativeWebPatchBatch *batch, const UmiNativeWebPatchBudget *budget, size_t *out_estimated_bytes);

#ifdef __cplusplus
}
#endif
#endif
