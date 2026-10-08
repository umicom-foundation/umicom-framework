/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/realtime_report.c
 * PURPOSE: Preserve raw bar values and subscription evidence in an owned CSV report.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include "umicom/broker_connectivity/realtime_report.h"
enum RealtimeColumn
{
    STREAM_RECORD_KIND,
    STREAM_REQUEST_ID,
    STREAM_CONTRACT_ID,
    STREAM_EXCHANGE,
    STREAM_DATA_KIND,
    STREAM_BAR_SECONDS,
    STREAM_REGULAR_HOURS,
    STREAM_ACTIVE,
    STREAM_NEEDS_CANCEL,
    STREAM_FAILED,
    STREAM_CANCELLED,
    STREAM_STALE,
    STREAM_CONNECTION_STATE,
    STREAM_REQUESTED_ENVIRONMENT,
    STREAM_ENVIRONMENT_ATTESTED,
    STREAM_REQUESTED_MONOTONIC_MS,
    STREAM_RECEIVED_MONOTONIC_MS,
    STREAM_EXPORTED_MONOTONIC_MS,
    STREAM_RETAINED_BAR_COUNT,
    STREAM_RECEIVED_BARS,
    STREAM_DROPPED_BARS,
    STREAM_GAP_EVENTS,
    STREAM_DUPLICATE_BARS,
    STREAM_PROVIDER_CODE,
    STREAM_MESSAGE,
    STREAM_BAR_TIME_UTC_MS,
    STREAM_OPEN,
    STREAM_HIGH,
    STREAM_LOW,
    STREAM_CLOSE,
    STREAM_OPEN_EXACT,
    STREAM_HIGH_EXACT,
    STREAM_LOW_EXACT,
    STREAM_CLOSE_EXACT,
    STREAM_REPORTED_VOLUME,
    STREAM_VOLUME_AVAILABLE,
    STREAM_REPORTED_WEIGHTED_AVERAGE,
    STREAM_WEIGHTED_AVERAGE_AVAILABLE,
    STREAM_REPORTED_TRADE_COUNT,
    STREAM_TRADE_COUNT_AVAILABLE,
    STREAM_COLUMNS
};
static UmiCsvCell StreamFlag(bool value) { return UmiCsvText(value ? "true" : "false"); }
/* Repeat scope on every data row so filtering a report cannot detach prices
 * from their contract, stale flag, correction state or rolling-window limits. */
