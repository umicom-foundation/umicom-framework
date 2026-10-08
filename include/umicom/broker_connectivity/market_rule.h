/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/broker_connectivity/market_rule.h
 * PURPOSE: Retrieve read-only broker price bands while retaining exact route and response identity.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BROKER_CONNECTIVITY_MARKET_RULE_H
#define UMICOM_BROKER_CONNECTIVITY_MARKET_RULE_H
#include "umicom/broker_connectivity/contract_details.h"
#include "umicom/trading/price_increment.h"
#ifdef __cplusplus
extern "C"
{
#endif
#define UMI_IBKR_MARKET_RULE_LIMIT 8U
    typedef struct UmiIbkrMarketRuleSnapshot
    {
        uint32_t ruleId;
        uint64_t requestedAtMilliseconds, receivedAtMilliseconds;
        size_t count;
        bool complete, stale;
        UmiTradingPriceIncrement bands[UMI_TRADING_PRICE_INCREMENT_LIMIT];
    } UmiIbkrMarketRuleSnapshot;
    /* Pair the exchange and rule lists by position. Require a complete, nonstale
 * contract capture, exact case-sensitive exchange and one unambiguous match.
 * Missing/zero identifiers are UNAVAILABLE; malformed lists are PARSE_ERROR.
 * The copied identifier is metadata, never evidence of FOK/AON support. */
    UmiStatus UmiIbkrContractRouteRule(const UmiIbkrContractDetailsSnapshot *details, size_t row,
                                       const char *exchange, uint32_t *outRule);
    /* Queue a read-only rule query. TWS replies carry a rule ID, not a unique query
 * ID, so each rule is requested at most once per connection. Repeated pending
 * requests return BUSY, completed requests reuse the captured value, and an
 * expired request returns TIMEOUT. Reconnect for a fresh capture. */
    UmiStatus UmiIbkrMarketRuleRequest(UmiIbkrConnection *connection, uint32_t ruleId,
                                       uint64_t nowMilliseconds);
    /* Copies stay valid after connection closure but are marked stale. No pointer
 * into the live owner escapes. There is no timer thread: the frontend pumps the
 * connection and supplies monotonic time, as it does for quote observations. */
    UmiStatus UmiIbkrMarketRuleCopy(const UmiIbkrConnection *connection, uint32_t ruleId,
                                    uint64_t nowMilliseconds, UmiIbkrMarketRuleSnapshot *out);
#ifdef __cplusplus
}
#endif
#endif
