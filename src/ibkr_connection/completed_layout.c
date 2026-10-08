/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/completed_layout.c
 * PURPOSE: Walk legacy completed-order layouts without guessing offsets after variable sections.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include "order_numbers.h"
#include <limits.h>
#include <string.h>

typedef struct Cursor
{
    char **fields;
    size_t count, at;
    UmiStatus status;
} Cursor;
static const char *Take(Cursor *c)
{
    if (c->status != UMI_STATUS_OK)
        return "";
    if (c->at == c->count)
    {
        c->status = UMI_STATUS_PARSE_ERROR;
        return "";
    }
    return c->fields[c->at++];
}
static void Skip(Cursor *c, size_t n)
{
    if (c->status != UMI_STATUS_OK)
        return;
    if (n > c->count - c->at)
    {
        c->status = UMI_STATUS_PARSE_ERROR;
        return;
    }
    c->at += n;
}
static uint64_t Unsigned(Cursor *c, uint64_t maximum)
{
    const char *text = Take(c);
    uint64_t value = 0U;
    if (c->status == UMI_STATUS_OK && (!UmiIbkrUnsigned(text, &value) || value > maximum))
        c->status = UMI_STATUS_PARSE_ERROR;
    return value;
}
static bool Flag(Cursor *c) { return Unsigned(c, 1U) == 1U; }
static void Text(Cursor *c, char *out, size_t capacity, bool optional)
{
    const char *text = Take(c);
    if (c->status != UMI_STATUS_OK)
        return;
    if (!UmiIbkrText(text, capacity, optional))
    {
        c->status = UMI_STATUS_PARSE_ERROR;
        return;
    }
    strcpy(out, text);
}
static void Number(Cursor *c, bool optional, bool nonnegative, UmiIbkrOrderNumber *out)
{
    const char *text = Take(c);
    if (c->status == UMI_STATUS_OK && !UmiIbkrOrderNumberRead(text, optional, nonnegative, out))
        c->status = UMI_STATUS_PARSE_ERROR;
}
static size_t Repeated(Cursor *c, size_t width, size_t limit)
{
    uint64_t n = Unsigned(c, limit);
    if (c->status == UMI_STATUS_OK)
        Skip(c, (size_t)n * width);
    return (size_t)n;
}
static void Conditions(Cursor *c, size_t *outCount)
{
    uint64_t count = Unsigned(c, 32U);
    *outCount = (size_t)count;
    for (size_t i = 0U; i < (size_t)count && c->status == UMI_STATUS_OK; ++i)
    {
        uint64_t kind = Unsigned(c, UINT32_MAX);
        const char *connector = Take(c);
        if (strcmp(connector, "a") && strcmp(connector, "o"))
        {
            c->status = UMI_STATUS_PARSE_ERROR;
            return;
        }
        /* Unknown condition types have unknown widths. Refuse them rather than
         * shifting the cursor and publishing unrelated fields as completion. */
        if (kind == 5U)
            Skip(c, 3U); /* Execution: instrument type, exchange, symbol. */
        else if (kind == 1U || kind == 3U || kind == 4U || kind == 6U || kind == 7U)
        {
            (void)Flag(c);
            Skip(c, kind == 1U ? 4U : (kind == 3U || kind == 4U) ? 1U : 3U);
        }
        else
        {
            c->status = UMI_STATUS_NOT_IMPLEMENTED;
            return;
        }
    }
    if (count)
    {
        (void)Flag(c);
        (void)Flag(c);
    }
}
static void Scale(Cursor *c)
{
    Skip(c, 2U); /* Initial/subsequent level sizes; retained in raw evidence. */
    const char *increment = Take(c);
    if (c->status != UMI_STATUS_OK)
        return;
    /* A positive scale increment adds seven fields. Empty/unset adds none.
     * Values outside our exact range are refused when they determine layout;
     * using rounded floating point here could shift every following field. */
    if (!*increment || !strcmp(increment, "1.7976931348623157E308") ||
        !strcmp(increment, "1.7976931348623157E+308") || !strcmp(increment, "1.7976931348623157e+308"))
        return;
    UmiDecimal value;
    if (UmiDecimalParseScientificExact(increment, strlen(increment), &value) != UMI_STATUS_OK)
    {
        c->status = UMI_STATUS_NOT_IMPLEMENTED;
        return;
    }
    if (value.coefficient > 0)
        Skip(c, 7U);
}
UmiStatus UmiIbkrCompletedDecode(char **fields, size_t count, int protocol, UmiIbkrCompletedOrder *out)
{
    if (!fields || !out || protocol < 151 || protocol > 176)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiIbkrCompletedOrder row = {0};
    Cursor c = {fields, count, 1U, UMI_STATUS_OK};
    if (count < 2U)
        return UMI_STATUS_PARSE_ERROR;
    UmiIbkrOpenOrderSummary *o = &row.order;
    o->contractId = (uint32_t)Unsigned(&c, INT32_MAX);
    Text(&c, o->symbol, sizeof o->symbol, true);
    Text(&c, o->securityType, sizeof o->securityType, false);
    Text(&c, o->expiry, sizeof o->expiry, true);
    Number(&c, false, false, &o->strike);
    Text(&c, o->right, sizeof o->right, true);
    Text(&c, o->multiplier, sizeof o->multiplier, true);
    Text(&c, o->exchange, sizeof o->exchange, true);
    Text(&c, o->currency, sizeof o->currency, true);
    Text(&c, o->localSymbol, sizeof o->localSymbol, true);
    Text(&c, o->tradingClass, sizeof o->tradingClass, true);
    Text(&c, o->action, sizeof o->action, false);
    Number(&c, false, true, &o->totalQuantity);
    Text(&c, o->orderType, sizeof o->orderType, false);
    Number(&c, true, false, &o->limitPrice);
    Number(&c, true, false, &o->auxiliaryPrice);
    Text(&c, o->timeInForce, sizeof o->timeInForce, true);
    Text(&c, o->ocaGroup, sizeof o->ocaGroup, true);
    Text(&c, o->account, sizeof o->account, false);
    Text(&c, o->openClose, sizeof o->openClose, true);
    o->origin = (int32_t)Unsigned(&c, INT32_MAX);
    Text(&c, o->orderReference, sizeof o->orderReference, true);
    row.permanentId = Unsigned(&c, INT64_MAX);
    if (!row.permanentId)
        return UMI_STATUS_PARSE_ERROR;
    row.outsideRegularHours = Flag(&c);
    row.hidden = Flag(&c);
    Skip(&c, 6U); /* Discretionary amount, good-after time, four advisor fields. */
    Text(&c, row.modelCode, sizeof row.modelCode, true);
    Skip(&c, 13U); /* Dates, settlement, short sale, box/pegged prices, display size. */
    row.sweepToFill = Flag(&c);
    row.allOrNone = Flag(&c);
    Number(&c, true, true, &row.minimumQuantity);
    if (!strcmp(row.minimumQuantity.reportedText, "2147483647"))
        row.minimumQuantity.exact = false;
    Skip(&c, 4U); /* OCA type, trigger method, volatility, volatility type. */
    const char *neutralType = Take(&c);
    Skip(&c, 1U);
    if (*neutralType)
        Skip(&c, 4U); /* Completed layout omits open-order clearing extras. */
    Skip(&c, 5U);     /* Continuous update, reference type, two trailing values, leg description. */
    row.comboLegCount = Repeated(&c, 8U, 32U);
    (void)Repeated(&c, 1U, 32U); /* Optional per-leg prices. */
    (void)Repeated(&c, 2U, 64U); /* SMART routing tag/value pairs. */
    Scale(&c);
    const char *hedge = Take(&c);
    if (*hedge)
        Skip(&c, 1U);
    Skip(&c, 2U);
    (void)Flag(&c); /* Clearing account/intent, not-held. */
    if (Flag(&c))
        Skip(&c, 3U); /* Delta-neutral contract. */
    const char *algorithm = Take(&c);
    if (*algorithm)
        (void)Repeated(&c, 2U, 64U);
    (void)Flag(&c); /* Solicited flag. */
    Text(&c, row.status, sizeof row.status, false);
    (void)Flag(&c);
    (void)Flag(&c); /* Size and price randomisation. */
    if (!strcmp(o->orderType, "PEG BENCH") || !strcmp(o->orderType, "PEGBENCH"))
        Skip(&c, 5U);
    Conditions(&c, &row.conditionCount);
    Skip(&c, 2U); /* Stop price and limit offset; not the earlier ticket prices. */
    Number(&c, true, true, &row.cashQuantity);
    (void)Flag(&c);
    (void)Flag(&c); /* Auto-price hedge and OMS container. */
    Text(&c, row.autoCancelDate, sizeof row.autoCancelDate, true);
    Number(&c, false, true, &row.filledQuantity);
    (void)Unsigned(&c, INT32_MAX); /* Reference futures contract. */
    (void)Flag(&c);
    Skip(&c, 1U); /* Shareholder text remains in raw evidence. */
    (void)Flag(&c);
    (void)Flag(&c);
    row.parentPermanentId = Unsigned(&c, INT64_MAX);
    Text(&c, row.completedTime, sizeof row.completedTime, true);
    Text(&c, row.completedStatus, sizeof row.completedStatus, false);
    if (protocol >= 170)
        Skip(&c, 5U); /* Peg-best/mid offsets introduced by the protocol. */
    if (c.status != UMI_STATUS_OK)
        return c.status;
    if (c.at != count)
        return UMI_STATUS_PARSE_ERROR;
    o->wireFieldCount = count;
    *out = row;
    return UMI_STATUS_OK;
}
