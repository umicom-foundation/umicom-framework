/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/time_sales_report.c
 *
 * PURPOSE:
 *   Copy coherent tick windows with row age, sequence identity and retention counts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include "umicom/broker_connectivity/time_sales_report.h"
#include <stdlib.h>
#include <string.h>

/* A view must not interleave individual reads from a moving ring. The owner
 * thread copies one generation without callbacks, then the UI renders only
 * these independent values. Retained events never gain freshness by repainting. */
UmiStatus UmiIbkrTimeSalesReportCopy(const UmiIbkrConnection *connection, uint32_t request,
    uint64_t now, uint64_t maximumAge, UmiIbkrTimeSalesReport *out)
{
    if (connection == NULL || request == 0U || maximumAge == 0U || out == NULL ||
        now < connection->lastNow) return UMI_STATUS_INVALID_ARGUMENT;
    const UmiIbkrTimeSalesStore *store = UmiIbkrTimeSalesFind(connection, request);
    if (store == NULL) return UMI_STATUS_NOT_FOUND;
    if (store->snapshot.count > UMI_IBKR_TIME_SALES_EVENT_LIMIT ||
        store->first >= UMI_IBKR_TIME_SALES_EVENT_LIMIT) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (memchr(store->snapshot.message, 0, sizeof(store->snapshot.message)) == NULL ||
        UmiIbkrTimeSalesQueryValidate(&store->snapshot.query) != UMI_STATUS_OK ||
        store->snapshot.requestedAtMilliseconds > now ||
        store->snapshot.receivedAtMilliseconds > now ||
        store->snapshot.receivedEvents < store->snapshot.count) return UMI_STATUS_INVALID_ARGUMENT;
    UmiIbkrTimeSalesReport *copy = calloc(1U, sizeof(*copy));
    if (copy == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = UmiIbkrTimeSalesCopy(connection, request, now, maximumAge, &copy->capture);
    if (status == UMI_STATUS_OK) {
        copy->capture.stale = copy->capture.stale || connection->snapshot.stale;
        copy->observedAtMilliseconds = now;
        copy->count = copy->capture.count < UMI_IBKR_TIME_SALES_VIEW_LIMIT
            ? copy->capture.count : UMI_IBKR_TIME_SALES_VIEW_LIMIT;
        copy->omittedStoredEvents = copy->capture.count - copy->count;
        uint64_t firstSequence = copy->count == 0U ? 0U : copy->capture.receivedEvents - copy->capture.count + 1U;
        for (size_t i = 0U; i < copy->count; ++i) {
            UmiIbkrTimeSalesReportRow *row = &copy->rows[i];
            size_t index = copy->omittedStoredEvents + i;
            status = UmiIbkrTimeSalesEventCopy(connection, request, index, &row->event);
            if (status != UMI_STATUS_OK) break;
            if (row->event.sequence != firstSequence + index ||
                row->event.receivedAtMilliseconds > now ||
                row->event.receivedAtMilliseconds < copy->capture.requestedAtMilliseconds) {
                status = UMI_STATUS_INVALID_ARGUMENT; break;
            }
            bool kindMatches = row->event.kind == copy->capture.query.kind ||
                (copy->capture.query.kind == UMI_IBKR_TICKS_ALL_LAST && row->event.kind == UMI_IBKR_TICKS_LAST);
            const UmiIbkrOrderNumber *numbers[] = {&row->event.price, &row->event.size, &row->event.bidPrice,
                &row->event.askPrice, &row->event.bidSize, &row->event.askSize};
            bool textValid = memchr(row->event.exchange, 0, sizeof(row->event.exchange)) != NULL &&
                memchr(row->event.specialConditions, 0, sizeof(row->event.specialConditions)) != NULL;
            for (size_t n = 0U; n < sizeof(numbers) / sizeof(numbers[0]); ++n)
                textValid = textValid && memchr(numbers[n]->reportedText, 0, sizeof(numbers[n]->reportedText)) != NULL;
            if (!kindMatches || !textValid) { status = UMI_STATUS_INVALID_ARGUMENT; break; }
            row->stale = copy->capture.stale || now - row->event.receivedAtMilliseconds > maximumAge;
        }
    }
    if (status == UMI_STATUS_OK) *out = *copy;
    free(copy);
    return status;
}
