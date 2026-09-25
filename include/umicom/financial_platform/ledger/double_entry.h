/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/financial_platform/ledger/double_entry.h
 *
 * PURPOSE:
 *   Evaluate Double-entry Balance through the shared financial platform control plane.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCIAL_PLATFORM_LEDGER_DOUBLE_ENTRY_H
#define UMICOM_FINANCIAL_PLATFORM_LEDGER_DOUBLE_ENTRY_H

#include "umicom/financial_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_financial_platform_evaluate_ledger_double_entry(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
