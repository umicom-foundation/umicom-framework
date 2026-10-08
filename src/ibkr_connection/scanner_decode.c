/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/scanner_decode.c
 * PURPOSE: Publish each scanner refresh atomically, retaining its broker rank and identity.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "observation_wire.h"
#include "order_numbers.h"
#include <limits.h>
#include <stdlib.h>
#include <string.h>
static UmiStatus ReadRow(char **f, UmiIbkrScannerRow *row)
{
    uint64_t rank, id;
    if (!UmiIbkrUnsigned(f[0], &rank) || rank >= UMI_IBKR_SCANNER_ROW_LIMIT || !UmiIbkrUnsigned(f[1], &id) ||
        !id || id > INT_MAX || !UmiIbkrOrderNumberRead(f[5], false, false, &row->strike))
        return UMI_STATUS_PARSE_ERROR;
    row->rank = (unsigned)rank;
    row->contractId = (uint32_t)id;
    char *dest[] = {row->symbol,    row->securityType, row->expiry,     row->right,        row->exchange,
                    row->currency,  row->localSymbol,  row->marketName, row->tradingClass, row->distance,
                    row->benchmark, row->projection,   row->legs};
    const size_t cap[] = {96, 24, 96, 16, 64, 16, 96, 128, 64, 256, 256, 256, 512};
    const size_t index[] = {2, 3, 4, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    for (size_t i = 0; i < sizeof cap / sizeof cap[0]; ++i)
    {
        if (!UmiIbkrText(f[index[i]], cap[i], true))
            return UMI_STATUS_PARSE_ERROR;
        strcpy(dest[i], f[index[i]]);
    }
    if (!*row->symbol || !*row->securityType || !*row->exchange)
        return UMI_STATUS_PARSE_ERROR;
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrScannerFrame(UmiIbkrConnection *c, const unsigned char *body, size_t length, uint64_t now)
{
    UmiIbkrObservationFields wire = {0};
    UmiStatus status =
        UmiIbkrObservationFieldsOpen(body, length, 4U + 16U * UMI_IBKR_SCANNER_ROW_LIMIT, &wire);
    if (status != UMI_STATUS_OK)
        return status;
    char **f = wire.values;
    uint64_t version = 0U, request = 0U, count = 0U;
    if (wire.count < 4U || !UmiIbkrUnsigned(f[1], &version) || version < 3U || version > 4U ||
        !UmiIbkrUnsigned(f[2], &request) || request > UINT32_MAX || !UmiIbkrUnsigned(f[3], &count))
        status = UMI_STATUS_PARSE_ERROR;
    UmiIbkrScannerStore *s = status == UMI_STATUS_OK ? UmiIbkrScannerFind(c, (uint32_t)request) : NULL;
    if (status == UMI_STATUS_OK && (!s || !s->snapshot.active || c->snapshot.state != UMI_IBKR_READY))
    {
        UmiIbkrObservationFieldsClose(&wire);
        return UmiIbkrObservationIgnore(c);
    }
    if (status == UMI_STATUS_OK &&
        (count > UMI_IBKR_SCANNER_ROW_LIMIT || count > s->snapshot.query.numberOfRows))
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    if (status == UMI_STATUS_OK && wire.count != 4U + 16U * (size_t)count)
        status = UMI_STATUS_PARSE_ERROR;
    UmiIbkrScannerRow *rows = NULL;
    if (status == UMI_STATUS_OK)
    {
        rows = calloc(UMI_IBKR_SCANNER_ROW_LIMIT, sizeof *rows);
        if (!rows)
            status = UMI_STATUS_OUT_OF_MEMORY;
    }
    bool ranks[UMI_IBKR_SCANNER_ROW_LIMIT] = {false};
    for (size_t i = 0; status == UMI_STATUS_OK && i < (size_t)count; ++i)
    {
        UmiIbkrScannerRow row = {0};
        status = ReadRow(f + 4U + 16U * i, &row);
        if (status != UMI_STATUS_OK)
            break;
        if (row.rank >= count || ranks[row.rank])
        {
            status = UMI_STATUS_PARSE_ERROR;
            break;
        }
        ranks[row.rank] = true;
        rows[row.rank] = row;
    }
    if (status == UMI_STATUS_OK && s->snapshot.generation == UINT64_MAX)
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    if (status == UMI_STATUS_OK)
    {
        /* The wire packet ends one refresh. Swap only after every row validates,
         * so a malformed tail cannot publish a mixed generation to the UI. */
        memcpy(s->rows, rows, sizeof s->rows);
        s->snapshot.count = (size_t)count;
        ++s->snapshot.generation;
        s->snapshot.receivedAtMilliseconds = now;
        s->snapshot.stale = false;
        strcpy(s->snapshot.message, count ? "Complete ranked contracts received; request quotes separately."
                                          : "Complete scanner refresh returned no contracts.");
    }
    free(rows);
    UmiIbkrObservationFieldsClose(&wire);
    return status;
}
