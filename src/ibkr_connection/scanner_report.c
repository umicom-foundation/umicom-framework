/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/scanner_report.c
 * PURPOSE: Freeze ranked discovery rows and filters into a self-contained CSV document.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include "umicom/broker_connectivity/discovery_report.h"
#include <stddef.h>
enum ScanColumn
{
    SCAN_KIND,
    SCAN_REQUEST,
    SCAN_GENERATION,
    SCAN_INSTRUMENT,
    SCAN_LOCATION,
    SCAN_CODE,
    SCAN_ROWS,
    SCAN_ACTIVE,
    SCAN_FAILED,
    SCAN_CANCELLED,
    SCAN_STALE,
    SCAN_CONNECTION,
    SCAN_ENVIRONMENT,
    SCAN_ATTESTED,
    SCAN_REQUESTED,
    SCAN_RECEIVED,
    SCAN_EXPORTED,
    SCAN_PROVIDER_CODE,
    SCAN_MESSAGE,
    SCAN_RANK,
    SCAN_CONTRACT,
    SCAN_SYMBOL,
    SCAN_TYPE,
    SCAN_EXPIRY,
    SCAN_STRIKE,
    SCAN_EXACT,
    SCAN_RIGHT,
    SCAN_EXCHANGE,
    SCAN_CURRENCY,
    SCAN_LOCAL_SYMBOL,
    SCAN_MARKET_NAME,
    SCAN_CLASS,
    SCAN_DISTANCE,
    SCAN_BENCHMARK,
    SCAN_PROJECTION,
    SCAN_LEGS,
    SCAN_PROPERTY,
    SCAN_VALUE,
    SCAN_COLUMNS
};
static UmiCsvCell ScanFlag(bool value) { return UmiCsvText(value ? "true" : "false"); }
static void ScanMetadata(const UmiIbkrConnection *c, const UmiIbkrScannerSnapshot *s, uint64_t now,
                         const char *kind, UmiCsvCell *cells)
{
    for (size_t i = 0; i < SCAN_COLUMNS; ++i)
        cells[i] = UmiCsvText("");
    cells[SCAN_KIND] = UmiCsvText(kind);
    cells[SCAN_REQUEST] = UmiCsvUnsigned(s->requestId);
    cells[SCAN_GENERATION] = UmiCsvUnsigned(s->generation);
    cells[SCAN_INSTRUMENT] = UmiCsvText(s->query.instrument);
    cells[SCAN_LOCATION] = UmiCsvText(s->query.locationCode);
    cells[SCAN_CODE] = UmiCsvText(s->query.scanCode);
    cells[SCAN_ROWS] = UmiCsvUnsigned(s->query.numberOfRows);
    cells[SCAN_ACTIVE] = ScanFlag(s->active);
    cells[SCAN_FAILED] = ScanFlag(s->failed);
    cells[SCAN_CANCELLED] = ScanFlag(s->cancelled);
    cells[SCAN_STALE] = ScanFlag(s->stale);
    cells[SCAN_CONNECTION] = UmiCsvText(UmiIbkrConnectionStateName(c->snapshot.state));
    cells[SCAN_ENVIRONMENT] =
        UmiCsvText(c->snapshot.requestedEnvironment == UMI_TRADING_LIVE ? "live" : "paper");
    cells[SCAN_ATTESTED] = ScanFlag(c->snapshot.environmentAttested);
    cells[SCAN_REQUESTED] = UmiCsvUnsigned(s->requestedAtMilliseconds);
    cells[SCAN_RECEIVED] = UmiCsvUnsigned(s->receivedAtMilliseconds);
    cells[SCAN_EXPORTED] = UmiCsvUnsigned(now);
    cells[SCAN_PROVIDER_CODE] = UmiCsvSigned(s->providerCode);
    cells[SCAN_MESSAGE] = UmiCsvText(s->message);
}
UmiStatus UmiIbkrScannerExportCsv(const UmiIbkrConnection *c, uint32_t request, uint64_t now, uint64_t age,
                                  UmiCsvDocument **out)
{
    if (!out)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    UmiIbkrScannerSnapshot s;
    UmiStatus status = UmiIbkrScannerCopy(c, request, now, age, &s);
    if (status != UMI_STATUS_OK)
        return status;
    static const char *const headers[] = {"record_kind",
                                          "request_id",
                                          "generation",
                                          "instrument",
                                          "location",
                                          "scan_code",
                                          "requested_rows",
                                          "active",
                                          "failed",
                                          "cancelled",
                                          "stale",
                                          "connection_state",
                                          "requested_environment",
                                          "environment_attested",
                                          "requested_monotonic_ms",
                                          "received_monotonic_ms",
                                          "exported_monotonic_ms",
                                          "provider_code",
                                          "message",
                                          "rank",
                                          "contract_id",
                                          "symbol",
                                          "security_type",
                                          "expiry",
                                          "strike",
                                          "strike_exact",
                                          "right",
                                          "exchange",
                                          "currency",
                                          "local_symbol",
                                          "market_name",
                                          "trading_class",
                                          "distance",
                                          "benchmark",
                                          "projection",
                                          "legs",
                                          "property",
                                          "value"};
    _Static_assert(sizeof headers / sizeof headers[0] == SCAN_COLUMNS, "Scanner CSV columns must agree");
    UmiCsvDocument *document = NULL;
    status = UmiCsvDocumentCreate(2U * 1024U * 1024U, &document);
    UmiCsvCell cells[SCAN_COLUMNS];
    for (size_t i = 0; i < SCAN_COLUMNS; ++i)
        cells[i] = UmiCsvText(headers[i]);
    if (status == UMI_STATUS_OK)
        status = UmiCsvDocumentAppendRow(document, cells, SCAN_COLUMNS);
    ScanMetadata(c, &s, now, "metadata", cells);
    if (status == UMI_STATUS_OK)
        status = UmiCsvDocumentAppendRow(document, cells, SCAN_COLUMNS);
    for (size_t i = 0; status == UMI_STATUS_OK && i < s.count; ++i)
    {
        UmiIbkrScannerRow row;
        status = UmiIbkrScannerRowCopy(c, request, s.generation, i, &row);
        if (status != UMI_STATUS_OK)
            break;
        ScanMetadata(c, &s, now, "contract", cells);
        cells[SCAN_RANK] = UmiCsvUnsigned(row.rank);
        cells[SCAN_CONTRACT] = UmiCsvUnsigned(row.contractId);
        cells[SCAN_SYMBOL] = UmiCsvText(row.symbol);
        cells[SCAN_TYPE] = UmiCsvText(row.securityType);
        cells[SCAN_EXPIRY] = UmiCsvText(row.expiry);
        cells[SCAN_STRIKE] = UmiCsvText(row.strike.reportedText);
        cells[SCAN_EXACT] = ScanFlag(row.strike.exact);
        cells[SCAN_RIGHT] = UmiCsvText(row.right);
        cells[SCAN_EXCHANGE] = UmiCsvText(row.exchange);
        cells[SCAN_CURRENCY] = UmiCsvText(row.currency);
        cells[SCAN_LOCAL_SYMBOL] = UmiCsvText(row.localSymbol);
        cells[SCAN_MARKET_NAME] = UmiCsvText(row.marketName);
        cells[SCAN_CLASS] = UmiCsvText(row.tradingClass);
        cells[SCAN_DISTANCE] = UmiCsvText(row.distance);
        cells[SCAN_BENCHMARK] = UmiCsvText(row.benchmark);
        cells[SCAN_PROJECTION] = UmiCsvText(row.projection);
        cells[SCAN_LEGS] = UmiCsvText(row.legs);
        status = UmiCsvDocumentAppendRow(document, cells, SCAN_COLUMNS);
    }
    /* Preserve optional request thresholds as named records, including unset values.
     * Generic filters use a separate record kind so identical names stay unambiguous. */
    const char *names[] = {"abovePrice",          "belowPrice",
                           "aboveVolume",         "marketCapAbove",
                           "marketCapBelow",      "moodyRatingAbove",
                           "moodyRatingBelow",    "spRatingAbove",
                           "spRatingBelow",       "maturityDateAbove",
                           "maturityDateBelow",   "couponRateAbove",
                           "couponRateBelow",     "averageOptionVolumeAbove",
                           "scannerSettingPairs", "stockTypeFilter",
                           "excludeConvertible"};
    const char *values[] = {s.query.abovePrice,
                            s.query.belowPrice,
                            s.query.aboveVolume,
                            s.query.marketCapAbove,
                            s.query.marketCapBelow,
                            s.query.moodyRatingAbove,
                            s.query.moodyRatingBelow,
                            s.query.spRatingAbove,
                            s.query.spRatingBelow,
                            s.query.maturityDateAbove,
                            s.query.maturityDateBelow,
                            s.query.couponRateAbove,
                            s.query.couponRateBelow,
                            s.query.averageOptionVolumeAbove,
                            s.query.scannerSettingPairs,
                            s.query.stockTypeFilter,
                            s.query.excludeConvertible ? "true" : "false"};
    for (size_t i = 0; status == UMI_STATUS_OK && i < sizeof names / sizeof names[0]; ++i)
    {
        ScanMetadata(c, &s, now, "query_property", cells);
        cells[SCAN_PROPERTY] = UmiCsvText(names[i]);
        cells[SCAN_VALUE] = UmiCsvText(values[i]);
        status = UmiCsvDocumentAppendRow(document, cells, SCAN_COLUMNS);
    }
    for (size_t i = 0; status == UMI_STATUS_OK && i < s.query.filterCount; ++i)
    {
        ScanMetadata(c, &s, now, "generic_filter", cells);
        cells[SCAN_PROPERTY] = UmiCsvText(s.query.filters[i].tag);
        cells[SCAN_VALUE] = UmiCsvText(s.query.filters[i].value);
        status = UmiCsvDocumentAppendRow(document, cells, SCAN_COLUMNS);
    }
    if (status != UMI_STATUS_OK)
    {
        UmiCsvDocumentDestroy(document);
        return status;
    }
    *out = document;
    return UMI_STATUS_OK;
}
