/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/trade_tape.h
 *
 * PURPOSE:
 *   Retain a bounded, sequence-checked public market trade tape with shared
 *   filtering and pause behaviour for every Umicom trading surface.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_TRADE_TAPE_H
#define UMICOM_TRADING_TRADE_TAPE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/trading/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TRADING_TRADE_TAPE_API_VERSION 1U
#define UMI_TRADING_TRADE_TAPE_CAPACITY 512U
#define UMI_TRADING_TRADE_CONDITION_CAPACITY 32U

/** Explain which side initiated a public market trade when the feed knows it. */
typedef enum UmiTradingTradeDirection {
    UMI_TRADING_TRADE_DIRECTION_UNKNOWN = 0,
    UMI_TRADING_TRADE_DIRECTION_BUYER_INITIATED = 1,
    UMI_TRADING_TRADE_DIRECTION_SELLER_INITIATED = 2
} UmiTradingTradeDirection;

/** Select which trade directions remain visible in a Time and Sales panel. */
typedef enum UmiTradingTradeTapeFilter {
    UMI_TRADING_TRADE_TAPE_ALL = 0,
    UMI_TRADING_TRADE_TAPE_BUYER_INITIATED = 1,
    UMI_TRADING_TRADE_TAPE_SELLER_INITIATED = 2,
    UMI_TRADING_TRADE_TAPE_UNKNOWN = 3
} UmiTradingTradeTapeFilter;

/**
 * Store one normalised public trade independently from account executions.
 * Sequence is supplied by the accepted market-data adapter. Condition is an
 * optional short provider-neutral label such as regular, auction, or block.
 */
typedef struct UmiTradingTradeTapeRecord {
    uint32_t structure_size;
    uint32_t api_version;
    uint64_t sequence;
    UmiTradeTick trade;
    UmiTradingTradeDirection direction;
    char condition[UMI_TRADING_TRADE_CONDITION_CAPACITY];
} UmiTradingTradeTapeRecord;

/** Copy observable tape health and filter state without exposing its storage. */
typedef struct UmiTradingTradeTapeSnapshot {
    uint32_t structure_size;
    uint32_t api_version;
    size_t retained_count;
    size_t visible_count;
    uint64_t last_sequence;
    uint64_t missing_sequence_count;
    uint64_t dropped_trade_count;
    uint64_t paused_at_sequence;
    uint64_t revision;
    double minimum_size;
    UmiTradingTradeTapeFilter filter;
    int provider_ready;
    int paused;
} UmiTradingTradeTapeSnapshot;

/** Opaque bounded tape owned by a trading workspace or another client. */
typedef struct UmiTradingTradeTape UmiTradingTradeTape;

/** Build a validated record from one normalised public market trade. */
UmiStatus umi_trading_trade_tape_record_init(
    UmiTradingTradeTapeRecord *record,
    uint64_t sequence,
    const UmiTradeTick *trade,
    UmiTradingTradeDirection direction,
    const char *condition);

/** Allocate an empty tape with conservative show-all defaults. */
UmiStatus umi_trading_trade_tape_create(UmiTradingTradeTape **out_tape);

/** Release a tape created by `umi_trading_trade_tape_create`. */
void umi_trading_trade_tape_destroy(UmiTradingTradeTape *tape);

/** Report whether an accepted public trade provider is currently connected. */
UmiStatus umi_trading_trade_tape_set_provider_ready(
    UmiTradingTradeTape *tape,
    int ready);

/**
 * Append one trade only when its sequence is newer than every accepted trade.
 * Gaps are counted for data-quality reporting and old rows are dropped safely
 * when the fixed capacity is full.
 */
UmiStatus umi_trading_trade_tape_append(
    UmiTradingTradeTape *tape,
    const UmiTradingTradeTapeRecord *record);

/** Apply a direction and minimum-size filter without discarding stored trades. */
UmiStatus umi_trading_trade_tape_set_filter(
    UmiTradingTradeTape *tape,
    UmiTradingTradeTapeFilter filter,
    double minimum_size);

/** Freeze or resume the visible sequence while ingestion continues safely. */
UmiStatus umi_trading_trade_tape_set_paused(
    UmiTradingTradeTape *tape,
    int paused);

/** Remove retained trades for an instrument that left its workspace. */
UmiStatus umi_trading_trade_tape_remove_instrument(
    UmiTradingTradeTape *tape,
    const char *instrument_id);

/** Return the visible filtered row count for one instrument or the whole tape. */
size_t umi_trading_trade_tape_visible_count(
    const UmiTradingTradeTape *tape,
    const char *instrument_id);

/** Copy one visible row in newest-first order. */
UmiStatus umi_trading_trade_tape_visible_at(
    const UmiTradingTradeTape *tape,
    const char *instrument_id,
    size_t newest_first_index,
    UmiTradingTradeTapeRecord *out_record);

/** Copy tape health, retention, filtering, and pause state. */
UmiStatus umi_trading_trade_tape_snapshot(
    const UmiTradingTradeTape *tape,
    UmiTradingTradeTapeSnapshot *out_snapshot);

/** Return a short readable name for a public-trade direction. */
const char *umi_trading_trade_direction_text(
    UmiTradingTradeDirection direction);

/** Return a short readable name for a Time and Sales filter. */
const char *umi_trading_trade_tape_filter_text(
    UmiTradingTradeTapeFilter filter);

#ifdef __cplusplus
}
#endif

#endif
