/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/cash_position.h
 *
 * PURPOSE:
 *   Represent an account cash position with settled and projected balances.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_CASH_POSITION_H
#define UMICOM_FINANCE_TREASURY_CASH_POSITION_H
#include "umicom/finance/treasury/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTreasuryCashPosition {
    char id[UMI_TREASURY_ID_CAPACITY];
    UmiCurrency currency;
    int64_t settled_minor;
    int64_t projected_minor;
} UmiTreasuryCashPosition;
UmiStatus umi_treasury_cash_position_init(UmiTreasuryCashPosition *value,
    const char *id,
    const char *currency_code,
    int64_t settled_minor,
    int64_t projected_minor);
bool umi_treasury_cash_position_valid(const UmiTreasuryCashPosition *value);
int64_t umi_treasury_cash_position_delta_minor(const UmiTreasuryCashPosition *value);
#ifdef __cplusplus
}
#endif
#endif
