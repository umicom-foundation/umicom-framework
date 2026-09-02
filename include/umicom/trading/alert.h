/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/alert.h
 *
 * PURPOSE:
 *   Define provider-neutral price alerts, their lifecycle and bounded storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This public contract isolates alert behaviour so Trader, treasury tools,
 * tests and market-data adapters can reuse one deterministic rule engine.
 */

#ifndef INCLUDE_UMICOM_TRADING_ALERT_H
#define INCLUDE_UMICOM_TRADING_ALERT_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TRADING_PRICE_ALERT_API_VERSION 1U
#define UMI_TRADING_MAX_ALERTS 128U

/** Describe which side of a price threshold activates an alert. */
typedef enum UmiTradingPriceAlertDirection {
    UMI_TRADING_PRICE_ALERT_CROSSES_ABOVE = 1,
    UMI_TRADING_PRICE_ALERT_CROSSES_BELOW = 2
} UmiTradingPriceAlertDirection;

/**
 * Store one price rule and its latest observable state. The rule contains no
 * provider or user-interface objects, so it can be saved and rendered by any
 * application adapter.
 */
typedef struct UmiTradingPriceAlert {
    uint32_t structure_size;
    uint32_t api_version;
    char alert_id[UMI_FINANCE_ID_CAPACITY];
    char instrument_id[UMI_FINANCE_ID_CAPACITY];
    UmiTradingPriceAlertDirection direction;
    double threshold;
    double last_value;
    int64_t created_at_ms;
    int64_t last_observed_at_ms;
    int64_t triggered_at_ms;
    uint64_t occurrences;
    int enabled;
    int active;
    int acknowledged;
    int has_last_value;
} UmiTradingPriceAlert;

/** Own a fixed-capacity alert collection with predictable memory use. */
typedef struct UmiTradingAlertBook {
    UmiTradingPriceAlert alerts[UMI_TRADING_MAX_ALERTS];
    size_t count;
    uint64_t revision;
} UmiTradingAlertBook;

/**
 * Return true when either direction crosses a threshold. This compatibility
 * helper remains useful to small indicators that do not need a stored rule.
 */
int umi_trading_alert_crossed(double previous_value,
                              double current_value,
                              double threshold);

/** Initialise one validated rule before adding it to an alert book. */
UmiStatus umi_trading_price_alert_init(
    UmiTradingPriceAlert *alert,
    const char *alert_id,
    const char *instrument_id,
    UmiTradingPriceAlertDirection direction,
    double threshold,
    int64_t created_at_ms);

/** Seed a rule with the current price so creation does not invent a crossing. */
UmiStatus umi_trading_price_alert_seed(
    UmiTradingPriceAlert *alert,
    double current_value);

/** Initialise an empty book whose records live inside caller-owned storage. */
void umi_trading_alert_book_init(UmiTradingAlertBook *book);

/** Add a defensive copy of a unique, validated alert rule. */
UmiStatus umi_trading_alert_book_add(
    UmiTradingAlertBook *book,
    const UmiTradingPriceAlert *alert);

/** Remove one rule by stable alert identifier. */
UmiStatus umi_trading_alert_book_remove(
    UmiTradingAlertBook *book,
    const char *alert_id);

/** Enable or pause a rule without discarding its occurrence history. */
UmiStatus umi_trading_alert_book_set_enabled(
    UmiTradingAlertBook *book,
    const char *alert_id,
    int enabled);

/** Acknowledge one active alert while retaining the rule for later crossings. */
UmiStatus umi_trading_alert_book_acknowledge(
    UmiTradingAlertBook *book,
    const char *alert_id);

/** Evaluate all rules for an instrument against one ordered market value. */
UmiStatus umi_trading_alert_book_evaluate(
    UmiTradingAlertBook *book,
    const char *instrument_id,
    double current_value,
    int64_t event_time_ms);

/** Return the number of stored rules. */
size_t umi_trading_alert_book_count(const UmiTradingAlertBook *book);

/** Return the number of rules currently presenting an active notification. */
size_t umi_trading_alert_book_active_count(const UmiTradingAlertBook *book);

/** Return active rules that still require acknowledgement. */
size_t umi_trading_alert_book_unacknowledged_count(
    const UmiTradingAlertBook *book);

/** Copy one rule by position without exposing the book's owned record. */
UmiStatus umi_trading_alert_book_at(
    const UmiTradingAlertBook *book,
    size_t position,
    UmiTradingPriceAlert *out_alert);

/** Return short display text for one direction. */
const char *umi_trading_price_alert_direction_text(
    UmiTradingPriceAlertDirection direction);
#ifdef __cplusplus
}
#endif
#endif
