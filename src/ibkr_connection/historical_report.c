/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/historical_report.c
 * PURPOSE: Write owned historical reports without dropping missing-value or freshness evidence.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include "umicom/broker_connectivity/historical_report.h"
enum HistoricalColumn
{
    HISTORY_RECORD_KIND,
    HISTORY_REQUEST_ID,
    HISTORY_CONTRACT_ID,
    HISTORY_EXCHANGE,
    HISTORY_DATA_KIND,
    HISTORY_BAR_SECONDS,
    HISTORY_DURATION_SECONDS,
    HISTORY_REGULAR_HOURS,
    HISTORY_REQUESTED_END_UTC,
    HISTORY_BROKER_START,
    HISTORY_BROKER_END,
    HISTORY_CAPTURE_PENDING,
    HISTORY_CAPTURE_COMPLETE,
    HISTORY_CAPTURE_FAILED,
    HISTORY_CAPTURE_CANCELLED,
    HISTORY_CAPTURE_STALE,
    HISTORY_CONNECTION_STATE,
    HISTORY_REQUESTED_ENVIRONMENT,
    HISTORY_ENVIRONMENT_ATTESTED,
    HISTORY_REQUESTED_MONOTONIC_MS,
    HISTORY_COMPLETED_MONOTONIC_MS,
    HISTORY_EXPORTED_MONOTONIC_MS,
    HISTORY_CAPTURED_BAR_COUNT,
    HISTORY_PROVIDER_CODE,
    HISTORY_CAPTURE_MESSAGE,
    HISTORY_BAR_TIME_UTC_MS,
    HISTORY_OPEN,
    HISTORY_HIGH,
    HISTORY_LOW,
    HISTORY_CLOSE,
    HISTORY_OPEN_EXACT,
    HISTORY_HIGH_EXACT,
    HISTORY_LOW_EXACT,
    HISTORY_CLOSE_EXACT,
    HISTORY_REPORTED_VOLUME,
    HISTORY_VOLUME_AVAILABLE,
    HISTORY_REPORTED_WEIGHTED_AVERAGE,
    HISTORY_WEIGHTED_AVERAGE_AVAILABLE,
    HISTORY_REPORTED_TRADE_COUNT,
    HISTORY_TRADE_COUNT_AVAILABLE,
    HISTORY_COLUMNS
};
static UmiCsvCell HistoricalFlag(bool value) { return UmiCsvText(value ? "true" : "false"); }
static void HistoricalMetadata(const UmiIbkrConnection *c, const UmiIbkrHistoricalSnapshot *s, uint64_t now,
                               const char *kind, UmiCsvCell *cells)
{
    for (size_t i = 0U; i < HISTORY_COLUMNS; ++i)
        cells[i] = UmiCsvText("");
    cells[HISTORY_RECORD_KIND] = UmiCsvText(kind);
    cells[HISTORY_REQUEST_ID] = UmiCsvUnsigned(s->requestId);
    cells[HISTORY_CONTRACT_ID] = UmiCsvUnsigned(s->query.contract.contractId);
    cells[HISTORY_EXCHANGE] = UmiCsvText(s->query.contract.exchange);
    cells[HISTORY_DATA_KIND] = UmiCsvText(UmiIbkrHistoricalDataSetting(s->query.dataKind));
    cells[HISTORY_BAR_SECONDS] = UmiCsvUnsigned(s->query.barSeconds);
    cells[HISTORY_DURATION_SECONDS] = UmiCsvUnsigned(s->query.durationSeconds);
    cells[HISTORY_REGULAR_HOURS] = HistoricalFlag(s->query.regularHours);
    cells[HISTORY_REQUESTED_END_UTC] = UmiCsvText(s->query.endUtc);
    cells[HISTORY_BROKER_START] = UmiCsvText(s->startText);
    cells[HISTORY_BROKER_END] = UmiCsvText(s->endText);
    cells[HISTORY_CAPTURE_PENDING] = HistoricalFlag(s->pending);
    cells[HISTORY_CAPTURE_COMPLETE] = HistoricalFlag(s->complete);
    cells[HISTORY_CAPTURE_FAILED] = HistoricalFlag(s->failed);
    cells[HISTORY_CAPTURE_CANCELLED] = HistoricalFlag(s->cancelled);
    cells[HISTORY_CAPTURE_STALE] = HistoricalFlag(s->stale);
    cells[HISTORY_CONNECTION_STATE] = UmiCsvText(UmiIbkrConnectionStateName(c->snapshot.state));
    cells[HISTORY_REQUESTED_ENVIRONMENT] =
        UmiCsvText(c->snapshot.requestedEnvironment == UMI_TRADING_LIVE ? "live" : "paper");
    cells[HISTORY_ENVIRONMENT_ATTESTED] = HistoricalFlag(c->snapshot.environmentAttested);
    cells[HISTORY_REQUESTED_MONOTONIC_MS] = UmiCsvUnsigned(s->requestedAtMilliseconds);
    if (s->complete)
        cells[HISTORY_COMPLETED_MONOTONIC_MS] = UmiCsvUnsigned(s->completedAtMilliseconds);
    cells[HISTORY_EXPORTED_MONOTONIC_MS] = UmiCsvUnsigned(now);
    cells[HISTORY_CAPTURED_BAR_COUNT] = UmiCsvUnsigned((uint64_t)s->count);
    cells[HISTORY_PROVIDER_CODE] = UmiCsvSigned(s->providerCode);
    cells[HISTORY_CAPTURE_MESSAGE] = UmiCsvText(s->message);
}
UmiStatus UmiIbkrHistoricalExportCsv(const UmiIbkrConnection *c, uint32_t request, uint64_t now, uint64_t age,
                                     UmiCsvDocument **out)
{
    if (!out)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    UmiIbkrHistoricalSnapshot snapshot;
    UmiStatus status = UmiIbkrHistoricalCopy(c, request, now, age, &snapshot);
    if (status != UMI_STATUS_OK)
        return status;
    static const char *const headers[HISTORY_COLUMNS] = {"record_kind",
                                                         "request_id",
                                                         "contract_id",
                                                         "exchange",
                                                         "data_kind",
                                                         "bar_seconds",
                                                         "duration_seconds",
                                                         "regular_hours",
                                                         "requested_end_utc",
                                                         "broker_start",
                                                         "broker_end",
                                                         "capture_pending",
                                                         "capture_complete",
                                                         "capture_failed",
                                                         "capture_cancelled",
                                                         "capture_stale",
                                                         "connection_state",
                                                         "requested_environment",
                                                         "environment_attested",
                                                         "requested_monotonic_ms",
                                                         "completed_monotonic_ms",
                                                         "exported_monotonic_ms",
                                                         "captured_bar_count",
                                                         "provider_code",
                                                         "capture_message",
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
    UmiCsvCell cells[HISTORY_COLUMNS];
    for (size_t i = 0U; i < HISTORY_COLUMNS; ++i)
        cells[i] = UmiCsvText(headers[i]);
    if (status == UMI_STATUS_OK)
        status = UmiCsvDocumentAppendRow(document, cells, HISTORY_COLUMNS);
    HistoricalMetadata(c, &snapshot, now, "metadata", cells);
    if (status == UMI_STATUS_OK)
        status = UmiCsvDocumentAppendRow(document, cells, HISTORY_COLUMNS);
    for (size_t i = 0U; status == UMI_STATUS_OK && i < snapshot.count; ++i)
    {
        UmiIbkrHistoricalBar bar;
        status = UmiIbkrHistoricalBarCopy(c, request, i, &bar);
        if (status != UMI_STATUS_OK)
            break;
        /* Repeat metadata so filtering or copying a row cannot silently detach
         * a candle from its interval, instrument or incomplete-capture flag. */
        HistoricalMetadata(c, &snapshot, now, "historical_bar", cells);
        cells[HISTORY_BAR_TIME_UTC_MS] = UmiCsvSigned(bar.timeMilliseconds);
        cells[HISTORY_OPEN] = UmiCsvText(bar.open.reportedText);
        cells[HISTORY_HIGH] = UmiCsvText(bar.high.reportedText);
        cells[HISTORY_LOW] = UmiCsvText(bar.low.reportedText);
        cells[HISTORY_CLOSE] = UmiCsvText(bar.close.reportedText);
        cells[HISTORY_OPEN_EXACT] = HistoricalFlag(bar.open.exact);
        cells[HISTORY_HIGH_EXACT] = HistoricalFlag(bar.high.exact);
        cells[HISTORY_LOW_EXACT] = HistoricalFlag(bar.low.exact);
        cells[HISTORY_CLOSE_EXACT] = HistoricalFlag(bar.close.exact);
        cells[HISTORY_REPORTED_VOLUME] = UmiCsvText(bar.volume.reportedText);
        cells[HISTORY_VOLUME_AVAILABLE] = HistoricalFlag(bar.volumeAvailable);
        cells[HISTORY_REPORTED_WEIGHTED_AVERAGE] = UmiCsvText(bar.weightedAveragePrice.reportedText);
        cells[HISTORY_WEIGHTED_AVERAGE_AVAILABLE] = HistoricalFlag(bar.weightedAverageAvailable);
        cells[HISTORY_REPORTED_TRADE_COUNT] = UmiCsvSigned(bar.tradeCount);
        cells[HISTORY_TRADE_COUNT_AVAILABLE] = HistoricalFlag(bar.tradeCountAvailable);
        status = UmiCsvDocumentAppendRow(document, cells, HISTORY_COLUMNS);
    }
    if (status != UMI_STATUS_OK)
    {
        UmiCsvDocumentDestroy(document);
        return status;
    }
    *out = document;
    return UMI_STATUS_OK;
}
