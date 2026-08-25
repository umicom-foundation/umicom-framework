/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/settlement_window.h
 *
 * PURPOSE:
 *   Define operational settlement opening and cut-off timestamps.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_SETTLEMENT_WINDOW_H
#define UMICOM_FINANCE_TREASURY_SETTLEMENT_WINDOW_H
#include "umicom/finance/treasury/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTreasurySettlementWindow {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t opens_epoch_millis;
    int64_t closes_epoch_millis;
} UmiTreasurySettlementWindow;
UmiStatus umi_treasury_settlement_window_init(UmiTreasurySettlementWindow *value,
    const char *id,
    int64_t opens_epoch_millis,
    int64_t closes_epoch_millis);
bool umi_treasury_settlement_window_valid(const UmiTreasurySettlementWindow *value);
int64_t umi_treasury_settlement_window_duration_millis(const UmiTreasurySettlementWindow *value);
#ifdef __cplusplus
}
#endif
#endif
