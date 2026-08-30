/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/settlement_netting.h
 *
 * PURPOSE:
 *   Calculate bilateral settlement netting across gross pay and receive legs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_SETTLEMENT_NETTING_H
#define UMICOM_FINANCE_TREASURY_SETTLEMENT_NETTING_H
#include "umicom/finance/treasury/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTreasurySettlementNetting {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t pay_minor;
    int64_t receive_minor;
} UmiTreasurySettlementNetting;
UmiStatus umi_treasury_settlement_netting_init(UmiTreasurySettlementNetting *value,
    const char *id,
    int64_t pay_minor,
    int64_t receive_minor);
bool umi_treasury_settlement_netting_valid(const UmiTreasurySettlementNetting *value);
int64_t umi_treasury_settlement_netting_net_minor(const UmiTreasurySettlementNetting *value);
#ifdef __cplusplus
}
#endif
#endif
