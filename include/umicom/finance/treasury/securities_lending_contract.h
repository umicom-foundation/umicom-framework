/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/securities_lending_contract.h
 *
 * PURPOSE:
 *   Model securities lending quantity, collateral value and fee rate.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_SECURITIES_LENDING_CONTRACT_H
#define UMICOM_FINANCE_TREASURY_SECURITIES_LENDING_CONTRACT_H
#include "umicom/finance/treasury/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTreasurySecuritiesLendingContract {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t quantity;
    int64_t collateral_minor;
    uint32_t fee_bps;
} UmiTreasurySecuritiesLendingContract;
UmiStatus umi_treasury_securities_lending_contract_init(UmiTreasurySecuritiesLendingContract *value,
    const char *id,
    int64_t quantity,
    int64_t collateral_minor,
    uint32_t fee_bps);
bool umi_treasury_securities_lending_contract_valid(const UmiTreasurySecuritiesLendingContract *value);
int64_t umi_treasury_securities_lending_contract_annual_fee_minor(const UmiTreasurySecuritiesLendingContract *value);
#ifdef __cplusplus
}
#endif
#endif
