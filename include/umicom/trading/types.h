/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/types.h
 *
 * PURPOSE:
 *   Define canonical toolkit-neutral trading records for instruments, market data, orders, fills, positions, risk and research.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
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

typedef enum UmiTradingEnvironment {
    UMI_TRADING_SIMULATION = 0,
    UMI_TRADING_PAPER = 1,
    UMI_TRADING_LIVE = 2
} UmiTradingEnvironment;

typedef enum UmiSide {
    UMI_SIDE_BUY = 1,
    UMI_SIDE_SELL = -1
} UmiSide;

typedef enum UmiOrderType {
    UMI_ORDER_MARKET = 0,
    UMI_ORDER_LIMIT = 1,
    UMI_ORDER_STOP = 2,
    UMI_ORDER_STOP_LIMIT = 3
} UmiOrderType;

typedef enum UmiTimeInForce {
    UMI_TIF_DAY = 0,
    UMI_TIF_GTC = 1,
    UMI_TIF_IOC = 2,
    UMI_TIF_FOK = 3
} UmiTimeInForce;

typedef enum UmiOrderStatus {
    UMI_ORDER_NEW = 0,
    UMI_ORDER_VALIDATED = 1,
    UMI_ORDER_ACCEPTED = 2,
    UMI_ORDER_PARTIALLY_FILLED = 3,
    UMI_ORDER_FILLED = 4,
    UMI_ORDER_CANCELLED = 5,
    UMI_ORDER_REJECTED = 6
} UmiOrderStatus;

typedef enum UmiMarketState {
    UMI_MARKET_CLOSED = 0,
    UMI_MARKET_PREOPEN = 1,
    UMI_MARKET_OPEN = 2,
    UMI_MARKET_HALTED = 3
} UmiMarketState;

typedef struct UmiInstrument {
    UmiFinancialId instrument_id;
    char symbol[UMI_TRADING_SYMBOL_CAPACITY];
    char venue[UMI_TRADING_VENUE_CAPACITY];
    UmiCurrency currency;
    double multiplier;
    int32_t expiry_yyyymmdd;
} UmiInstrument;

typedef struct UmiQuote {
    UmiInstrument instrument;
    double bid;
    double ask;
    double bid_size;
    double ask_size;
    int64_t event_time_ms;
} UmiQuote;

typedef struct UmiTradeTick {
    UmiInstrument instrument;
    double price;
    double size;
    int64_t event_time_ms;
} UmiTradeTick;

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

typedef struct UmiDepthLevel {
    double price;
    double size;
} UmiDepthLevel;

typedef struct UmiMarketDepth {
    UmiInstrument instrument;
    UmiDepthLevel bids[UMI_TRADING_MAX_DEPTH];
    UmiDepthLevel asks[UMI_TRADING_MAX_DEPTH];
    size_t bid_count;
    size_t ask_count;
    int64_t event_time_ms;
} UmiMarketDepth;

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

typedef struct UmiOrder {
    UmiOrderRequest request;
    UmiOrderStatus status;
    double filled_quantity;
    double average_fill_price;
    uint64_t version;
} UmiOrder;

typedef struct UmiExecutionReport {
    UmiFinancialId execution_id;
    UmiFinancialId client_order_id;
    double fill_quantity;
    double fill_price;
    int64_t event_time_ms;
} UmiExecutionReport;

typedef struct UmiPosition {
    UmiInstrument instrument;
    double quantity;
    double average_price;
    double realised_pnl;
} UmiPosition;

typedef struct UmiRiskLimit {
    double max_order_quantity;
    double max_order_notional;
    double max_position_quantity;
    double max_daily_loss;
} UmiRiskLimit;

typedef struct UmiRiskDecision {
    int allowed;
    char reason[UMI_TRADING_TEXT_CAPACITY];
} UmiRiskDecision;

typedef struct UmiReplayEvent {
    uint64_t sequence;
    int64_t event_time_ms;
    char type[32];
    char payload[UMI_TRADING_TEXT_CAPACITY];
} UmiReplayEvent;

typedef struct UmiStrategySignal {
    double score;
    double probability;
    int direction;
    int64_t valid_until_ms;
} UmiStrategySignal;

typedef struct UmiMarketFactor {
    char name[48];
    double value_before;
    double value_after;
    double contribution;
} UmiMarketFactor;

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
