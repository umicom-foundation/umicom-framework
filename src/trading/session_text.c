/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/session_text.c
 * PURPOSE: Explain captured session evidence and discrepancies without claiming broker reconciliation.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "umicom/trading/session_report.h"
#include "umicom/trading/environment.h"
#include "umicom/trading/order_type.h"
#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

typedef struct SessionText { char *output; size_t capacity, length; UmiStatus status; } SessionText;
static void Append(SessionText *text, const char *format, ...)
{
    if (text->status != UMI_STATUS_OK) return;
    va_list args; va_start(args, format);
    size_t remaining = text->length < text->capacity ? text->capacity - text->length : 0U;
    int needed = vsnprintf(remaining != 0 ? text->output + text->length : NULL, remaining, format, args);
    va_end(args);
    if (needed < 0 || (size_t)needed > SIZE_MAX - text->length - 1U) { text->status = UMI_STATUS_CAPACITY_EXCEEDED; return; }
    text->length += (size_t)needed;
}
UmiStatus UmiTradingSessionReportDescribe(const UmiTradingSessionReport *report,
    char *output, size_t capacity, size_t *required)
{
    if (required != NULL) *required = 0;
    if (output != NULL && capacity != 0) output[0] = '\0';
    if (report == NULL || (output == NULL && capacity != 0) || (output == NULL && required == NULL)) return UMI_STATUS_INVALID_ARGUMENT;
    UmiTradingSessionSummary summary;
    UmiStatus status = UmiTradingSessionReportSummary(report, &summary);
    if (status != UMI_STATUS_OK) return status;
    SessionText text = {output,capacity,0,UMI_STATUS_OK};
    Append(&text, "RETAINED LOCAL SESSION REVIEW\nAccount: %s\nEnvironment at capture: %s\nWorkspace revision: %" PRIu64
        "\nInstrument filter: %s\nOrders: %zu of %zu; executions: %zu of %zu; positions: %zu of %zu\n"
        "Whole-book consistency issues: %zu\n"
        "This checks local retained records, not a broker statement. No orders are sent, cancelled or repaired.\n"
        "Realised P&L is gross, before commissions, financing and tax. No cash balance, currency conversion or unrealised valuation is inferred.\n",
        summary.account.value, umi_trading_environment_text(summary.environment), summary.revision,
        summary.instrumentFilter[0] ? summary.instrumentFilter : "all instruments", summary.orders,summary.retainedOrders,
        summary.executions,summary.retainedExecutions,summary.positions,summary.retainedPositions,summary.issues);
    Append(&text, "\nCONSISTENCY\n");
    if (summary.issues == 0) Append(&text, "Retained fills agree exactly with orders and positions using the same arithmetic in arrival order. Missing external history cannot be detected here.\n");
    for (size_t i=0; i<summary.issues; ++i) {
        UmiTradingSessionIssue issue; (void)UmiTradingSessionReportIssueAt(report,i,&issue);
        Append(&text,"%s %s:",UmiTradingSessionAreaText(issue.area),issue.id.value);
        if (issue.flags & UMI_SESSION_QUANTITY) Append(&text," quantity");
        if (issue.flags & UMI_SESSION_PRICE) Append(&text," average price");
        if (issue.flags & UMI_SESSION_STATE) Append(&text," lifecycle state");
        if (issue.flags & UMI_SESSION_CONTRACT) Append(&text," instrument definition");
        if (issue.flags & UMI_SESSION_DUPLICATE) Append(&text," duplicate identity");
        if (issue.flags & UMI_SESSION_MISSING) Append(&text," missing related record");
        if (issue.flags & UMI_SESSION_ACCOUNT) Append(&text," account mismatch");
        if (issue.flags & UMI_SESSION_ENVIRONMENT) Append(&text," environment differs from current book");
        if (issue.flags & UMI_SESSION_PNL) Append(&text," realised P&L or arithmetic capacity");
        Append(&text,"\n");
    }
    Append(&text,"\nCURRENCY TOTALS (matching positions)\n");
    if (!summary.totalsAvailable) Append(&text,"Withheld: inspect whole-book consistency issues first. Source rows below remain unchanged.\n");
    else if (summary.currencies==0) Append(&text,"No matching retained positions; no monetary total is inferred.\n");
    for (size_t i=0; i<summary.currencies; ++i) {
        UmiTradingSessionCurrency c; (void)UmiTradingSessionReportCurrencyAt(report,i,&c);
        Append(&text,"%.3s: gross realised P&L %.17g; positions %zu\n",c.currency.code,c.realisedPnl,c.positions);
    }
    Append(&text,"\nORDERS (creation order)\n");
    for (size_t i=0; i<summary.orders; ++i) {
        UmiOrder o; (void)UmiTradingSessionReportOrderAt(report,i,&o);
        Append(&text,"%s | account %s | %s %s %s | %s | %s %.17g | %s | filled %.17g @ %.17g | version %" PRIu64 "\n",
            o.request.client_order_id.value,o.request.account_id.value,o.request.instrument.instrument_id.value,o.request.instrument.symbol,o.request.instrument.venue,
            umi_trading_environment_text(o.request.environment),umi_trading_side_text(o.request.side),o.request.quantity,
            umi_trading_order_status_text(o.status),o.filled_quantity,o.average_fill_price,o.version);
    }
    Append(&text,"\nEXECUTIONS (arrival order; event time is milliseconds since Unix epoch)\n");
    for (size_t i=0; i<summary.executions; ++i) {
        UmiTradingSessionExecution e; (void)UmiTradingSessionReportExecutionAt(report,i,&e);
        Append(&text,"%s | order %s | %s %s | %.17g @ %.17g | time %" PRId64 "\n",e.fill.execution_id.value,
            e.fill.client_order_id.value,e.hasOrder?e.order.instrument.symbol:"unresolved",e.hasOrder?umi_trading_side_text(e.order.side):"unknown",
            e.fill.fill_quantity,e.fill.fill_price,e.fill.event_time_ms);
    }
    Append(&text,"\nPOSITIONS (retained average-cost book)\n");
    for (size_t i=0; i<summary.positions; ++i) {
        UmiPosition p; (void)UmiTradingSessionReportPositionAt(report,i,&p);
        Append(&text,"%s | %s %s | quantity %.17g | average %.17g | multiplier %.17g | gross realised P&L %.3s %.17g\n",
            p.instrument.instrument_id.value,p.instrument.symbol,p.instrument.venue,p.quantity,p.average_price,
            p.instrument.multiplier,p.instrument.currency.code,p.realised_pnl);
    }
    if (required != NULL && text.status == UMI_STATUS_OK) *required = text.length + 1U;
    if (text.status == UMI_STATUS_OK && output != NULL && text.length >= capacity) text.status = UMI_STATUS_CAPACITY_EXCEEDED;
    if (text.status != UMI_STATUS_OK && output != NULL && capacity != 0) output[0]='\0';
    return text.status;
}
