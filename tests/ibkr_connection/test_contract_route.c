/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_contract_route.c
 * PURPOSE: Reject mismatched, ambiguous or incomplete exchange-to-rule mappings.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/broker_connectivity/market_rule.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(x))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "%d: %s\n", __LINE__, #x);                                                       \
            return 1;                                                                                        \
        }                                                                                                    \
    } while (0)
static int Run(const char *mode, UmiIbkrContractDetailsSnapshot *details)
{
    details->count = 1U;
    details->complete = true;
    strcpy(details->items[0].validExchanges, "SMART,LSE,ISLAND");
    strcpy(details->items[0].marketRuleIds, "26,28,27");
    uint32_t rule = 99U;
    if (!strcmp(mode, "valid"))
    {
        CHECK(UmiIbkrContractRouteRule(details, 0U, "LSE", &rule) == UMI_STATUS_OK && rule == 28U);
        CHECK(UmiIbkrContractRouteRule(details, 0U, "ISLAND", &rule) == UMI_STATUS_OK && rule == 27U);
        CHECK(UmiIbkrContractRouteRule(details, 0U, "SMART", &rule) == UMI_STATUS_OK && rule == 26U);
        return 0;
    }
    UmiStatus expected = UMI_STATUS_PARSE_ERROR;
    const char *exchange = "LSE";
    if (!strcmp(mode, "partial"))
    {
        details->complete = false;
        expected = UMI_STATUS_INVALID_STATE;
    }
    else if (!strcmp(mode, "stale"))
    {
        details->stale = true;
        expected = UMI_STATUS_INVALID_STATE;
    }
    else if (!strcmp(mode, "failed"))
    {
        details->failed = true;
        expected = UMI_STATUS_INVALID_STATE;
    }
    else if (!strcmp(mode, "unknown"))
    {
        exchange = "LSEETF";
        expected = UMI_STATUS_UNAVAILABLE;
    }
    else if (!strcmp(mode, "case-sensitive"))
    {
        exchange = "lse";
        expected = UMI_STATUS_UNAVAILABLE;
    }
    else if (!strcmp(mode, "empty"))
    {
        details->items[0].marketRuleIds[0] = '\0';
        expected = UMI_STATUS_UNAVAILABLE;
    }
    else if (!strcmp(mode, "zero"))
    {
        strcpy(details->items[0].marketRuleIds, "26,0,27");
        expected = UMI_STATUS_UNAVAILABLE;
    }
    else if (!strcmp(mode, "unequal"))
        strcpy(details->items[0].marketRuleIds, "26,28");
    else if (!strcmp(mode, "duplicate"))
        strcpy(details->items[0].validExchanges, "LSE,LSE,ISLAND");
    else if (!strcmp(mode, "interior-empty"))
        strcpy(details->items[0].marketRuleIds, "26,,27");
    else if (!strcmp(mode, "overflow"))
        strcpy(details->items[0].marketRuleIds, "26,2147483648,27");
    else if (!strcmp(mode, "unterminated"))
        memset(details->items[0].validExchanges, 'x', sizeof details->items[0].validExchanges);
    else
        return 2;
    CHECK(UmiIbkrContractRouteRule(details, 0U, exchange, &rule) == expected);
    CHECK(rule == 99U);
    return 0;
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    UmiIbkrContractDetailsSnapshot *details = calloc(1U, sizeof *details);
    if (details == NULL)
        return 1;
    int result = Run(argv[1], details);
    free(details);
    return result;
}
