/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/confirmation.h
 *
 * PURPOSE:
 *   Represent trade confirmation terms and confirmation state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_CONFIRMATION_H
#define UMICOM_FINANCE_TREASURY_CONFIRMATION_H
#include "umicom/finance/treasury/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTreasuryConfirmation {
    char id[UMI_TREASURY_ID_CAPACITY];
    char trade_id[UMI_TREASURY_ID_CAPACITY];
    bool sent;
    bool acknowledged;
} UmiTreasuryConfirmation;
UmiStatus umi_treasury_confirmation_init(UmiTreasuryConfirmation *value,
    const char *id,
    const char *trade_id,
    bool sent,
    bool acknowledged);
bool umi_treasury_confirmation_valid(const UmiTreasuryConfirmation *value);
bool umi_treasury_confirmation_complete(const UmiTreasuryConfirmation *value);
#ifdef __cplusplus
}
#endif
#endif
