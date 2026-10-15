/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/financial_platform/ledger/journal_validation.h
 *
 * PURPOSE:
 *   Evaluate Journal Validation through the shared financial platform control plane.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCIAL_PLATFORM_LEDGER_JOURNAL_VALIDATION_H
#define UMICOM_FINANCIAL_PLATFORM_LEDGER_JOURNAL_VALIDATION_H

#include <stddef.h>
#include "umicom/financial_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_financial_platform_evaluate_ledger_journal_validation(
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
UmiStatus umi_financial_platform_evaluate_ledger_journal_validation_batch(const UmiFinancialPlatformInput *inputs,
    size_t count, UmiFinancialPlatformSnapshot *outputs, size_t capacity);

#ifdef __cplusplus
}
#endif
#endif
