/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/securities_finance_exposure.h
 *
 * PURPOSE:
 *   Calculate secured and unsecured exposure for securities financing.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_SECURITIES_FINANCE_EXPOSURE_H
#define UMICOM_FINANCE_TREASURY_SECURITIES_FINANCE_EXPOSURE_H
#include "umicom/finance/treasury/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the treasury securities finance exposure data shared with callers of this
 * public contract.
 */
typedef struct UmiTreasurySecuritiesFinanceExposure {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t exposure_minor;
    int64_t collateral_minor;
} UmiTreasurySecuritiesFinanceExposure;
/**
 * Initialise treasury securities finance exposure from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_treasury_securities_finance_exposure_init(UmiTreasurySecuritiesFinanceExposure *value,
    const char *id,
    int64_t exposure_minor,
    int64_t collateral_minor);
/**
 * Check that treasury securities finance exposure satisfies its contract before another
 * service relies on it.
 */
bool umi_treasury_securities_finance_exposure_valid(const UmiTreasurySecuritiesFinanceExposure *value);
/**
 * Provide the treasury securities finance exposure unsecured minor operation used by this
 * module and its client applications.
 */
int64_t umi_treasury_securities_finance_exposure_unsecured_minor(const UmiTreasurySecuritiesFinanceExposure *value);
#ifdef __cplusplus
}
#endif
#endif
