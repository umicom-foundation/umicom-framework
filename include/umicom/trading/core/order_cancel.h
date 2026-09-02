/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/order_cancel.h
 *
 * PURPOSE:
 *   Describe a cancellable order request with version control and reason code.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_ORDER_CANCEL_H
#define UMICOM_TRADING_CORE_ORDER_CANCEL_H
#include "umicom/trading/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the trading order cancel data shared with callers of this public contract.
 */
typedef struct UmiTradingOrderCancel { UmiFinancialId client_order_id; uint64_t expected_version; uint32_t reason_code; } UmiTradingOrderCancel;
/* Initialise and validate describe a cancellable order request with version control and reason code. */
UmiStatus umi_trading_order_cancel_init(UmiTradingOrderCancel *value,const UmiFinancialId * client_order_id, uint64_t expected_version, uint32_t reason_code);
/* Validate the invariant set for this trading record. */
bool umi_trading_order_cancel_valid(const UmiTradingOrderCancel *value);
#ifdef __cplusplus
}
#endif
#endif
