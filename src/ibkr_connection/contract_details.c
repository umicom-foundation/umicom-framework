/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/contract_details.c
 * PURPOSE: Own bounded exact-contract metadata requests and preserve route uncertainty.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static bool DetailsPending(const UmiIbkrConnection *connection, uint64_t now)
{
    const UmiIbkrContractDetailsSnapshot *details = &connection->contractDetails;
    return details->requestId != 0U && !details->complete && !details->failed &&
           now >= details->requestedAtMilliseconds &&
           now - details->requestedAtMilliseconds < connection->options.timeoutMilliseconds;
}
UmiStatus UmiIbkrContractDetailsRequest(UmiIbkrConnection *connection, const UmiIbkrQuoteContract *contract,
                                        uint64_t now, uint32_t *outRequest)
{
    if (connection == NULL || contract == NULL || outRequest == NULL || contract->contractId == 0U ||
        contract->contractId > INT_MAX ||
        !UmiIbkrText(contract->exchange, sizeof contract->exchange, false) || now < connection->lastNow)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (connection->snapshot.state != UMI_IBKR_READY)
        return UMI_STATUS_INVALID_STATE;
    if (connection->snapshot.protocolVersion < 164 || connection->snapshot.protocolVersion > 176)
        return UMI_STATUS_NOT_IMPLEMENTED;
    if (DetailsPending(connection, now))
        return UMI_STATUS_BUSY;
    uint32_t request = connection->nextQuoteRequest == 0U ? 36000U : connection->nextQuoteRequest;
    if (request >= INT_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    char requestText[24], contractText[24];
    (void)snprintf(requestText, sizeof requestText, "%u", (unsigned)request);
    (void)snprintf(contractText, sizeof contractText, "%u", (unsigned)contract->contractId);
    /* Only identity and route are supplied. The empty descriptive fields let
     * TWS resolve its canonical instrument without inventing a symbol match. */
    const char *fields[] = {"9", "8", requestText, contractText, "",  "", "", "0", "", "", contract->exchange,
                            "",  "",  "",          "",           "0", "", "", ""};
    size_t count = connection->snapshot.protocolVersion >= 176 ? 19U : 18U;
    UmiStatus status = UmiIbkrQueueFields(connection, fields, count);
    if (status != UMI_STATUS_OK)
        return status;
    UmiIbkrContractDetailsSnapshot *details = &connection->contractDetails;
    memset(details, 0, sizeof *details);
    details->requestId = request;
    details->requestedContract = *contract;
    details->requestedAtMilliseconds = now;
    strcpy(details->message, "Contract description requested; waiting for the provider's end marker.");
    connection->nextQuoteRequest = request + 1U;
    connection->lastNow = now;
    *outRequest = request;
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrContractDetailsCopy(const UmiIbkrConnection *connection, uint32_t request, uint64_t now,
                                     UmiIbkrContractDetailsSnapshot *out)
{
    if (connection == NULL || out == NULL || request == 0U || now < connection->lastNow)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (connection->contractDetails.requestId != request)
        return UMI_STATUS_NOT_FOUND;
    *out = connection->contractDetails;
    if (!out->complete && !out->failed &&
        now - out->requestedAtMilliseconds >= connection->options.timeoutMilliseconds)
    {
        out->failed = true;
        strcpy(out->message, "Contract lookup timed out; partial metadata is not complete.");
    }
    out->stale = out->failed || connection->snapshot.state != UMI_IBKR_READY;
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrContractDetailsAbandon(UmiIbkrConnection *connection, uint32_t request)
{
    if (connection == NULL || request == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (connection->contractDetails.requestId != request)
        return UMI_STATUS_NOT_FOUND;
    connection->contractDetails.failed = true;
    strcpy(connection->contractDetails.message, "Local contract review abandoned; no order was cancelled.");
    return UMI_STATUS_OK;
}
static UmiStatus DetailsIgnored(UmiIbkrConnection *connection)
{
    if (connection->snapshot.ignoredFrames == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    ++connection->snapshot.ignoredFrames;
    return UMI_STATUS_OK;
}
static UmiStatus DetailsFields(const unsigned char *body, size_t length, char **fields, size_t *count,
                               char *copy)
{
    if (length == 0U || body[length - 1U] != 0U)
        return UMI_STATUS_PARSE_ERROR;
    memcpy(copy, body, length);
    size_t start = 0U, used = 0U;
    for (size_t index = 0U; index < length; ++index)
        if (copy[index] == '\0')
        {
            if (used == 128U)
                return UMI_STATUS_CAPACITY_EXCEEDED;
            fields[used++] = copy + start;
            start = index + 1U;
        }
    *count = used;
    return UMI_STATUS_OK;
}
static UmiStatus Description(char **fields, size_t count, UmiIbkrContractDescription *out)
{
    uint64_t pairs, id;
    if (count < 40U || !UmiIbkrUnsigned(fields[30], &pairs) || pairs > 16U ||
        count != 40U + (size_t)pairs * 2U || !UmiIbkrUnsigned(fields[12], &id) || id == 0U || id > INT_MAX)
        return UMI_STATUS_PARSE_ERROR;
    /* Validate skipped metadata too. Optional empty fields remain unknown;
     * control text and malformed UTF-8 cannot cross the presentation boundary. */
    for (size_t index = 2U; index < count; ++index)
        if (!UmiIbkrText(fields[index], 4096U, true))
            return UMI_STATUS_PARSE_ERROR;
    if (!UmiIbkrDecimalText(fields[5]) || !UmiIbkrDecimalText(fields[13]) || !UmiIbkrDecimalText(fields[17]))
        return UMI_STATUS_PARSE_ERROR;
    UmiIbkrContractDescription row = {0};
    row.contractId = (uint32_t)id;
    const size_t tail = 31U + (size_t)pairs * 2U;
    char *dest[] = {row.symbol,       row.securityType,    row.expiry,
                    row.strike,       row.right,           row.exchange,
                    row.currency,     row.localSymbol,     row.marketName,
                    row.tradingClass, row.minimumTick,     row.multiplier,
                    row.orderTypes,   row.validExchanges,  row.priceMagnifier,
                    row.longName,     row.primaryExchange, row.marketRuleIds,
                    row.minimumSize,  row.sizeIncrement,   row.suggestedSizeIncrement};
    const size_t capacities[] = {
        sizeof row.symbol,       sizeof row.securityType,    sizeof row.expiry,
        sizeof row.strike,       sizeof row.right,           sizeof row.exchange,
        sizeof row.currency,     sizeof row.localSymbol,     sizeof row.marketName,
        sizeof row.tradingClass, sizeof row.minimumTick,     sizeof row.multiplier,
        sizeof row.orderTypes,   sizeof row.validExchanges,  sizeof row.priceMagnifier,
        sizeof row.longName,     sizeof row.primaryExchange, sizeof row.marketRuleIds,
        sizeof row.minimumSize,  sizeof row.sizeIncrement,   sizeof row.suggestedSizeIncrement};
    const size_t indexes[] = {2U,  3U,  4U,  5U,  6U,  7U,  8U,        9U,        10U,       11U,      13U,
                              14U, 15U, 16U, 17U, 19U, 20U, tail + 3U, tail + 6U, tail + 7U, tail + 8U};
    for (size_t index = 0U; index < sizeof indexes / sizeof indexes[0]; ++index)
    {
        size_t bytes = strlen(fields[indexes[index]]) + 1U;
        if (bytes > capacities[index])
            return UMI_STATUS_CAPACITY_EXCEEDED;
        memcpy(dest[index], fields[indexes[index]], bytes);
    }
    if (row.symbol[0] == '\0' || row.securityType[0] == '\0' || row.exchange[0] == '\0')
        return UMI_STATUS_PARSE_ERROR;
    *out = row;
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrContractDetailsFrame(UmiIbkrConnection *connection, uint64_t message,
                                      const unsigned char *body, size_t length, uint64_t now)
{
    if (connection->snapshot.protocolVersion < 164 || connection->snapshot.protocolVersion > 176)
        return DetailsIgnored(connection);
    char *copy = malloc(length);
    if (copy == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    char *fields[128];
    size_t count = 0U;
    UmiStatus status = DetailsFields(body, length, fields, &count, copy);
    uint64_t request = 0U;
    size_t identity = message == 52U ? 2U : 1U;
    if (status == UMI_STATUS_OK && (count <= identity || !UmiIbkrUnsigned(fields[identity], &request)))
        status = UMI_STATUS_PARSE_ERROR;
    UmiIbkrContractDetailsSnapshot *details = &connection->contractDetails;
    if (status == UMI_STATUS_OK && (request != details->requestId || !DetailsPending(connection, now)))
    {
        free(copy);
        return DetailsIgnored(connection);
    }
    if (status == UMI_STATUS_OK && message == 52U)
    {
        if (count != 3U || strcmp(fields[1], "1") != 0)
            status = UMI_STATUS_PARSE_ERROR;
        else
        {
            details->complete = true;
            details->completedAtMilliseconds = now;
            strcpy(details->message,
                   "Provider contract response complete. FOK/AON support remains unconfirmed.");
        }
    }
    else if (status == UMI_STATUS_OK && message == 18U)
    {
        details->failed = true;
        strcpy(details->message, "Bond-specific descriptions are not supported by this inspector.");
    }
    else if (status == UMI_STATUS_OK)
    {
        UmiIbkrContractDescription *row = malloc(sizeof *row);
        if (row == NULL)
            status = UMI_STATUS_OUT_OF_MEMORY;
        else
        {
            status = Description(fields, count, row);
            /* Descriptive option requests have no conId until the broker replies.
             * Verify their complete identity; preserve the exact-ID-only check for review. */
#if 0
            if (status == UMI_STATUS_OK && row->contractId != details->requestedContract.contractId)
                status = UMI_STATUS_PARSE_ERROR;
#endif
            if (status == UMI_STATUS_OK && !UmiIbkrContractIdentityMatches(details,row))
                status = UMI_STATUS_PARSE_ERROR;
            /* The former route/class-only key is retained for review. The active
             * key below also includes conId, preventing ambiguity from being hidden. */
#if 0
                   (strcmp(details->items[index].exchange, row->exchange) != 0 ||
                    strcmp(details->items[index].tradingClass, row->tradingClass) != 0))
#endif
            size_t index = 0U;
            while (status == UMI_STATUS_OK && index < details->count &&
                   /* Preserve each conId so ambiguous option matches remain visible. */
                   (details->items[index].contractId != row->contractId ||
                    strcmp(details->items[index].exchange, row->exchange) != 0 ||
                    strcmp(details->items[index].tradingClass, row->tradingClass) != 0))
                ++index;
            if (status == UMI_STATUS_OK && index == UMI_IBKR_CONTRACT_DETAILS_LIMIT)
                status = UMI_STATUS_CAPACITY_EXCEEDED;
            if (status == UMI_STATUS_OK)
            {
                details->items[index] = *row;
                if (index == details->count)
                    ++details->count;
            }
            free(row);
        }
    }
    free(copy);
    return status;
}
bool UmiIbkrContractDetailsProviderMessage(UmiIbkrConnection *connection, const char *identity, int code,
                                           const char *text)
{
    uint64_t request;
    if (!UmiIbkrUnsigned(identity, &request) || request == 0U ||
        request != connection->contractDetails.requestId)
        return false;
    UmiIbkrContractDetailsSnapshot *details = &connection->contractDetails;
    details->providerCode = code;
    details->failed = true;
    /* Refusal belongs to this lookup. Other observations remain available.
     * Omit overlong diagnostics instead of cutting through a UTF-8 character. */
    if (strlen(text) > 440U)
        (void)snprintf(details->message, sizeof details->message,
                       "IBKR %d: contract diagnostic exceeds the display limit.", code);
    else
        (void)snprintf(details->message, sizeof details->message, "IBKR %d: %s", code, text);
    return true;
}
