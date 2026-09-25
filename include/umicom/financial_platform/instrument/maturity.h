/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/financial_platform/instrument/maturity.h
 *
 * PURPOSE:
 *   Evaluate Instrument Maturity through the shared financial platform control plane.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCIAL_PLATFORM_INSTRUMENT_MATURITY_H
#define UMICOM_FINANCIAL_PLATFORM_INSTRUMENT_MATURITY_H

#include "umicom/financial_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_financial_platform_evaluate_instrument_maturity(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
