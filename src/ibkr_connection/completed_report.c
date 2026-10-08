/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/completed_report.c
 * PURPOSE: Format completed broker observations through the shared owned CSV writer.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include "umicom/broker_connectivity/completed_report.h"
enum ReportColumn
{
    REPORT_RECORD,
    REPORT_SCOPE,
    REPORT_COMPLETE,
    REPORT_FAILED,
    REPORT_CAPTURE_STALE,
    REPORT_ROW_STALE,
    REPORT_CONNECTION,
    REPORT_ENVIRONMENT,
    REPORT_ATTESTED,
    REPORT_REQUEST_TIME,
    REPORT_END_TIME,
    REPORT_CAPTURE_TIME,
    REPORT_ROW_TIME,
    REPORT_ROW_COUNT,
    REPORT_PERMANENT,
    REPORT_PARENT,
    REPORT_ACCOUNT,
    REPORT_SYMBOL,
    REPORT_CONTRACT,
    REPORT_SECURITY,
    REPORT_CURRENCY,
    REPORT_ACTION,
    REPORT_TYPE,
    REPORT_TIF,
    REPORT_QUANTITY,
    REPORT_FILLED,
    REPORT_FILLED_EXACT,
    REPORT_LIMIT,
    REPORT_AUX,
    REPORT_STATUS,
    REPORT_COMPLETED_STATUS,
    REPORT_COMPLETED_TIME,
    REPORT_AON,
    REPORT_MINIMUM,
    REPORT_MINIMUM_EXACT,
    REPORT_OUTSIDE,
    REPORT_HIDDEN,
    REPORT_SWEEP,
    REPORT_CASH,
    REPORT_EXCHANGE,
    REPORT_EXPIRY,
    REPORT_STRIKE,
    REPORT_RIGHT,
    REPORT_MULTIPLIER,
    REPORT_LOCAL,
    REPORT_CLASS,
    REPORT_OCA,
    REPORT_REFERENCE,
    REPORT_MODEL,
    REPORT_COMBOS,
    REPORT_CONDITIONS,
    REPORT_RAW_FIELDS,
    REPORT_DUPLICATES,
    REPORT_COLUMN_COUNT
};
static UmiCsvCell FlagText(bool value) { return UmiCsvText(value ? "true" : "false"); }
/* Scope, time and completeness accompany every row. Filtering a spreadsheet
 * must not detach a record from the evidence that qualifies its interpretation. */
