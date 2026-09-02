/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/alert.c
 *
 * PURPOSE:
 *   Implement provider-neutral price-alert rules and their bounded lifecycle.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Product-specific presentation and external market providers stay outside
 * this deterministic module. Every operation checks its fixed bounds before
 * writing, and no alert record owns dynamically allocated memory.
 */

#include "umicom/trading/alert.h"

#include <math.h>
#include <string.h>

/* Measure text only inside the capacity owned by its contract. */
static size_t bounded_text_length(const char *text, size_t capacity)
{
    size_t length = 0U;

    if (text == NULL) {
        return capacity;
    }
    while (length < capacity && text[length] != '\0') {
        length += 1U;
    }
    return length;
}

/* Check collection metadata before using its count as an array boundary. */
static int valid_book(const UmiTradingAlertBook *book)
{
    return book != NULL && book->count <= UMI_TRADING_MAX_ALERTS;
}

/* Return the position of an alert, or the current count when it is absent. */
static size_t find_alert(const UmiTradingAlertBook *book,
                         const char *alert_id)
{
    size_t index;
    size_t identifier_length;

    /* A missing identifier cannot match any valid alert. */
    if (!valid_book(book) || alert_id == NULL) {
        return book != NULL ? book->count : 0U;
    }
    identifier_length = bounded_text_length(
        alert_id, UMI_FINANCE_ID_CAPACITY);
    if (identifier_length == 0U ||
        identifier_length >= UMI_FINANCE_ID_CAPACITY) {
        return book->count;
    }
    for (index = 0U; index < book->count; ++index) {
        /* Stable identifiers make lookup independent of visible ordering. */
        if (strncmp(book->alerts[index].alert_id,
                    alert_id,
                    UMI_FINANCE_ID_CAPACITY) == 0) {
            return index;
        }
    }
    return book->count;
}

/* Check that a stored record follows this version of the public contract. */
static int valid_alert(const UmiTradingPriceAlert *alert)
{
    return alert != NULL &&
           alert->structure_size >= sizeof(*alert) &&
           alert->api_version == UMI_TRADING_PRICE_ALERT_API_VERSION &&
           alert->alert_id[0] != '\0' &&
           alert->instrument_id[0] != '\0' &&
           memchr(alert->alert_id,
                  '\0',
                  sizeof(alert->alert_id)) != NULL &&
           memchr(alert->instrument_id,
                  '\0',
                  sizeof(alert->instrument_id)) != NULL &&
           (alert->direction == UMI_TRADING_PRICE_ALERT_CROSSES_ABOVE ||
            alert->direction == UMI_TRADING_PRICE_ALERT_CROSSES_BELOW) &&
           isfinite(alert->threshold) && alert->threshold > 0.0 &&
           alert->created_at_ms >= 0 &&
           (!alert->has_last_value ||
            (isfinite(alert->last_value) && alert->last_value > 0.0 &&
             alert->last_observed_at_ms >= 0));
}

/* Decide whether one directional rule crossed on this observation. */
static int directional_crossed(UmiTradingPriceAlertDirection direction,
                               double previous_value,
                               double current_value,
                               double threshold)
{
    /* Above rules include a value that lands exactly on the threshold. */
    if (direction == UMI_TRADING_PRICE_ALERT_CROSSES_ABOVE) {
        return previous_value < threshold && current_value >= threshold;
    }
    /* Below rules use the matching inclusive boundary in the other direction. */
    return previous_value > threshold && current_value <= threshold;
}

/* Return true when either direction crosses a finite positive threshold. */
int umi_trading_alert_crossed(double previous_value,
                              double current_value,
                              double threshold)
{
    /* Invalid market values must never activate a user rule. */
    if (!isfinite(previous_value) || !isfinite(current_value) ||
        !isfinite(threshold) || threshold <= 0.0) {
        return 0;
    }
    return directional_crossed(UMI_TRADING_PRICE_ALERT_CROSSES_ABOVE,
                               previous_value,
                               current_value,
                               threshold) ||
           directional_crossed(UMI_TRADING_PRICE_ALERT_CROSSES_BELOW,
                               previous_value,
                               current_value,
                               threshold);
}

