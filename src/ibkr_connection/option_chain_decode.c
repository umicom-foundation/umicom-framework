/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/option_chain_decode.c
 * PURPOSE: Validate bounded expiry and strike sets before publishing an option definition.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "observation_wire.h"
#include "order_numbers.h"
#include <limits.h>
#include <stdlib.h>
#include <string.h>
static UmiStatus ReadChain(char **f, size_t count, UmiIbkrOptionChain *row)
{
    uint64_t underlying, expiries, strikes;
    if (count < 8U || !UmiIbkrText(f[2], sizeof row->exchange, false) ||
        !UmiIbkrUnsigned(f[3], &underlying) || !underlying || underlying > INT_MAX ||
        !UmiIbkrText(f[4], sizeof row->tradingClass, false) ||
        !UmiIbkrText(f[5], sizeof row->multiplier, false) || !UmiIbkrUnsigned(f[6], &expiries))
        return UMI_STATUS_PARSE_ERROR;
    if (expiries > UMI_IBKR_OPTION_EXPIRY_LIMIT)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    size_t strikeCountIndex = 7U + (size_t)expiries;
    if (count <= strikeCountIndex || !UmiIbkrUnsigned(f[strikeCountIndex], &strikes))
        return UMI_STATUS_PARSE_ERROR;
    if (strikes > UMI_IBKR_OPTION_STRIKE_LIMIT)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (count != strikeCountIndex + 1U + (size_t)strikes)
        return UMI_STATUS_PARSE_ERROR;
    row->underlyingContractId = (uint32_t)underlying;
    strcpy(row->exchange, f[2]);
    strcpy(row->tradingClass, f[4]);
    strcpy(row->multiplier, f[5]);
    row->expiryCount = (size_t)expiries;
    row->strikeCount = (size_t)strikes;
    for (size_t i = 0; i < row->expiryCount; ++i)
    {
        if (!UmiIbkrDiscoveryDate(f[7U + i]))
            return UMI_STATUS_PARSE_ERROR;
        strcpy(row->expirations[i], f[7U + i]);
        for (size_t j = 0; j < i; ++j)
            if (!strcmp(row->expirations[i], row->expirations[j]))
                return UMI_STATUS_PARSE_ERROR;
    }
    for (size_t i = 0; i < row->strikeCount; ++i)
    {
        if (!UmiIbkrOrderNumberRead(f[strikeCountIndex + 1U + i], false, false, &row->strikes[i]))
            return UMI_STATUS_PARSE_ERROR;
        for (size_t j = 0; j < i; ++j)
        {
            int comparison = 1;
            if (!strcmp(row->strikes[i].reportedText, row->strikes[j].reportedText))
                return UMI_STATUS_PARSE_ERROR;
            if (row->strikes[i].exact && row->strikes[j].exact &&
                UmiDecimalCompare(row->strikes[i].value, row->strikes[j].value, &comparison) ==
                    UMI_STATUS_OK &&
                comparison == 0)
                return UMI_STATUS_PARSE_ERROR;
        }
    }
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrOptionChainFrame(UmiIbkrConnection *c, uint64_t message, const unsigned char *body,
                                  size_t length, uint64_t now)
{
    UmiIbkrObservationFields wire = {0};
    UmiStatus status = UmiIbkrObservationFieldsOpen(
        body, length, 8U + UMI_IBKR_OPTION_EXPIRY_LIMIT + UMI_IBKR_OPTION_STRIKE_LIMIT, &wire);
    if (status != UMI_STATUS_OK)
        return status;
    uint64_t request = 0U;
    if (wire.count < 2U || !UmiIbkrUnsigned(wire.values[1], &request))
        status = UMI_STATUS_PARSE_ERROR;
    UmiIbkrOptionChainStore *s = c->optionChains;
    if (status == UMI_STATUS_OK &&
        (!s || request != s->snapshot.requestId || !UmiIbkrOptionChainPending(c, now) ||
         c->snapshot.state != UMI_IBKR_READY))
    {
        UmiIbkrObservationFieldsClose(&wire);
        return UmiIbkrObservationIgnore(c);
    }
    if (status == UMI_STATUS_OK && message == 76U)
    {
        if (wire.count != 2U)
            status = UMI_STATUS_PARSE_ERROR;
        else
        {
            s->snapshot.complete = true;
            s->snapshot.completedAtMilliseconds = now;
            strcpy(s->snapshot.message,
                   "Option definitions complete. Expiry and strike pairs still require contract resolution.");
        }
    }
    else if (status == UMI_STATUS_OK)
    {
        UmiIbkrOptionChain *row = calloc(1, sizeof *row);
        if (!row)
            status = UMI_STATUS_OUT_OF_MEMORY;
        if (status == UMI_STATUS_OK)
            status = ReadChain(wire.values, wire.count, row);
        if (status == UMI_STATUS_OK && row->underlyingContractId != s->snapshot.query.underlyingContractId)
            status = UMI_STATUS_PARSE_ERROR;
        size_t index = 0U;
        while (status == UMI_STATUS_OK && index < s->snapshot.count &&
               (strcmp(row->exchange, s->items[index]->exchange) ||
                strcmp(row->tradingClass, s->items[index]->tradingClass) ||
                strcmp(row->multiplier, s->items[index]->multiplier)))
            ++index;
        if (status == UMI_STATUS_OK && index < s->snapshot.count)
            status = UMI_STATUS_PARSE_ERROR;
        if (status == UMI_STATUS_OK && index == UMI_IBKR_OPTION_CHAIN_LIMIT)
            status = UMI_STATUS_CAPACITY_EXCEEDED;
        if (status == UMI_STATUS_OK)
        {
            /* A complete record moves into the store. On every failure the temporary
             * is released and earlier records remain unchanged for inspection. */
            s->items[index] = row;
            ++s->snapshot.count;
            row = NULL;
        }
        free(row);
    }
    UmiIbkrObservationFieldsClose(&wire);
    return status;
}
