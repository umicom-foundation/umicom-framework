/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/types.h
 *
 * PURPOSE:
 *   Define fixed-capacity exchange, brokerage and matching primitives shared by advanced trading services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_TYPES_H
#define UMICOM_TRADING_CORE_TYPES_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/finance/types.h"
#include "umicom/trading/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_TRADING_CORE_TEXT_CAPACITY 128U
#define UMI_TRADING_CORE_MAX_ITEMS 64U
#define UMI_TRADING_CORE_MAX_EVENTS 128U
#define UMI_TRADING_CORE_MAX_BOOK_LEVELS 32U
typedef int64_t UmiTradingPriceTicks;
typedef int64_t UmiTradingQuantityLots;
/**
 * List the named trading core order state values accepted by this public contract.
 */
typedef enum UmiTradingCoreOrderState { UMI_TRADING_CORE_ORDER_PENDING_NEW=0, UMI_TRADING_CORE_ORDER_OPEN=1, UMI_TRADING_CORE_ORDER_PARTIAL=2, UMI_TRADING_CORE_ORDER_FILLED=3, UMI_TRADING_CORE_ORDER_PENDING_CANCEL=4, UMI_TRADING_CORE_ORDER_CANCELLED=5, UMI_TRADING_CORE_ORDER_REJECTED=6, UMI_TRADING_CORE_ORDER_EXPIRED=7 } UmiTradingCoreOrderState;
/**
 * List the named trading core market phase values accepted by this public contract.
 */
typedef enum UmiTradingCoreMarketPhase { UMI_TRADING_CORE_PHASE_CLOSED=0, UMI_TRADING_CORE_PHASE_PREOPEN=1, UMI_TRADING_CORE_PHASE_AUCTION=2, UMI_TRADING_CORE_PHASE_CONTINUOUS=3, UMI_TRADING_CORE_PHASE_HALT=4, UMI_TRADING_CORE_PHASE_POSTCLOSE=5 } UmiTradingCoreMarketPhase;
/**
 * List the named trading core severity values accepted by this public contract.
 */
typedef enum UmiTradingCoreSeverity { UMI_TRADING_CORE_INFO=0, UMI_TRADING_CORE_WARNING=1, UMI_TRADING_CORE_CRITICAL=2 } UmiTradingCoreSeverity;
/**
 * Represent the trading core decision data shared with callers of this public contract.
 */
typedef struct UmiTradingCoreDecision { bool allowed; char reason[UMI_TRADING_CORE_TEXT_CAPACITY]; } UmiTradingCoreDecision;
/* Copy bounded text and reject truncation. */
UmiStatus umi_trading_core_copy_text(char *destination,size_t capacity,const char *source);
/* Assign a bounded Framework financial identifier. */
UmiStatus umi_trading_core_id_assign(UmiFinancialId *id,const char *text);
/* Compare two Framework financial identifiers. */
bool umi_trading_core_id_equal(const UmiFinancialId *left,const UmiFinancialId *right);
/* Return an absolute signed value while protecting INT64_MIN. */
UmiStatus umi_trading_core_abs_i64(int64_t value,int64_t *out_value);
/* Add signed 64-bit values with overflow detection. */
UmiStatus umi_trading_core_add_i64(int64_t left,int64_t right,int64_t *out_value);
#ifdef __cplusplus
}
#endif
#endif
