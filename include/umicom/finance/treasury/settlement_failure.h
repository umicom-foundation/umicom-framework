/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/settlement_failure.h
 *
 * PURPOSE:
 *   Record failed settlement exposure, age and retry eligibility.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_SETTLEMENT_FAILURE_H
#define UMICOM_FINANCE_TREASURY_SETTLEMENT_FAILURE_H
#include "umicom/finance/treasury/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTreasurySettlementFailure {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t exposure_minor;
    uint32_t age_days;
    uint32_t retry_count;
} UmiTreasurySettlementFailure;
UmiStatus umi_treasury_settlement_failure_init(UmiTreasurySettlementFailure *value,
    const char *id,
    int64_t exposure_minor,
    uint32_t age_days,
    uint32_t retry_count);
bool umi_treasury_settlement_failure_valid(const UmiTreasurySettlementFailure *value);
bool umi_treasury_settlement_failure_aged(const UmiTreasurySettlementFailure *value);
#ifdef __cplusplus
}
#endif
#endif