static void Metadata(const UmiIbkrConnection *c, const UmiIbkrCompletedOrdersSnapshot *capture, uint64_t now,
                     const char *kind, UmiCsvCell *cells)
{
    for (size_t i = 0U; i < REPORT_COLUMN_COUNT; ++i)
        cells[i] = UmiCsvText("");
    cells[REPORT_RECORD] = UmiCsvText(kind);
    cells[REPORT_SCOPE] = UmiCsvText(capture->apiOnly ? "api-origin-visible" : "all-visible");
    cells[REPORT_COMPLETE] = FlagText(capture->complete);
    cells[REPORT_FAILED] = FlagText(capture->failed);
    cells[REPORT_CAPTURE_STALE] = FlagText(capture->stale);
    cells[REPORT_CONNECTION] = UmiCsvText(UmiIbkrConnectionStateName(c->snapshot.state));
    cells[REPORT_ENVIRONMENT] =
        UmiCsvText(c->snapshot.requestedEnvironment == UMI_TRADING_LIVE ? "live" : "paper");
    cells[REPORT_ATTESTED] = FlagText(c->snapshot.environmentAttested);
    cells[REPORT_REQUEST_TIME] = UmiCsvUnsigned(capture->requestedAtMilliseconds);
    if (capture->complete)
        cells[REPORT_END_TIME] = UmiCsvUnsigned(capture->completedAtMilliseconds);
    cells[REPORT_CAPTURE_TIME] = UmiCsvUnsigned(now);
    cells[REPORT_ROW_COUNT] = UmiCsvUnsigned((uint64_t)capture->count);
}
UmiStatus UmiIbkrCompletedOrdersExportCsv(const UmiIbkrConnection *c, uint64_t now, uint64_t age,
                                          UmiCsvDocument **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    UmiIbkrCompletedOrdersSnapshot capture;
    UmiStatus status = UmiIbkrCompletedOrdersCopy(c, now, age, &capture);
    if (status != UMI_STATUS_OK)
        return status;
    if (!capture.requested)
        return UMI_STATUS_INVALID_STATE;
    static const char *const headers[REPORT_COLUMN_COUNT] = {
        "record_kind",
        "scope",
        "capture_complete",
        "capture_failed",
        "capture_stale",
        "row_stale",
        "connection_state",
        "requested_environment",
        "environment_attested",
        "requested_monotonic_ms",
        "end_monotonic_ms",
        "captured_monotonic_ms",
        "received_monotonic_ms",
        "captured_order_count",
        "permanent_id",
        "parent_permanent_id",
        "account",
        "symbol",
        "contract_id",
        "security_type",
        "currency",
        "action",
        "order_type",
        "time_in_force",
        "original_quantity",
        "reported_filled_quantity",
        "filled_exact",
        "limit_price",
        "auxiliary_price",
        "order_state",
        "completed_status",
        "broker_completed_time",
        "all_or_none_reported",
        "minimum_quantity",
        "minimum_exact",
        "outside_regular_hours",
        "hidden",
        "sweep_to_fill",
        "cash_quantity",
        "exchange",
        "expiry",
        "strike",
        "right",
        "multiplier",
        "local_symbol",
        "trading_class",
        "oca_group",
        "order_reference",
        "model_code",
        "combo_leg_count",
        "condition_count",
        "retained_field_count",
        "duplicate_count",
    };
    UmiCsvDocument *document = NULL;
    status = UmiCsvDocumentCreate(512U * 1024U, &document);
    UmiCsvCell cells[REPORT_COLUMN_COUNT];
    for (size_t i = 0U; i < REPORT_COLUMN_COUNT; ++i)
        cells[i] = UmiCsvText(headers[i]);
    if (status == UMI_STATUS_OK)
        status = UmiCsvDocumentAppendRow(document, cells, REPORT_COLUMN_COUNT);
    Metadata(c, &capture, now, "metadata", cells);
    if (status == UMI_STATUS_OK)
        status = UmiCsvDocumentAppendRow(document, cells, REPORT_COLUMN_COUNT);
    for (size_t i = 0U; status == UMI_STATUS_OK && i < capture.count; ++i)
    {
        UmiIbkrCompletedOrder row;
        status = UmiIbkrCompletedOrderCopy(c, i, now, age, &row);
        if (status != UMI_STATUS_OK)
            break;
        Metadata(c, &capture, now, "completed_order", cells);
        cells[REPORT_ROW_STALE] = FlagText(row.stale);
        cells[REPORT_ROW_TIME] = UmiCsvUnsigned(row.receivedAtMilliseconds);
        cells[REPORT_PERMANENT] = UmiCsvUnsigned(row.permanentId);
        cells[REPORT_PARENT] = UmiCsvUnsigned(row.parentPermanentId);
        cells[REPORT_CONTRACT] = UmiCsvUnsigned(row.order.contractId);
        cells[REPORT_FILLED_EXACT] = FlagText(row.filledQuantity.exact);
        cells[REPORT_MINIMUM_EXACT] = FlagText(row.minimumQuantity.exact);
        cells[REPORT_AON] = FlagText(row.allOrNone);
        cells[REPORT_OUTSIDE] = FlagText(row.outsideRegularHours);
        cells[REPORT_HIDDEN] = FlagText(row.hidden);
        cells[REPORT_SWEEP] = FlagText(row.sweepToFill);
        cells[REPORT_COMBOS] = UmiCsvUnsigned((uint64_t)row.comboLegCount);
        cells[REPORT_CONDITIONS] = UmiCsvUnsigned((uint64_t)row.conditionCount);
        cells[REPORT_RAW_FIELDS] = UmiCsvUnsigned((uint64_t)row.order.wireFieldCount);
        cells[REPORT_DUPLICATES] = UmiCsvUnsigned(row.duplicateCount);
        cells[REPORT_ACCOUNT] = UmiCsvText(row.order.account);
        cells[REPORT_SYMBOL] = UmiCsvText(row.order.symbol);
        cells[REPORT_SECURITY] = UmiCsvText(row.order.securityType);
        cells[REPORT_CURRENCY] = UmiCsvText(row.order.currency);
        cells[REPORT_ACTION] = UmiCsvText(row.order.action);
        cells[REPORT_TYPE] = UmiCsvText(row.order.orderType);
        cells[REPORT_TIF] = UmiCsvText(row.order.timeInForce);
        cells[REPORT_QUANTITY] = UmiCsvText(row.order.totalQuantity.reportedText);
        cells[REPORT_FILLED] = UmiCsvText(row.filledQuantity.reportedText);
        cells[REPORT_LIMIT] = UmiCsvText(row.order.limitPrice.reportedText);
        cells[REPORT_AUX] = UmiCsvText(row.order.auxiliaryPrice.reportedText);
        cells[REPORT_STATUS] = UmiCsvText(row.status);
        cells[REPORT_COMPLETED_STATUS] = UmiCsvText(row.completedStatus);
        cells[REPORT_COMPLETED_TIME] = UmiCsvText(row.completedTime);
        cells[REPORT_MINIMUM] = UmiCsvText(row.minimumQuantity.reportedText);
        cells[REPORT_CASH] = UmiCsvText(row.cashQuantity.reportedText);
        cells[REPORT_EXCHANGE] = UmiCsvText(row.order.exchange);
        cells[REPORT_EXPIRY] = UmiCsvText(row.order.expiry);
        cells[REPORT_STRIKE] = UmiCsvText(row.order.strike.reportedText);
        cells[REPORT_RIGHT] = UmiCsvText(row.order.right);
        cells[REPORT_MULTIPLIER] = UmiCsvText(row.order.multiplier);
        cells[REPORT_LOCAL] = UmiCsvText(row.order.localSymbol);
        cells[REPORT_CLASS] = UmiCsvText(row.order.tradingClass);
        cells[REPORT_OCA] = UmiCsvText(row.order.ocaGroup);
        cells[REPORT_REFERENCE] = UmiCsvText(row.order.orderReference);
        cells[REPORT_MODEL] = UmiCsvText(row.modelCode);
        status = UmiCsvDocumentAppendRow(document, cells, REPORT_COLUMN_COUNT);
    }
    if (status != UMI_STATUS_OK)
    {
        UmiCsvDocumentDestroy(document);
        return status;
    }
    *out = document;
    return UMI_STATUS_OK;
}
