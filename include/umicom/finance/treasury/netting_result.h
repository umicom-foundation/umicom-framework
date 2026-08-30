/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/netting_result.h
 *
 * PURPOSE:
 *   Record gross and net exposure reduction from netting.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_NETTING_RESULT_H
#define UMICOM_FINANCE_TREASURY_NETTING_RESULT_H
#include "umicom/finance/treasury/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTreasuryNettingResult {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t gross_minor;
    int64_t net_minor;
} UmiTreasuryNettingResult;
UmiStatus umi_treasury_netting_result_init(UmiTreasuryNettingResult *value,
    const char *id,
    int64_t gross_minor,
    int64_t net_minor);
bool umi_treasury_netting_result_valid(const UmiTreasuryNettingResult *value);
int64_t umi_treasury_netting_result_reduction_minor(const UmiTreasuryNettingResult *value);
#ifdef __cplusplus
}
#endif
#endif
