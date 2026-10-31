/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/order_wire.c
 *
 * PURPOSE:
 *   Encode bounded reviewed limit orders and individual cancellations without opening the read-only queue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include <stdio.h>
#include <string.h>

/* This encoder is private to the reviewed ticket service. The general read-only
 * queue continues to reject order message IDs. Build a whole frame locally before
 * appending, so capacity failures cannot publish half an order. */
static UmiStatus QueueOrder(UmiIbkrConnection *c, const char *const *fields, size_t count)
{
    if (!c || c->snapshot.state != UMI_IBKR_READY || c->snapshot.protocolVersion < 164 ||
        c->snapshot.protocolVersion > 176) return UMI_STATUS_INVALID_STATE;
    unsigned char packet[UMI_IBKR_TX_LIMIT]; size_t used = 4U;
    for (size_t i = 0U; i < count; ++i) {
        if (!UmiIbkrText(fields[i], 4096U, true)) return UMI_STATUS_INVALID_ARGUMENT;
        size_t length = strlen(fields[i]) + 1U;
        if (length > sizeof packet - used) return UMI_STATUS_CAPACITY_EXCEEDED;
        memcpy(packet + used, fields[i], length); used += length;
    }
    if (used > sizeof c->tx - c->txSize) return UMI_STATUS_BUSY;
    size_t body = used - 4U;
    packet[0] = (unsigned char)(body >> 24U); packet[1] = (unsigned char)(body >> 16U);
    packet[2] = (unsigned char)(body >> 8U); packet[3] = (unsigned char)body;
    memcpy(c->tx + c->txSize, packet, used); c->txSize += used;
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrOrderWire(UmiIbkrConnection *c, const UmiIbkrOrderReview *review, uint32_t id)
{
    if (!c || !review || !c->submission.report.enabled ||
        !c->submission.report.reviewPending || review != &c->submission.review)
        return UMI_STATUS_PERMISSION_DENIED;
    const UmiIbkrOrderDraft *d = &review->draft;
    const UmiIbkrContractDescription *r = &review->instrument;
    char idText[24], contract[24], quantity[24];
    (void)snprintf(idText, sizeof idText, "%u", (unsigned)id);
    (void)snprintf(contract, sizeof contract, "%u", (unsigned)d->contractId);
    (void)snprintf(quantity, sizeof quantity, "%u", (unsigned)d->quantity);
    /* Named groups follow the negotiated 164..176 field contract. Empty fields
     * are unset values, not zero prices. Adjusted-order fields use the provider's
     * raw double sentinel representation even when adjusted orders are absent. */
    const char *fields[128] = {
        "3", idText,
        contract, r->symbol, r->securityType, r->expiry, r->strike, r->right,
        r->multiplier, d->exchange, r->primaryExchange, r->currency,
        r->localSymbol, r->tradingClass, "", "",
        d->sell ? "SELL" : "BUY", quantity, "LMT", d->limitPrice, "",
        "DAY", "", d->account, "O", "0", "UmicomTrader", "1", "0",
        "0", "0", "0", "0", "0", "0",
        /* Allocation, timing, advisor and model. */
        "", "0", "", "", "", "", "", "", "",
        /* Short sale, OCA, auction and volatility defaults. */
        "0", "", "-1", "0", "", "", "0", "", "", "0", "0", "",
        "0", "", "", "", "", "", "0", "", "", "", "",
        /* Continuous update, reference, trailing, scale and hedge. */
        "0", "", "", "", "", "", "", "", "", "", "",
        /* Routing, clearing, hold, delta-neutral, algorithm and what-if. */
        "0", "", "", "0", "0", "", "", "0", "", "0", "0", "0",
        /* Conditions, adjusted order and raw unset-double fields. */
        "0", "", "1.7976931348623e+308", "1.7976931348623e+308",
        "1.7976931348623e+308", "1.7976931348623e+308", "1.7976931348623e+308", "0",
        /* Operator, soft dollar, cash quantity, MiFID, hedge and OMS. */
        "", "", "", "", "", "", "", "", "0", "0", "0",
        /* Price management, duration, post-to-ATS, auto-cancel parent. */
        "", "", "", "0"
    };
    size_t count = 0U;
    while (count < sizeof fields / sizeof fields[0] && fields[count]) ++count;
    if (c->snapshot.protocolVersion >= 166) fields[count++] = ""; /* Error override. */
    if (c->snapshot.protocolVersion >= 169) fields[count++] = ""; /* Manual order time. */
    return QueueOrder(c, fields, count);
}
UmiStatus UmiIbkrOrderCancelWire(UmiIbkrConnection *c, uint32_t id)
{
    char text[24];
    (void)snprintf(text, sizeof text, "%u", (unsigned)id);
    const char *fields[] = {"4", "1", text, ""};
    return QueueOrder(c, fields, c->snapshot.protocolVersion >= 169 ? 4U : 3U);
}
