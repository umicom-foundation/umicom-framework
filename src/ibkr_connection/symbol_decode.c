/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/symbol_decode.c
 * PURPOSE: Decode an entire symbol-candidate response before publishing any row.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "observation_wire.h"
#include <limits.h>
#include <stdlib.h>
#include <string.h>
static UmiStatus SearchText(UmiIbkrObservationFields *fields, size_t *at, char *out, size_t capacity,
                            bool empty)
{
    if (*at >= fields->count || !UmiIbkrText(fields->values[*at], capacity, empty))
        return UMI_STATUS_PARSE_ERROR;
    strcpy(out, fields->values[(*at)++]);
    return UMI_STATUS_OK;
}
static UmiStatus SearchUnsigned(UmiIbkrObservationFields *fields, size_t *at, uint64_t maximum, uint64_t *out)
{
    if (*at >= fields->count || !UmiIbkrUnsigned(fields->values[(*at)++], out) || *out > maximum)
        return UMI_STATUS_PARSE_ERROR;
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrSymbolSearchFrame(UmiIbkrConnection *c, const unsigned char *body, size_t length,
                                   uint64_t now)
{
    UmiIbkrObservationFields fields = {0};
    UmiIbkrSymbolSearchSnapshot *candidate = NULL;
    UmiStatus status = UmiIbkrObservationFieldsOpen(body, length, 512U, &fields);
    if (status != UMI_STATUS_OK)
        return status;
    size_t at = 1U;
    uint64_t request = 0U, count = 0U;
    status = SearchUnsigned(&fields, &at, INT_MAX, &request);
    if (status != UMI_STATUS_OK)
        goto done;
    if (!UmiIbkrSymbolSearchAccepting(c, (uint32_t)request, now))
    {
        status = UmiIbkrObservationIgnore(c);
        goto done;
    }
    status = SearchUnsigned(&fields, &at, UMI_IBKR_SYMBOL_LIMIT, &count);
    if (status != UMI_STATUS_OK)
        goto done;
    candidate = malloc(sizeof *candidate);
    if (candidate == NULL)
    {
        status = UMI_STATUS_OUT_OF_MEMORY;
        goto done;
    }
    *candidate = c->symbolSearch;
    candidate->count = (size_t)count;
    for (size_t i = 0U; i < candidate->count; ++i)
    {
        UmiIbkrSymbolCandidate *row = &candidate->rows[i];
        uint64_t value;
        status = SearchUnsigned(&fields, &at, INT_MAX, &value);
        if (status != UMI_STATUS_OK || value == 0U)
        {
            status = UMI_STATUS_PARSE_ERROR;
            goto done;
        }
        row->contractId = (uint32_t)value;
        status = SearchText(&fields, &at, row->symbol, sizeof row->symbol, false);
        if (status == UMI_STATUS_OK)
            status = SearchText(&fields, &at, row->securityType, sizeof row->securityType, false);
        if (status == UMI_STATUS_OK)
            status = SearchText(&fields, &at, row->primaryExchange, sizeof row->primaryExchange, true);
        if (status == UMI_STATUS_OK)
            status = SearchText(&fields, &at, row->currency, sizeof row->currency, true);
        if (status == UMI_STATUS_OK)
            status = SearchUnsigned(&fields, &at, UMI_IBKR_DERIVATIVE_TYPE_LIMIT, &value);
        if (status != UMI_STATUS_OK)
            goto done;
        row->derivativeTypeCount = (size_t)value;
        for (size_t j = 0U; j < row->derivativeTypeCount; ++j)
        {
            status = SearchText(&fields, &at, row->derivativeTypes[j], sizeof row->derivativeTypes[j], false);
            if (status != UMI_STATUS_OK)
                goto done;
        }
        /* Description and issuer were appended at negotiated protocol 176.
         * Earlier packets end at derivative types; never guess missing fields. */
        if (c->snapshot.protocolVersion >= 176)
        {
            status = SearchText(&fields, &at, row->description, sizeof row->description, true);
            if (status == UMI_STATUS_OK)
                status = SearchText(&fields, &at, row->issuerId, sizeof row->issuerId, true);
            if (status != UMI_STATUS_OK)
                goto done;
        }
    }
    if (at != fields.count)
    {
        status = UMI_STATUS_PARSE_ERROR;
        goto done;
    }
    candidate->complete = true;
    candidate->receivedAtMilliseconds = now;
    strcpy(candidate->message, "Contract candidates received. Review identity and route before using one.");
    c->symbolSearch = *candidate;
done:
    free(candidate);
    UmiIbkrObservationFieldsClose(&fields);
    return status;
}
