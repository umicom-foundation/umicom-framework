/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/execution_decode.c
 * PURPOSE: Decode correlated recent execution reports using bounded legacy wire fields.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include <limits.h>
#include <stdlib.h>
#include <string.h>
static UmiStatus ExecutionIgnored(UmiIbkrConnection *c)
{
    if (c->snapshot.ignoredFrames == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    ++c->snapshot.ignoredFrames;
    return UMI_STATUS_OK;
}
static bool ExecutionSigned(const char *text, int32_t *out)
{
    bool negative = text[0] == '-';
    uint64_t magnitude;
    if (!UmiIbkrUnsigned(text + (negative ? 1U : 0U), &magnitude) ||
        magnitude > (negative ? UINT64_C(2147483648) : INT32_MAX))
        return false;
    *out =
        negative ? (magnitude == UINT64_C(2147483648) ? INT32_MIN : -(int32_t)magnitude) : (int32_t)magnitude;
    return true;
}
static UmiStatus ExecutionRow(char **f, size_t count, UmiIbkrExecutionObservation *out)
{
    if (count != 31U)
        return UMI_STATUS_PARSE_ERROR;
    UmiIbkrExecutionObservation v = {0};
    uint64_t con, perm, flag;
    if (!UmiIbkrUnsigned(f[3], &con) || con == 0U || con > INT_MAX || !UmiIbkrUnsigned(f[21], &perm) ||
        perm > INT64_MAX || !ExecutionSigned(f[2], &v.orderId) || !ExecutionSigned(f[22], &v.clientId) ||
        !UmiIbkrUnsigned(f[23], &flag) || flag > 1U || !UmiIbkrUnsigned(f[30], &flag) || flag > 4U)
        return UMI_STATUS_PARSE_ERROR;
    v.contractId = (uint32_t)con;
    v.permanentOrderId = perm;
    char *dest[] = {v.symbol,  v.securityType, v.currency, v.executionId,   v.time,
                    v.account, v.exchange,     v.side,     v.orderReference};
    const size_t cap[] = {sizeof v.symbol,      sizeof v.securityType, sizeof v.currency,
                          sizeof v.executionId, sizeof v.time,         sizeof v.account,
                          sizeof v.exchange,    sizeof v.side,         sizeof v.orderReference};
    const size_t field[] = {4U, 5U, 11U, 14U, 15U, 16U, 17U, 18U, 26U};
    for (size_t i = 0U; i < sizeof field / sizeof field[0]; ++i)
    {
        if (!UmiIbkrText(f[field[i]], cap[i], true))
            return UMI_STATUS_PARSE_ERROR;
        strcpy(dest[i], f[field[i]]);
    }
    /* Unprojected descriptive fields are validated too. They must not smuggle
     * invalid text through an otherwise valid capture. No locale parsing occurs. */
    for (size_t i = 4U; i < count; ++i)
        if (!UmiIbkrText(f[i], 256U, true))
            return UMI_STATUS_PARSE_ERROR;
    if (!UmiIbkrDecimalText(f[7]) || !UmiIbkrDecimalText(f[28]))
        return UMI_STATUS_PARSE_ERROR;
    if (UmiDecimalParseScientificExact(f[19], strlen(f[19]), &v.quantity) != UMI_STATUS_OK ||
        UmiDecimalParseScientificExact(f[20], strlen(f[20]), &v.price) != UMI_STATUS_OK ||
        UmiDecimalParseScientificExact(f[24], strlen(f[24]), &v.cumulativeQuantity) != UMI_STATUS_OK ||
        UmiDecimalParseScientificExact(f[25], strlen(f[25]), &v.averagePrice) != UMI_STATUS_OK ||
        !UmiIbkrExecutionObservationValid(&v))
        return UMI_STATUS_PARSE_ERROR;
    *out = v;
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrExecutionsFrame(UmiIbkrConnection *c, uint64_t kind, const unsigned char *body,
                                 size_t length, uint64_t now)
{
    if (length == 0U || length > UMI_IBKR_FRAME_LIMIT || body[length - 1U] != 0U)
        return UMI_STATUS_PARSE_ERROR;
    char *copy = malloc(length), *fields[32];
    if (copy == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    memcpy(copy, body, length);
    size_t count = 0U, start = 0U;
    UmiStatus status = UMI_STATUS_OK;
    for (size_t i = 0U; i < length; ++i)
        if (copy[i] == '\0')
        {
            if (count == 32U)
            {
                status = UMI_STATUS_CAPACITY_EXCEEDED;
                goto done;
            }
            fields[count++] = copy + start;
            start = i + 1U;
        }
    size_t requestField = kind == 55U ? 2U : 1U;
    if (count <= requestField)
    {
        status = UMI_STATUS_PARSE_ERROR;
        goto done;
    }
    /* Unsolicited executions have no requested account capture. Do not mix them
     * into this finite review or infer completion from asynchronous arrivals. */
    if (strcmp(fields[requestField], "-1") == 0)
    {
        status = ExecutionIgnored(c);
        goto done;
    }
    uint64_t request;
    if (!UmiIbkrUnsigned(fields[requestField], &request) || request > UINT32_MAX)
    {
        status = UMI_STATUS_PARSE_ERROR;
        goto done;
    }
    if (!UmiIbkrExecutionsAccepting(c, (uint32_t)request, now))
    {
        status = ExecutionIgnored(c);
        goto done;
    }
    UmiIbkrExecutionSnapshot *capture = &c->executions;
    if (capture->revision == UINT64_MAX)
    {
        status = UMI_STATUS_CAPACITY_EXCEEDED;
        goto done;
    }
    if (kind == 55U)
    {
        if (count != 3U || strcmp(fields[1], "1") != 0)
        {
            status = UMI_STATUS_PARSE_ERROR;
            goto done;
        }
        capture->complete = true;
        ++capture->revision;
        strcpy(capture->message,
               "Recent execution end marker received. Visibility depends on TWS and API client settings.");
        goto done;
    }
    if (kind != 11U)
    {
        status = UMI_STATUS_INVALID_ARGUMENT;
        goto done;
    }
    UmiIbkrExecutionObservation row;
    status = ExecutionRow(fields, count, &row);
    if (status != UMI_STATUS_OK)
        goto done;
    /* Accepted wire identities advance the shared conservative floor even if
     * the requested account filter omits the execution from its finite view. */
    UmiIbkrOrdersObserveId(c, row.orderId);
    if (strcmp(row.account, capture->account) != 0)
    {
        status = ExecutionIgnored(c);
        goto done;
    }
    for (size_t i = 0U; i < capture->count; ++i)
    {
        if (strcmp(row.executionId, capture->rows[i].executionId) != 0)
            continue;
        if (!UmiIbkrExecutionObservationEqual(&row, &capture->rows[i]))
        {
            /* An identical ID with different business content is ambiguous,
             * unlike a documented correction with a distinct execution ID. */
            status = UMI_STATUS_PARSE_ERROR;
            goto done;
        }
        if (capture->duplicateCount == UINT64_MAX)
        {
            status = UMI_STATUS_CAPACITY_EXCEEDED;
            goto done;
        }
        ++capture->duplicateCount;
        ++capture->revision;
        goto done;
    }
    if (capture->count == UMI_IBKR_EXECUTION_OBSERVATION_LIMIT)
    {
        status = UMI_STATUS_CAPACITY_EXCEEDED;
        goto done;
    }
    capture->rows[capture->count++] = row;
    ++capture->revision;
done:
    free(copy);
    return status;
}
