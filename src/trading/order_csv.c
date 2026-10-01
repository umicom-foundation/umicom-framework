/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/order_csv.c
 * PURPOSE: Export retained order evidence through the shared owned CSV service.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/trading/order_csv.h"
#include "order_report_private.h"
#include <stdlib.h>

UmiStatus UmiTradingWorkspaceExportOrdersCsv(const UmiTradingWorkspace *workspace,
    UmiCsvDocument **outDocument)
{
    if (outDocument == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outDocument = NULL;
    if (workspace == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiTradingOrderReport *capture = calloc(1U, sizeof(*capture));
    if (capture == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = UmiTradingCopyOrderReport(workspace, capture);
    UmiCsvDocument *document = NULL;
    if (status == UMI_STATUS_OK) status = UmiCsvDocumentCreate(UMI_CSV_MAX_BYTES, &document);
    if (status != UMI_STATUS_OK) { free(capture); return status; }
    static const char *const names[] = {"record", "workspace_revision", "filter_status",
        "filter_text", "retained_orders", "matching_orders", "order_id", "account_id",
        "instrument_id", "symbol", "venue", "currency", "environment", "side", "type",
        "time_in_force", "status", "quantity", "limit_price", "stop_price",
        "filled_quantity", "average_fill_price", "order_version"};
    UmiCsvCell cells[sizeof(names) / sizeof(names[0])];
    const size_t count = sizeof(cells) / sizeof(cells[0]);
    for (size_t i = 0U; i < count; ++i) cells[i] = UmiCsvText(names[i]);
    status = UmiCsvDocumentAppendRow(document, cells, count);
    /* Metadata survives an empty filter. It does not imply that any order
     * was submitted or that the unretained broker history is complete. */
    for (size_t i = 0U; i < count; ++i) cells[i] = UmiCsvText("");
    cells[0] = UmiCsvText("retained-order-summary");
    cells[1] = UmiCsvUnsigned(capture->revision);
    cells[2] = UmiCsvSigned((int64_t)capture->query.status);
    cells[3] = UmiCsvText(capture->query.text);
    cells[4] = UmiCsvUnsigned(capture->retained);
    cells[5] = UmiCsvUnsigned(capture->matching);
    if (status == UMI_STATUS_OK) status = UmiCsvDocumentAppendRow(document, cells, count);
    for (size_t i = 0U; status == UMI_STATUS_OK && i < capture->matching; ++i) {
        const UmiOrder order = capture->orders[i];
        cells[0] = UmiCsvText("order");
        cells[6] = UmiCsvText(order.request.client_order_id.value);
        cells[7] = UmiCsvText(order.request.account_id.value);
        cells[8] = UmiCsvText(order.request.instrument.instrument_id.value);
        cells[9] = UmiCsvText(order.request.instrument.symbol);
        cells[10] = UmiCsvText(order.request.instrument.venue);
        cells[11] = UmiCsvText(order.request.instrument.currency.code);
        cells[12] = UmiCsvText(umi_trading_environment_text(order.request.environment));
        cells[13] = UmiCsvText(umi_trading_side_text(order.request.side));
        cells[14] = UmiCsvText(umi_trading_order_type_text(order.request.type));
        cells[15] = UmiCsvText(umi_trading_time_in_force_text(order.request.tif));
        cells[16] = UmiCsvText(umi_trading_order_status_text(order.status));
        cells[17] = UmiCsvReal(order.request.quantity);
        cells[18] = UmiCsvReal(order.request.limit_price);
        cells[19] = UmiCsvReal(order.request.stop_price);
        cells[20] = UmiCsvReal(order.filled_quantity);
        cells[21] = UmiCsvReal(order.average_fill_price);
        cells[22] = UmiCsvUnsigned(order.version);
        status = UmiCsvDocumentAppendRow(document, cells, count);
    }
    free(capture);
    if (status != UMI_STATUS_OK) { UmiCsvDocumentDestroy(document); return status; }
    *outDocument = document;
    return UMI_STATUS_OK;
}
