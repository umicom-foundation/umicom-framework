/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/limit_utilization.h
 *
 * PURPOSE:
 *   Calculate risk-limit utilisation using basis points.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_LIMIT_UTILIZATION_H
#define UMICOM_FINANCE_TREASURY_LIMIT_UTILIZATION_H
#include "umicom/finance/treasury/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTreasuryLimitUtilization {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t used_minor;
    int64_t limit_minor;
} UmiTreasuryLimitUtilization;
UmiStatus umi_treasury_limit_utilization_init(UmiTreasuryLimitUtilization *value,
    const char *id,
    int64_t used_minor,
    int64_t limit_minor);
bool umi_treasury_limit_utilization_valid(const UmiTreasuryLimitUtilization *value);
uint32_t umi_treasury_limit_utilization_utilization_bps(const UmiTreasuryLimitUtilization *value);
#ifdef __cplusplus
}
#endif
#endif
