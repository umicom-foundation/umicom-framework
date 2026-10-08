/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/order_decode.c
 * PURPOSE: Decode bounded open-order prefixes and complete legacy order-status callbacks.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include "observation_wire.h"
#include "order_numbers.h"
#include <limits.h>
#include <stdio.h>
#include <string.h>

static bool Signed(const char *text, int32_t *out)
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
/* The original numeric reader is retained for review. Open and completed
 * order streams now share the same exact/raw interpretation, so extending the
 * supported numeric range cannot give the two reports different meanings. */
#if 0
static bool Number(const char *text, bool optional, bool nonnegative, UmiIbkrOrderNumber *out)
{
    if (!UmiIbkrText(text, sizeof out->reportedText, optional))
        return false;
    if (!*text)
        return true;
    if (!UmiIbkrDecimalText(text) || (nonnegative && text[0] == '-'))
        return false;
    strcpy(out->reportedText, text);
    /* Unset double sentinels and extra precision stay visible as raw text.
     * Exact consumers must check the flag before using the decimal member. */
    out->exact = UmiDecimalParseScientificExact(text, strlen(text), &out->value) == UMI_STATUS_OK;
    return true;
}
#endif
static bool Number(const char *text, bool optional, bool nonnegative, UmiIbkrOrderNumber *out)
{
    return UmiIbkrOrderNumberRead(text,optional,nonnegative,out);
}
static UmiStatus OpenOrder(char **f, size_t count, UmiIbkrRecoveredOrder *out)
{
    /* Protocol 151..176 has no message-version field here. Only the stable
     * prefix through permId is interpreted. The remaining order/OrderState
     * fields are retained as opaque evidence, not silently treated as defaults. */
    if (count <= 26U)
        return UMI_STATUS_PARSE_ERROR;
    uint64_t con, permanent;
    if (!Signed(f[1], &out->key.orderId) || !UmiIbkrUnsigned(f[2], &con) || con > INT32_MAX ||
        !Signed(f[22], &out->open.origin) || out->open.origin < 0 || !Signed(f[24], &out->key.clientId) ||
        out->key.clientId < 0 || !UmiIbkrUnsigned(f[25], &permanent) || permanent > INT64_MAX)
        return UMI_STATUS_PARSE_ERROR;
    out->open.contractId = (uint32_t)con;
    out->permanentId = permanent;
    char *dest[] = {
        out->open.symbol,       out->open.securityType, out->open.expiry,    out->open.right,
        out->open.multiplier,   out->open.exchange,     out->open.currency,  out->open.localSymbol,
        out->open.tradingClass, out->open.action,       out->open.orderType, out->open.timeInForce,
        out->open.ocaGroup,     out->open.account,      out->open.openClose, out->open.orderReference};
    const size_t caps[] = {64U, 16U, 32U, 8U, 32U, 64U, 16U, 64U, 64U, 16U, 32U, 16U, 128U, 64U, 8U, 256U};
    const size_t indices[] = {3U, 4U, 5U, 7U, 8U, 9U, 10U, 11U, 12U, 13U, 15U, 18U, 19U, 20U, 21U, 23U};
    for (size_t i = 0U; i < sizeof indices / sizeof indices[0]; ++i)
    {
        if (!UmiIbkrText(f[indices[i]], caps[i], true))
            return UMI_STATUS_PARSE_ERROR;
        strcpy(dest[i], f[indices[i]]);
    }
    if (!Number(f[6], false, false, &out->open.strike) ||
        !Number(f[14], false, true, &out->open.totalQuantity) ||
        !Number(f[16], true, false, &out->open.limitPrice) ||
        !Number(f[17], true, false, &out->open.auxiliaryPrice))
        return UMI_STATUS_PARSE_ERROR;
    out->open.wireFieldCount = count;
    out->hasOpenOrder = true;
    return UMI_STATUS_OK;
}
static UmiStatus StatusOrder(char **f, size_t count, UmiIbkrRecoveredOrder *out)
{
    if (count != 12U)
        return UMI_STATUS_PARSE_ERROR;
    uint64_t permanent;
    if (!Signed(f[1], &out->key.orderId) || !UmiIbkrText(f[2], sizeof out->status.status, false) ||
        !Number(f[3], false, true, &out->status.filled) ||
        !Number(f[4], false, true, &out->status.remaining) ||
        !Number(f[5], false, false, &out->status.averageFillPrice) || !UmiIbkrUnsigned(f[6], &permanent) ||
        permanent > INT64_MAX || !Signed(f[7], &out->status.parentId) ||
        !Number(f[8], false, false, &out->status.lastFillPrice) || !Signed(f[9], &out->key.clientId) ||
        out->key.clientId < 0 || !UmiIbkrText(f[10], sizeof out->status.whyHeld, true) ||
        !Number(f[11], true, false, &out->status.marketCapPrice))
        return UMI_STATUS_PARSE_ERROR;
    out->permanentId = permanent;
    strcpy(out->status.status, f[2]);
    strcpy(out->status.whyHeld, f[10]);
    out->hasStatus = true;
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrOrdersFrame(UmiIbkrConnection *c, uint64_t kind, const unsigned char *body, size_t length,
                             uint64_t now)
{
    UmiIbkrObservationFields fields = {0};
    UmiStatus status = UmiIbkrObservationFieldsOpen(body, length, 1024U, &fields);
    if (status != UMI_STATUS_OK)
        return status;
    UmiIbkrOrdersExpire(c, now);
    if (kind == 53U)
    {
        if (fields.count != 2U || strcmp(fields.values[1], "1"))
            status = UMI_STATUS_PARSE_ERROR;
        else if (!c->orders.snapshot.pending)
            status = UmiIbkrObservationIgnore(c);
        else
        {
            c->orders.snapshot.pending = false;
            c->orders.snapshot.complete = true;
            c->orders.snapshot.stale = false;
            c->orders.snapshot.completedAtMilliseconds = now;
            (void)snprintf(c->orders.snapshot.message, sizeof c->orders.snapshot.message,
                           "Open-order end marker received. This is a point-in-time recovery, not a "
                           "subscription or execution history.");
        }
    }
    else
    {
        UmiIbkrRecoveredOrder row = {0};
        status = kind == 5U   ? OpenOrder(fields.values, fields.count, &row)
                 : kind == 3U ? StatusOrder(fields.values, fields.count, &row)
                              : UMI_STATUS_INVALID_ARGUMENT;
        if (status == UMI_STATUS_OK)
            status = UmiIbkrOrdersStore(c, &row, kind == 5U ? body : NULL, kind == 5U ? length : 0U, now);
    }
    UmiIbkrObservationFieldsClose(&fields);
    return status;
}
