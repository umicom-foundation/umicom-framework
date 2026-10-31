/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_time_sales_report.c
 *
 * PURPOSE:
 *   Verify tick report retention, age, lossless values and atomic failure outputs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/broker_connectivity/time_sales_report.h"

static int Run(Fixture *f, const char *test)
{
    f->c->snapshot.state = UMI_IBKR_READY; f->c->snapshot.stale = false; f->c->lastNow = 10U;
    UmiIbkrTimeSalesStore *store = calloc(1U, sizeof(*store)); CHECK(store != NULL);
    f->c->timeSales[0] = store;
    store->snapshot.requestId = 70U; store->snapshot.query.contract.contractId = 123U;
    strcpy(store->snapshot.query.contract.exchange, "SMART"); store->snapshot.query.kind = UMI_IBKR_TICKS_LAST;
    store->snapshot.active = store->snapshot.needsCancel = true;
    store->snapshot.requestedAtMilliseconds = 10U; store->snapshot.receivedAtMilliseconds = 20U;
    size_t count = 2U;
    if (!strcmp(test, "window")) count = 80U;
    if (!strcmp(test, "ring")) { count = 512U; store->first = 7U; store->snapshot.droppedEvents = 7U; }
    if (!strcmp(test, "empty")) count = 0U;
    store->snapshot.count = count; store->snapshot.receivedEvents = count + store->snapshot.droppedEvents;
    for (size_t i = 0U; i < count; ++i) {
        UmiIbkrTimeSalesEvent *event = &store->events[(store->first + i) % 512U];
        event->sequence = store->snapshot.droppedEvents + i + 1U; event->kind = UMI_IBKR_TICKS_LAST;
        event->epochSeconds = 1791363600U; event->receivedAtMilliseconds = 20U;
        strcpy(event->price.reportedText, "9007199254740993.123456789");
        strcpy(event->size.reportedText, "1"); strcpy(event->exchange, "TEST"); event->flags = 3U;
    }
    UmiIbkrTimeSalesReport *out = malloc(sizeof(*out)), *before = malloc(sizeof(*before));
    CHECK(out != NULL && before != NULL); memset(out, 0x5a, sizeof(*out)); *before = *out;
    UmiIbkrTimeSalesEvent *event = &store->events[store->first];
    uint64_t now = 30U, age = 15U; uint32_t id = 70U; UmiStatus expected = UMI_STATUS_OK;
    if (!strcmp(test, "invalid")) {
        CHECK(UmiIbkrTimeSalesReportCopy(NULL, id, now, age, out) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiIbkrTimeSalesReportCopy(f->c, id, now, age, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        id = 0U; expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (!strcmp(test, "age-zero")) { age = 0U; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (!strcmp(test, "missing")) { id = 71U; expected = UMI_STATUS_NOT_FOUND; }
    else if (!strcmp(test, "clock-backward")) { now = 9U; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (!strcmp(test, "future-request")) { store->snapshot.requestedAtMilliseconds = 31U; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (!strcmp(test, "future-capture")) { store->snapshot.receivedAtMilliseconds = 31U; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (!strcmp(test, "future-row")) { event->receivedAtMilliseconds = 31U; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (!strcmp(test, "row-before-request")) { event->receivedAtMilliseconds = 9U; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (!strcmp(test, "bad-sequence")) { event->sequence = 9U; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (!strcmp(test, "bad-count")) { store->snapshot.count = 513U; expected = UMI_STATUS_CAPACITY_EXCEEDED; }
    else if (!strcmp(test, "bad-first")) { store->first = 512U; expected = UMI_STATUS_CAPACITY_EXCEEDED; }
    else if (!strcmp(test, "bad-total")) { store->snapshot.receivedEvents = 1U; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (!strcmp(test, "unterminated-price")) { memset(event->price.reportedText, '9', 96U); expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (!strcmp(test, "unterminated-message")) { memset(store->snapshot.message, 'X', 512U); expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (!strcmp(test, "bad-kind")) { event->kind = UMI_IBKR_TICKS_BID_ASK; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (!strcmp(test, "old-row")) event->receivedAtMilliseconds = 10U;
    else if (!strcmp(test, "boundary")) now = 35U;
    else if (!strcmp(test, "aged")) now = 36U;
    else if (!strcmp(test, "session-stale")) f->c->snapshot.stale = true;
    else if (!strcmp(test, "disconnected")) f->c->snapshot.state = UMI_IBKR_DISCONNECTED;
    else if (!strcmp(test, "cancelled")) { store->snapshot.active = false; store->snapshot.cancelled = true; }
    else if (!strcmp(test, "failed")) store->snapshot.failed = true;
    else if (!strcmp(test, "exchange-order")) store->events[1].epochSeconds = 1U;
    else if (!strcmp(test, "all-last")) {
        store->snapshot.query.kind = UMI_IBKR_TICKS_ALL_LAST; store->events[1].kind = UMI_IBKR_TICKS_ALL_LAST;
    } else if (!strcmp(test, "bid-ask") || !strcmp(test, "midpoint")) {
        store->snapshot.query.kind = !strcmp(test, "bid-ask") ? UMI_IBKR_TICKS_BID_ASK : UMI_IBKR_TICKS_MIDPOINT;
        for (size_t i = 0U; i < count; ++i) store->events[i].kind = store->snapshot.query.kind;
    } else if (strcmp(test, "rows") && strcmp(test, "owned") && strcmp(test, "duplicate") &&
        strcmp(test, "window") && strcmp(test, "ring") && strcmp(test, "empty")) return 2;
    CHECK(UmiIbkrTimeSalesReportCopy(f->c, id, now, age, out) == expected);
    if (expected != UMI_STATUS_OK) CHECK(!memcmp(out, before, sizeof(*out)));
    else {
        size_t shown = count < 64U ? count : 64U;
        CHECK(out->count == shown && out->omittedStoredEvents == count - shown);
        CHECK(out->observedAtMilliseconds == now && out->capture.receivedEvents == count + store->snapshot.droppedEvents);
        bool stale = !count || now > 35U || store->snapshot.cancelled || store->snapshot.failed ||
            f->c->snapshot.stale || f->c->snapshot.state != UMI_IBKR_READY;
        CHECK(out->capture.stale == stale);
        if (count) {
            CHECK(out->rows[0].event.sequence == store->snapshot.receivedEvents - shown + 1U);
            CHECK(out->rows[shown - 1U].event.sequence == store->snapshot.receivedEvents);
            CHECK(out->rows[0].stale == (stale || !strcmp(test, "old-row")));
            CHECK(out->rows[shown - 1U].stale == stale);
            CHECK(!strcmp(out->rows[0].event.price.reportedText, "9007199254740993.123456789"));
            CHECK(!out->rows[0].event.price.exact && out->rows[0].event.flags == 3U);
            if (!strcmp(test, "exchange-order")) CHECK(out->rows[1].event.epochSeconds == 1U);
            if (!strcmp(test, "duplicate")) CHECK(out->rows[0].event.epochSeconds == out->rows[1].event.epochSeconds);
            if (!strcmp(test, "owned")) { memset(store->events, 0, sizeof(store->events)); CHECK(out->rows[0].event.sequence == 1U); }
        }
    }
    CHECK(f->c->txSize == 0U && f->opens == 0U && f->outSize == 0U);
    free(before); free(out); return 0;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    Fixture *f = New(); if (f == NULL) return 1;
    int status = Run(f, argv[1]); Delete(f); return status;
}
