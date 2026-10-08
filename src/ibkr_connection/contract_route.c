/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/contract_route.c
 * PURPOSE: Match each reported exchange to its corresponding market-rule identifier without guessing.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include <limits.h>
#include <string.h>
UmiStatus UmiIbkrContractRouteRule(const UmiIbkrContractDetailsSnapshot *details, size_t row,
                                   const char *exchange, uint32_t *outRule)
{
    if (details == NULL || outRule == NULL || !UmiIbkrText(exchange, 64U, false) ||
        details->count > UMI_IBKR_CONTRACT_DETAILS_LIMIT || row >= details->count)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!details->complete || details->failed || details->stale)
        return UMI_STATUS_INVALID_STATE;
    const UmiIbkrContractDescription *description = &details->items[row];
    if (!UmiIbkrText(description->validExchanges, sizeof description->validExchanges, true) ||
        !UmiIbkrText(description->marketRuleIds, sizeof description->marketRuleIds, true))
        return UMI_STATUS_PARSE_ERROR;
    const char *venue = description->validExchanges, *rule = description->marketRuleIds;
    if (*venue == '\0' || *rule == '\0')
        return UMI_STATUS_UNAVAILABLE;
    bool found = false;
    uint32_t selected = 0U;
    for (;;)
    {
        const char *venueEnd = strchr(venue, ','), *ruleEnd = strchr(rule, ',');
        size_t venueLength = venueEnd ? (size_t)(venueEnd - venue) : strlen(venue);
        size_t ruleLength = ruleEnd ? (size_t)(ruleEnd - rule) : strlen(rule);
        if (venueLength == 0U || venueLength >= 64U || ruleLength == 0U || ruleLength >= 24U ||
            ((venueEnd == NULL) != (ruleEnd == NULL)))
            return UMI_STATUS_PARSE_ERROR;
        char number[24];
        memcpy(number, rule, ruleLength);
        number[ruleLength] = '\0';
        uint64_t identity;
        if (!UmiIbkrUnsigned(number, &identity) || identity > INT_MAX)
            return UMI_STATUS_PARSE_ERROR;
        if (strlen(exchange) == venueLength && memcmp(exchange, venue, venueLength) == 0)
        {
            /* Even repeated equal identifiers are ambiguous metadata. Requiring
             * one route entry prevents accidental first-match selection. */
            if (found)
                return UMI_STATUS_PARSE_ERROR;
            found = true;
            selected = (uint32_t)identity;
        }
        if (venueEnd == NULL)
            break;
        venue = venueEnd + 1;
        rule = ruleEnd + 1;
    }
    if (!found || selected == 0U)
        return UMI_STATUS_UNAVAILABLE;
    *outRule = selected;
    return UMI_STATUS_OK;
}
