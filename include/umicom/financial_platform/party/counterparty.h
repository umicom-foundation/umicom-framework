/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/financial_platform/party/counterparty.h
 *
 * PURPOSE:
 *   Evaluate Counterparty Readiness through the shared financial platform control plane.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCIAL_PLATFORM_PARTY_COUNTERPARTY_H
#define UMICOM_FINANCIAL_PLATFORM_PARTY_COUNTERPARTY_H

#include "umicom/financial_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_financial_platform_evaluate_party_counterparty(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
