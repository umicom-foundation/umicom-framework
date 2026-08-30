/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/settlement_obligation.h
 *
 * PURPOSE:
 *   Represent delivery-versus-payment settlement obligations and state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_SETTLEMENT_OBLIGATION_H
#define UMICOM_FINANCE_TREASURY_SETTLEMENT_OBLIGATION_H
#include "umicom/finance/treasury/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTreasurySettlementObligation {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t cash_minor;
    int64_t security_quantity;
    UmiTreasurySettlementState state;
} UmiTreasurySettlementObligation;
UmiStatus umi_treasury_settlement_obligation_init(UmiTreasurySettlementObligation *value,
    const char *id,
    int64_t cash_minor,
    int64_t security_quantity,
    UmiTreasurySettlementState state);
bool umi_treasury_settlement_obligation_valid(const UmiTreasurySettlementObligation *value);
bool umi_treasury_settlement_obligation_complete(const UmiTreasurySettlementObligation *value);
#ifdef __cplusplus
}
#endif
#endif