/* Initialise one validated rule before adding it to an alert book. */
UmiStatus umi_trading_price_alert_init(
    UmiTradingPriceAlert *alert,
    const char *alert_id,
    const char *instrument_id,
    UmiTradingPriceAlertDirection direction,
    double threshold,
    int64_t created_at_ms)
{
    size_t alert_id_length;
    size_t instrument_id_length;

    /* Validate all caller values before changing the destination record. */
    if (alert == NULL || alert_id == NULL || instrument_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    alert_id_length = bounded_text_length(
        alert_id, sizeof(alert->alert_id));
    instrument_id_length = bounded_text_length(
        instrument_id, sizeof(alert->instrument_id));
    if (alert_id_length == 0U ||
        alert_id_length >= sizeof(alert->alert_id) ||
        instrument_id_length == 0U ||
        instrument_id_length >= sizeof(alert->instrument_id) ||
        (direction != UMI_TRADING_PRICE_ALERT_CROSSES_ABOVE &&
         direction != UMI_TRADING_PRICE_ALERT_CROSSES_BELOW) ||
        !isfinite(threshold) || threshold <= 0.0 || created_at_ms < 0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(alert, 0, sizeof(*alert));
    alert->structure_size = (uint32_t)sizeof(*alert);
    alert->api_version = UMI_TRADING_PRICE_ALERT_API_VERSION;
    (void)memcpy(alert->alert_id, alert_id, alert_id_length + 1U);
    (void)memcpy(alert->instrument_id,
                 instrument_id,
                 instrument_id_length + 1U);
    alert->direction = direction;
    alert->threshold = threshold;
    alert->created_at_ms = created_at_ms;
    alert->enabled = 1;
    return UMI_STATUS_OK;
}

/* Seed a rule with the current price so creation does not invent a crossing. */
UmiStatus umi_trading_price_alert_seed(UmiTradingPriceAlert *alert,
                                       double current_value)
{
    if (!valid_alert(alert) || !isfinite(current_value) ||
        current_value <= 0.0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    alert->last_value = current_value;
    alert->has_last_value = 1;
    return UMI_STATUS_OK;
}

/* Initialise an empty book whose records live inside caller-owned storage. */
void umi_trading_alert_book_init(UmiTradingAlertBook *book)
{
    if (book == NULL) {
        return;
    }
    (void)memset(book, 0, sizeof(*book));
    book->revision = 1U;
}

/* Add a defensive copy of a unique, validated alert rule. */
UmiStatus umi_trading_alert_book_add(UmiTradingAlertBook *book,
                                     const UmiTradingPriceAlert *alert)
{
    if (!valid_book(book) || !valid_alert(alert)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (find_alert(book, alert->alert_id) < book->count) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    if (book->count >= UMI_TRADING_MAX_ALERTS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    book->alerts[book->count++] = *alert;
    book->revision += 1U;
    return UMI_STATUS_OK;
}

/* Remove one rule by stable alert identifier. */
UmiStatus umi_trading_alert_book_remove(UmiTradingAlertBook *book,
                                        const char *alert_id)
{
    size_t position;

    if (!valid_book(book) || alert_id == NULL || alert_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_alert(book, alert_id);
    if (position >= book->count) {
        return UMI_STATUS_NOT_FOUND;
    }
    /* Move later records left so iteration remains compact and deterministic. */
    if (position + 1U < book->count) {
        (void)memmove(&book->alerts[position],
                      &book->alerts[position + 1U],
                      (book->count - position - 1U) * sizeof(book->alerts[0]));
    }
    book->count -= 1U;
    (void)memset(&book->alerts[book->count],
                 0,
                 sizeof(book->alerts[book->count]));
    book->revision += 1U;
    return UMI_STATUS_OK;
}

/* Enable or pause a rule without discarding its occurrence history. */
UmiStatus umi_trading_alert_book_set_enabled(UmiTradingAlertBook *book,
                                             const char *alert_id,
                                             int enabled)
{
    size_t position;
    const int normalised_enabled = enabled != 0;

    if (!valid_book(book) || alert_id == NULL || alert_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_alert(book, alert_id);
    if (position >= book->count) {
        return UMI_STATUS_NOT_FOUND;
    }
    /* Repeating the same request leaves the revision and observers unchanged. */
    if (book->alerts[position].enabled == normalised_enabled) {
        return UMI_STATUS_OK;
    }
    book->alerts[position].enabled = normalised_enabled;
    if (!normalised_enabled) {
        book->alerts[position].active = 0;
    }
    book->revision += 1U;
    return UMI_STATUS_OK;
}

/* Acknowledge one active alert while retaining the rule for later crossings. */
UmiStatus umi_trading_alert_book_acknowledge(UmiTradingAlertBook *book,
                                             const char *alert_id)
{
    size_t position;

    if (!valid_book(book) || alert_id == NULL || alert_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_alert(book, alert_id);
    if (position >= book->count) {
        return UMI_STATUS_NOT_FOUND;
    }
    if (!book->alerts[position].active) {
        return UMI_STATUS_INVALID_STATE;
    }
    book->alerts[position].active = 0;
    book->alerts[position].acknowledged = 1;
    book->revision += 1U;
    return UMI_STATUS_OK;
}

/* Evaluate all rules for an instrument against one ordered market value. */
UmiStatus umi_trading_alert_book_evaluate(UmiTradingAlertBook *book,
                                          const char *instrument_id,
                                          double current_value,
                                          int64_t event_time_ms)
{
    size_t index;
    size_t instrument_id_length;
    int observed = 0;

    instrument_id_length = bounded_text_length(
        instrument_id, UMI_FINANCE_ID_CAPACITY);
    if (!valid_book(book) || instrument_id_length == 0U ||
        instrument_id_length >= UMI_FINANCE_ID_CAPACITY ||
        !isfinite(current_value) || current_value <= 0.0 ||
        event_time_ms < 0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < book->count; ++index) {
        UmiTradingPriceAlert *alert = &book->alerts[index];

        /* A quote only affects rules that belong to its instrument. */
        if (strncmp(alert->instrument_id,
                    instrument_id,
                    UMI_FINANCE_ID_CAPACITY) != 0) {
            continue;
        }
        /* Ignore stale observations so delayed data cannot invent a crossing. */
        if (alert->has_last_value &&
            event_time_ms < alert->last_observed_at_ms) {
            continue;
        }
        observed = 1;
        if (alert->enabled && alert->has_last_value &&
            directional_crossed(alert->direction,
                                alert->last_value,
                                current_value,
                                alert->threshold)) {
            alert->active = 1;
            alert->acknowledged = 0;
            alert->triggered_at_ms = event_time_ms;
            /* Saturation preserves truthful history instead of wrapping to zero. */
            if (alert->occurrences < UINT64_MAX) {
                alert->occurrences += 1U;
            }
        }
        /* Disabled rules still follow price, preventing stale crossings on resume. */
        alert->last_value = current_value;
        alert->last_observed_at_ms = event_time_ms;
        alert->has_last_value = 1;
    }
    if (observed) {
        book->revision += 1U;
    }
    return UMI_STATUS_OK;
}

/* Return the number of stored rules. */
size_t umi_trading_alert_book_count(const UmiTradingAlertBook *book)
{
    return valid_book(book) ? book->count : 0U;
}

/* Return the number of rules currently presenting an active notification. */
size_t umi_trading_alert_book_active_count(const UmiTradingAlertBook *book)
{
    size_t index;
    size_t count = 0U;

    if (!valid_book(book)) {
        return 0U;
    }
    for (index = 0U; index < book->count; ++index) {
        count += (size_t)(book->alerts[index].active != 0);
    }
    return count;
}

/* Return active rules that still require acknowledgement. */
size_t umi_trading_alert_book_unacknowledged_count(
    const UmiTradingAlertBook *book)
{
    size_t index;
    size_t count = 0U;

    if (!valid_book(book)) {
        return 0U;
    }
    for (index = 0U; index < book->count; ++index) {
        count += (size_t)(book->alerts[index].active &&
                          !book->alerts[index].acknowledged);
    }
    return count;
}

/* Copy one rule by position without exposing the book's owned record. */
UmiStatus umi_trading_alert_book_at(const UmiTradingAlertBook *book,
                                    size_t position,
                                    UmiTradingPriceAlert *out_alert)
{
    if (!valid_book(book) || out_alert == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (position >= book->count) {
        return UMI_STATUS_NOT_FOUND;
    }
    *out_alert = book->alerts[position];
    return UMI_STATUS_OK;
}

/* Return short display text for one direction. */
const char *umi_trading_price_alert_direction_text(
    UmiTradingPriceAlertDirection direction)
{
    switch (direction) {
        case UMI_TRADING_PRICE_ALERT_CROSSES_ABOVE: return "crosses above";
        case UMI_TRADING_PRICE_ALERT_CROSSES_BELOW: return "crosses below";
        default: return "unknown direction";
    }
}
