/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/market_rule.c
 * PURPOSE: Own read-only market-rule captures without confusing delayed replies with a retry.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static UmiStatus RuleIgnored(UmiIbkrConnection *connection)
{
    if (connection->snapshot.ignoredFrames == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    ++connection->snapshot.ignoredFrames;
    return UMI_STATUS_OK;
}
static bool RuleExpired(const UmiIbkrConnection *connection, const UmiIbkrMarketRuleSnapshot *rule,
                        uint64_t now)
{
    return !rule->complete && now - rule->requestedAtMilliseconds >= connection->options.timeoutMilliseconds;
}
UmiStatus UmiIbkrMarketRuleRequest(UmiIbkrConnection *connection, uint32_t ruleId, uint64_t now)
{
    if (connection == NULL || ruleId == 0U || ruleId > INT_MAX || now < connection->lastNow)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (connection->snapshot.state != UMI_IBKR_READY)
        return UMI_STATUS_INVALID_STATE;
    if (connection->snapshot.protocolVersion < 151 || connection->snapshot.protocolVersion > 176)
        return UMI_STATUS_NOT_IMPLEMENTED;
    for (size_t index = 0U; index < connection->marketRuleCount; ++index)
    {
        const UmiIbkrMarketRuleSnapshot *rule = &connection->marketRules[index];
        if (rule->ruleId == ruleId)
        {
            if (rule->complete)
                return UMI_STATUS_OK;
            return RuleExpired(connection, rule, now) ? UMI_STATUS_TIMEOUT : UMI_STATUS_BUSY;
        }
    }
    if (connection->marketRuleCount == UMI_IBKR_MARKET_RULE_LIMIT)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    char identity[24];
    (void)snprintf(identity, sizeof identity, "%u", (unsigned)ruleId);
    const char *fields[] = {"91", identity};
    UmiStatus status = UmiIbkrQueueFields(connection, fields, 2U);
    if (status != UMI_STATUS_OK)
        return status;
    UmiIbkrMarketRuleSnapshot *rule = &connection->marketRules[connection->marketRuleCount++];
    memset(rule, 0, sizeof *rule);
    rule->ruleId = ruleId;
    rule->requestedAtMilliseconds = now;
    connection->lastNow = now;
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrMarketRuleCopy(const UmiIbkrConnection *connection, uint32_t ruleId, uint64_t now,
                                UmiIbkrMarketRuleSnapshot *out)
{
    if (connection == NULL || out == NULL || ruleId == 0U || now < connection->lastNow)
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t index = 0U; index < connection->marketRuleCount; ++index)
    {
        const UmiIbkrMarketRuleSnapshot *rule = &connection->marketRules[index];
        if (rule->ruleId == ruleId)
        {
            *out = *rule;
            out->stale = connection->snapshot.state != UMI_IBKR_READY || RuleExpired(connection, rule, now);
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}
UmiStatus UmiIbkrMarketRuleFrame(UmiIbkrConnection *connection, const unsigned char *body, size_t length,
                                 uint64_t now)
{
    if (length == 0U || body[length - 1U] != 0U)
        return UMI_STATUS_PARSE_ERROR;
    char *copy = malloc(length);
    if (copy == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    memcpy(copy, body, length);
    char *fields[3U + 2U * UMI_TRADING_PRICE_INCREMENT_LIMIT];
    size_t start = 0U, count = 0U;
    UmiStatus status = UMI_STATUS_OK;
    for (size_t index = 0U; index < length; ++index)
        if (copy[index] == '\0')
        {
            if (count == sizeof fields / sizeof fields[0])
            {
                status = UMI_STATUS_CAPACITY_EXCEEDED;
                break;
            }
            fields[count++] = copy + start;
            start = index + 1U;
        }
    uint64_t identity = 0U, bandCount = 0U;
    if (status == UMI_STATUS_OK && (count < 3U || !UmiIbkrUnsigned(fields[1], &identity) || identity == 0U ||
                                    identity > INT_MAX || !UmiIbkrUnsigned(fields[2], &bandCount)))
        status = UMI_STATUS_PARSE_ERROR;
    if (status != UMI_STATUS_OK)
    {
        free(copy);
        return status;
    }
    UmiIbkrMarketRuleSnapshot *owned = NULL;
    for (size_t index = 0U; index < connection->marketRuleCount; ++index)
        if (connection->marketRules[index].ruleId == identity)
        {
            owned = &connection->marketRules[index];
            break;
        }
    /* An expired query is never retried under the same connection. Its eventual
     * reply therefore cannot be mistaken for newly requested price information. */
    if (owned == NULL || owned->complete || RuleExpired(connection, owned, now))
    {
        free(copy);
        return RuleIgnored(connection);
    }
    if (bandCount > UMI_TRADING_PRICE_INCREMENT_LIMIT)
    {
        free(copy);
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (count != 3U + 2U * (size_t)bandCount)
    {
        free(copy);
        return UMI_STATUS_PARSE_ERROR;
    }
    UmiIbkrMarketRuleSnapshot candidate = *owned;
    candidate.count = (size_t)bandCount;
    for (size_t index = 0U; status == UMI_STATUS_OK && index < candidate.count; ++index)
    {
        const char *lower = fields[3U + 2U * index], *increment = fields[4U + 2U * index];
        status = UmiDecimalParseScientificExact(lower, strlen(lower), &candidate.bands[index].lowerBound);
        if (status == UMI_STATUS_OK)
            status = UmiDecimalParseScientificExact(increment, strlen(increment),
                                                    &candidate.bands[index].increment);
    }
    if (status == UMI_STATUS_OK && candidate.count != 0U)
        status = UmiTradingPriceIncrementsValidate(candidate.bands, candidate.count);
    if (status == UMI_STATUS_OK)
    {
        candidate.complete = true;
        candidate.receivedAtMilliseconds = now;
        *owned = candidate;
    }
    free(copy);
    return status;
}
