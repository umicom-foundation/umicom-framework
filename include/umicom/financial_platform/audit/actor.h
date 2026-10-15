/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/financial_platform/audit/actor.h
 *
 * PURPOSE:
 *   Evaluate Financial Audit Actor through the shared financial platform control plane.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCIAL_PLATFORM_AUDIT_ACTOR_H
#define UMICOM_FINANCIAL_PLATFORM_AUDIT_ACTOR_H

#include <stddef.h>
#include "umicom/financial_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_financial_platform_evaluate_audit_actor(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot);


/** Evaluate a complete group using this policy without publishing partial rows.
 * Zero count is a no-op and permits NULL arrays. Nonempty groups require arrays
 * with count readable inputs and capacity writable outputs. At most 4096 rows
 * and 16 MiB of temporary output are accepted; allocation can fail.
 * Any failure preserves all outputs. Non-finite signals and overlong labels
 * are refused. Inputs may borrow output storage until publication. The caller
 * serializes access; this calculation does not submit orders or execute jobs.
 */
UmiStatus umi_financial_platform_evaluate_audit_actor_batch(const UmiFinancialPlatformInput *inputs,
    size_t count, UmiFinancialPlatformSnapshot *outputs, size_t capacity);

#ifdef __cplusplus
}
#endif
#endif
