/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/counterparty_exposure.h
 *
 * PURPOSE:
 *   Represent counterparty current and potential future exposure.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_COUNTERPARTY_EXPOSURE_H
#define UMICOM_FINANCE_TREASURY_COUNTERPARTY_EXPOSURE_H
#include "umicom/finance/treasury/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the treasury counterparty exposure data shared with callers of this public
 * contract.
 */
typedef struct UmiTreasuryCounterpartyExposure {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t current_minor;
    int64_t potential_future_minor;
} UmiTreasuryCounterpartyExposure;
/**
 * Initialise treasury counterparty exposure from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_treasury_counterparty_exposure_init(UmiTreasuryCounterpartyExposure *value,
    const char *id,
    int64_t current_minor,
    int64_t potential_future_minor);
/**
 * Check that treasury counterparty exposure satisfies its contract before another service
 * relies on it.
 */
bool umi_treasury_counterparty_exposure_valid(const UmiTreasuryCounterpartyExposure *value);
/**
 * Provide the treasury counterparty exposure total minor operation used by this module and
 * its client applications.
 */
int64_t umi_treasury_counterparty_exposure_total_minor(const UmiTreasuryCounterpartyExposure *value);
#ifdef __cplusplus
}
#endif
#endif
