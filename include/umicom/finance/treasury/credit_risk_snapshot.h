/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/credit_risk_snapshot.h
 *
 * PURPOSE:
 *   Capture aggregate credit exposure and expected loss.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_CREDIT_RISK_SNAPSHOT_H
#define UMICOM_FINANCE_TREASURY_CREDIT_RISK_SNAPSHOT_H
#include "umicom/finance/treasury/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTreasuryCreditRiskSnapshot {
    char id[UMI_TREASURY_ID_CAPACITY];
    UmiTreasuryRiskClass risk_class;
    int64_t primary_minor;
    int64_t secondary_minor;
} UmiTreasuryCreditRiskSnapshot;
UmiStatus umi_treasury_credit_risk_snapshot_init(UmiTreasuryCreditRiskSnapshot *value,
    const char *id,
    int64_t primary_minor,
    int64_t secondary_minor);
bool umi_treasury_credit_risk_snapshot_valid(const UmiTreasuryCreditRiskSnapshot *value);
int64_t umi_treasury_credit_risk_snapshot_combined_absolute_minor(const UmiTreasuryCreditRiskSnapshot *value);
#ifdef __cplusplus
}
#endif
#endif
