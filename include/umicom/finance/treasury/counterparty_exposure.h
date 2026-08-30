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
typedef struct UmiTreasuryCounterpartyExposure {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t current_minor;
    int64_t potential_future_minor;
} UmiTreasuryCounterpartyExposure;
UmiStatus umi_treasury_counterparty_exposure_init(UmiTreasuryCounterpartyExposure *value,
    const char *id,
    int64_t current_minor,
    int64_t potential_future_minor);
bool umi_treasury_counterparty_exposure_valid(const UmiTreasuryCounterpartyExposure *value);
int64_t umi_treasury_counterparty_exposure_total_minor(const UmiTreasuryCounterpartyExposure *value);
#ifdef __cplusplus
}
#endif
#endif
