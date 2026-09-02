/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/types.h
 *
 * PURPOSE:
 *   Define canonical toolkit-neutral trading records for instruments, market data, orders, fills, positions, risk and research.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * These records are the shared vocabulary used by Trader, TMS, broker adapters, replay tools and tests. They contain no GTK or vendor SDK types.
 */

#ifndef UMICOM_TRADING_TYPES_H
#define UMICOM_TRADING_TYPES_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/finance/types.h"
#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TRADING_SYMBOL_CAPACITY 32U
#define UMI_TRADING_VENUE_CAPACITY 24U
#define UMI_TRADING_TEXT_CAPACITY 128U
#define UMI_TRADING_BOOK_CAPACITY 64U
#define UMI_TRADING_MAX_DEPTH 16U
#define UMI_TRADING_MAX_ORDERS 128U
#define UMI_TRADING_MAX_POSITIONS 64U
#define UMI_TRADING_MAX_FACTORS 32U
#define UMI_TRADING_MAX_WATCHLIST 64U

/**
 * List the named trading environment values accepted by this public contract.
 */
typedef enum UmiTradingEnvironment {
    UMI_TRADING_SIMULATION = 0,
    UMI_TRADING_PAPER = 1,
    UMI_TRADING_LIVE = 2
} UmiTradingEnvironment;

/**
 * List the named side values accepted by this public contract.
 */
typedef enum UmiSide {
    UMI_SIDE_BUY = 1,
    UMI_SIDE_SELL = -1
} UmiSide;

/**
 * List the named order type values accepted by this public contract.
 */
typedef enum UmiOrderType {
    UMI_ORDER_MARKET = 0,
    UMI_ORDER_LIMIT = 1,
    UMI_ORDER_STOP = 2,
    UMI_ORDER_STOP_LIMIT = 3
} UmiOrderType;

/**
 * List the named time in force values accepted by this public contract.
 */
typedef enum UmiTimeInForce {
    UMI_TIF_DAY = 0,
    UMI_TIF_GTC = 1,
    UMI_TIF_IOC = 2,
    UMI_TIF_FOK = 3
} UmiTimeInForce;

/**
 * List the named order status values accepted by this public contract.
 */
typedef enum UmiOrderStatus {
    UMI_ORDER_NEW = 0,
    UMI_ORDER_VALIDATED = 1,
    UMI_ORDER_ACCEPTED = 2,
    UMI_ORDER_PARTIALLY_FILLED = 3,
    UMI_ORDER_FILLED = 4,
    UMI_ORDER_CANCELLED = 5,
    UMI_ORDER_REJECTED = 6
} UmiOrderStatus;

/**
 * List the named market state values accepted by this public contract.
 */
typedef enum UmiMarketState {
    UMI_MARKET_CLOSED = 0,
    UMI_MARKET_PREOPEN = 1,
    UMI_MARKET_OPEN = 2,
    UMI_MARKET_HALTED = 3
} UmiMarketState;

/**
 * Represent the instrument data shared with callers of this public contract.
 */
typedef struct UmiInstrument {
    UmiFinancialId instrument_id;
    char symbol[UMI_TRADING_SYMBOL_CAPACITY];
    char venue[UMI_TRADING_VENUE_CAPACITY];
    UmiCurrency currency;
    double multiplier;
    int32_t expiry_yyyymmdd;
} UmiInstrument;

/**
 * Represent the quote data shared with callers of this public contract.
 */
typedef struct UmiQuote {
    UmiInstrument instrument;
    double bid;
    double ask;
    double bid_size;
    double ask_size;
    int64_t event_time_ms;
} UmiQuote;

/**
 * Represent the trade tick data shared with callers of this public contract.
 */
typedef struct UmiTradeTick {
    UmiInstrument instrument;
    double price;
    double size;
    int64_t event_time_ms;
} UmiTradeTick;

/**
 * Represent the bar data shared with callers of this public contract.
 */
typedef struct UmiBar {
    UmiInstrument instrument;
    double open;
    double high;
    double low;
    double close;
    double volume;
    int64_t start_time_ms;
    int64_t end_time_ms;
} UmiBar;

/**
 * Represent the depth level data shared with callers of this public contract.
 */
typedef struct UmiDepthLevel {
    double price;
    double size;
} UmiDepthLevel;

/**
 * Represent the market depth data shared with callers of this public contract.
 */
typedef struct UmiMarketDepth {
    UmiInstrument instrument;
    UmiDepthLevel bids[UMI_TRADING_MAX_DEPTH];
    UmiDepthLevel asks[UMI_TRADING_MAX_DEPTH];
    size_t bid_count;
    size_t ask_count;
    int64_t event_time_ms;
} UmiMarketDepth;

/**
 * Represent the order request data shared with callers of this public contract.
 */
typedef struct UmiOrderRequest {
    UmiFinancialId client_order_id;
    UmiFinancialId account_id;
    UmiInstrument instrument;
    UmiSide side;
    UmiOrderType type;
    UmiTimeInForce tif;
    double quantity;
    double limit_price;
    double stop_price;
    UmiTradingEnvironment environment;
} UmiOrderRequest;

/**
 * Represent the order data shared with callers of this public contract.
 */
typedef struct UmiOrder {
    UmiOrderRequest request;
    UmiOrderStatus status;
    double filled_quantity;
    double average_fill_price;
    uint64_t version;
} UmiOrder;

/**
 * Represent the execution report data shared with callers of this public contract.
 */
typedef struct UmiExecutionReport {
    UmiFinancialId execution_id;
    UmiFinancialId client_order_id;
    double fill_quantity;
    double fill_price;
    int64_t event_time_ms;
} UmiExecutionReport;

/**
 * Represent the position data shared with callers of this public contract.
 */
typedef struct UmiPosition {
    UmiInstrument instrument;
    double quantity;
    double average_price;
    double realised_pnl;
} UmiPosition;

/**
 * Represent the risk limit data shared with callers of this public contract.
 */
typedef struct UmiRiskLimit {
    double max_order_quantity;
    double max_order_notional;
    double max_position_quantity;
    double max_daily_loss;
} UmiRiskLimit;

/**
 * Represent the risk decision data shared with callers of this public contract.
 */
typedef struct UmiRiskDecision {
    int allowed;
    char reason[UMI_TRADING_TEXT_CAPACITY];
} UmiRiskDecision;

/**
 * Represent the replay event data shared with callers of this public contract.
 */
typedef struct UmiReplayEvent {
    uint64_t sequence;
    int64_t event_time_ms;
    char type[32];
    char payload[UMI_TRADING_TEXT_CAPACITY];
} UmiReplayEvent;

/**
 * Represent the strategy signal data shared with callers of this public contract.
 */
typedef struct UmiStrategySignal {
    double score;
    double probability;
    int direction;
    int64_t valid_until_ms;
} UmiStrategySignal;

/**
 * Represent the market factor data shared with callers of this public contract.
 */
typedef struct UmiMarketFactor {
    char name[48];
    double value_before;
    double value_after;
    double contribution;
} UmiMarketFactor;

/**
 * Represent the movement event data shared with callers of this public contract.
 */
typedef struct UmiMovementEvent {
    double start_price;
    double end_price;
    double absolute_points;
    int64_t start_time_ms;
    int64_t end_time_ms;
    int qualifies;
} UmiMovementEvent;

#ifdef __cplusplus
}
#endif
#endif