static void StreamMetadata(const UmiIbkrConnection *c, const UmiIbkrRealtimeSnapshot *s, uint64_t now,
                           const char *kind, UmiCsvCell *cells)
{
    for (size_t i = 0; i < STREAM_COLUMNS; ++i)
        cells[i] = UmiCsvText("");
    cells[STREAM_RECORD_KIND] = UmiCsvText(kind);
    cells[STREAM_REQUEST_ID] = UmiCsvUnsigned(s->requestId);
    cells[STREAM_CONTRACT_ID] = UmiCsvUnsigned(s->query.contract.contractId);
    cells[STREAM_EXCHANGE] = UmiCsvText(s->query.contract.exchange);
    cells[STREAM_DATA_KIND] = UmiCsvText(UmiIbkrHistoricalDataSetting(s->query.dataKind));
    cells[STREAM_BAR_SECONDS] = UmiCsvUnsigned(5U);
    cells[STREAM_REGULAR_HOURS] = StreamFlag(s->query.regularHours);
    cells[STREAM_CONNECTION_STATE] = UmiCsvText(UmiIbkrConnectionStateName(c->snapshot.state));
    cells[STREAM_REQUESTED_ENVIRONMENT] =
        UmiCsvText(c->snapshot.requestedEnvironment == UMI_TRADING_LIVE ? "live" : "paper");
    cells[STREAM_ENVIRONMENT_ATTESTED] = StreamFlag(c->snapshot.environmentAttested);
    cells[STREAM_REQUESTED_MONOTONIC_MS] = UmiCsvUnsigned(s->requestedAtMilliseconds);
    cells[STREAM_EXPORTED_MONOTONIC_MS] = UmiCsvUnsigned(now);
    cells[STREAM_RETAINED_BAR_COUNT] = UmiCsvUnsigned((uint64_t)s->count);
    cells[STREAM_PROVIDER_CODE] = UmiCsvSigned(s->providerCode);
    cells[STREAM_MESSAGE] = UmiCsvText(s->message);
    cells[STREAM_ACTIVE] = StreamFlag(s->active);
    cells[STREAM_NEEDS_CANCEL] = StreamFlag(s->needsCancel);
    cells[STREAM_FAILED] = StreamFlag(s->failed);
    cells[STREAM_CANCELLED] = StreamFlag(s->cancelled);
    cells[STREAM_STALE] = StreamFlag(s->stale);
    cells[STREAM_RECEIVED_BARS] = UmiCsvUnsigned(s->receivedBars);
    cells[STREAM_DROPPED_BARS] = UmiCsvUnsigned(s->droppedBars);
    cells[STREAM_GAP_EVENTS] = UmiCsvUnsigned(s->gapEvents);
    cells[STREAM_DUPLICATE_BARS] = UmiCsvUnsigned(s->duplicateBars);
    if (s->count)
        cells[STREAM_RECEIVED_MONOTONIC_MS] = UmiCsvUnsigned(s->receivedAtMilliseconds);
}
UmiStatus UmiIbkrRealtimeExportCsv(const UmiIbkrConnection *c, uint32_t request, uint64_t now, uint64_t age,
                                   UmiCsvDocument **out)
{
    if (!out)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    UmiIbkrRealtimeSnapshot s;
    UmiStatus status = UmiIbkrRealtimeCopy(c, request, now, age, &s);
    if (status != UMI_STATUS_OK)
        return status;
    static const char *const headers[STREAM_COLUMNS] = {"record_kind",
                                                        "request_id",
                                                        "contract_id",
                                                        "exchange",
                                                        "data_kind",
                                                        "bar_seconds",
                                                        "regular_hours",
                                                        "active",
                                                        "needs_cancel",
                                                        "failed",
                                                        "cancelled",
                                                        "stale",
                                                        "connection_state",
                                                        "requested_environment",
                                                        "environment_attested",
                                                        "requested_monotonic_ms",
                                                        "received_monotonic_ms",
                                                        "exported_monotonic_ms",
                                                        "retained_bar_count",
                                                        "received_bars",
                                                        "dropped_bars",
                                                        "gap_events",
                                                        "duplicate_bars",
                                                        "provider_code",
                                                        "message",
                                                        "bar_time_utc_ms",
                                                        "open",
                                                        "high",
                                                        "low",
                                                        "close",
                                                        "open_exact",
                                                        "high_exact",
                                                        "low_exact",
                                                        "close_exact",
                                                        "reported_volume",
                                                        "volume_available",
                                                        "reported_weighted_average",
                                                        "weighted_average_available",
                                                        "reported_trade_count",
                                                        "trade_count_available"};
    UmiCsvDocument *document = NULL;
    status = UmiCsvDocumentCreate(2U * 1024U * 1024U, &document);
    UmiCsvCell cells[STREAM_COLUMNS];
    for (size_t i = 0; i < STREAM_COLUMNS; ++i)
        cells[i] = UmiCsvText(headers[i]);
    if (status == UMI_STATUS_OK)
        status = UmiCsvDocumentAppendRow(document, cells, STREAM_COLUMNS);
    StreamMetadata(c, &s, now, "metadata", cells);
    if (status == UMI_STATUS_OK)
        status = UmiCsvDocumentAppendRow(document, cells, STREAM_COLUMNS);
    for (size_t i = 0; status == UMI_STATUS_OK && i < s.count; ++i)
    {
        UmiIbkrRealtimeBar bar;
        status = UmiIbkrRealtimeBarCopy(c, request, i, &bar);
        if (status != UMI_STATUS_OK)
            break;
        StreamMetadata(c, &s, now, "streaming_bar", cells);
        cells[STREAM_BAR_TIME_UTC_MS] = UmiCsvSigned(bar.timeMilliseconds);
        cells[STREAM_OPEN] = UmiCsvText(bar.open.reportedText);
        cells[STREAM_OPEN_EXACT] = StreamFlag(bar.open.exact);
        cells[STREAM_HIGH] = UmiCsvText(bar.high.reportedText);
        cells[STREAM_HIGH_EXACT] = StreamFlag(bar.high.exact);
        cells[STREAM_LOW] = UmiCsvText(bar.low.reportedText);
        cells[STREAM_LOW_EXACT] = StreamFlag(bar.low.exact);
        cells[STREAM_CLOSE] = UmiCsvText(bar.close.reportedText);
        cells[STREAM_CLOSE_EXACT] = StreamFlag(bar.close.exact);
        cells[STREAM_REPORTED_VOLUME] = UmiCsvText(bar.volume.reportedText);
        cells[STREAM_VOLUME_AVAILABLE] = StreamFlag(bar.volumeAvailable);
        cells[STREAM_REPORTED_WEIGHTED_AVERAGE] = UmiCsvText(bar.weightedAveragePrice.reportedText);
        cells[STREAM_WEIGHTED_AVERAGE_AVAILABLE] = StreamFlag(bar.weightedAverageAvailable);
        cells[STREAM_REPORTED_TRADE_COUNT] = UmiCsvSigned(bar.tradeCount);
        cells[STREAM_TRADE_COUNT_AVAILABLE] = StreamFlag(bar.tradeCountAvailable);
        status = UmiCsvDocumentAppendRow(document, cells, STREAM_COLUMNS);
    }
    if (status != UMI_STATUS_OK)
    {
        UmiCsvDocumentDestroy(document);
        return status;
    }
    *out = document;
    return UMI_STATUS_OK;
}
