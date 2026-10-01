/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/session_report.c
 * PURPOSE: Reconcile retained order, execution and position evidence using the existing fill rules.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "session_report_private.h"
#include "umicom/trading/execution_report.h"
#include "umicom/trading/fill.h"
#include "umicom/trading/instrument.h"
#include "umicom/trading/order_request.h"
#include "umicom/trading/position.h"
#include "umicom/trading/pretrade_risk.h"
#include "umicom/finance/identifier.h"
#include "umicom/finance/accounting/types.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

static unsigned char Fold(unsigned char c)
{ return c >= 'A' && c <= 'Z' ? (unsigned char)(c + ('a' - 'A')) : c; }
static bool Contains(const char *text, const char *filter)
{
    if (*filter == '\0') return true;
    for (; *text != '\0'; ++text) {
        size_t n = 0;
        while (filter[n] != '\0' && text[n] != '\0' && Fold((unsigned char)text[n]) == Fold((unsigned char)filter[n])) ++n;
        if (filter[n] == '\0') return true;
    }
    return false;
}
static bool Matches(const UmiInstrument *instrument, const char *filter)
{
    return Contains(instrument->instrument_id.value, filter) || Contains(instrument->symbol, filter) ||
        Contains(instrument->venue, filter) || Contains(instrument->currency.code, filter);
}
/* At most one row per retained order/fill/position, one per missing derived
 * position, and one aggregate overflow row fit the private fixed capacity. */
static void Issue(UmiTradingSessionReport *report, UmiTradingSessionArea area,
    UmiFinancialId id, uint32_t flags)
{
    if (flags != 0U) report->issues[report->summary.issues++] = (UmiTradingSessionIssue){area,id,flags};
}
static size_t FindOrder(const UmiTradingSessionSource *source, const char *id)
{
    for (size_t i = 0; i < source->orderCount; ++i)
        if (strcmp(source->orders[i].request.client_order_id.value, id) == 0) return i;
    return SIZE_MAX;
}
static size_t FindPosition(const UmiPosition *positions, size_t count, const char *id)
{
    for (size_t i = 0; i < count; ++i)
        if (strcmp(positions[i].instrument.instrument_id.value, id) == 0) return i;
    return SIZE_MAX;
}
/* Invalid bounds, identifiers or nonfinite source values cannot be displayed
 * safely. Economic disagreement is different: preserve it as an issue row. */
static bool SourceValid(const UmiTradingSessionSource *s)
{
    if (s == NULL || s->orderCount > UMI_TRADING_MAX_ORDERS || s->executionCount > UMI_TRADING_MAX_ORDERS ||
        s->positionCount > UMI_TRADING_MAX_POSITIONS || !umi_financial_id_valid(&s->account) ||
        s->environment < UMI_TRADING_SIMULATION || s->environment > UMI_TRADING_LIVE) return false;
    for (size_t i = 0; i < s->orderCount; ++i) {
        const UmiOrder *o = &s->orders[i];
        if (umi_order_request_validate(&o->request) != UMI_STATUS_OK || o->status < UMI_ORDER_NEW || o->status > UMI_ORDER_REJECTED ||
            !isfinite(o->filled_quantity) || o->filled_quantity < 0 ||
            !isfinite(o->average_fill_price) || o->average_fill_price < 0) return false;
    }
    for (size_t i = 0; i < s->executionCount; ++i) if (!umi_execution_report_valid(&s->executions[i])) return false;
    for (size_t i = 0; i < s->positionCount; ++i) {
        const UmiPosition *p = &s->positions[i];
        if (!umi_instrument_valid(&p->instrument) || !isfinite(p->quantity) ||
            !isfinite(p->average_price) || p->average_price < 0 || !isfinite(p->realised_pnl)) return false;
    }
    return true;
}
static UmiStatus Reconcile(UmiTradingSessionReport *report)
{
    const UmiTradingSessionSource *s = &report->source;
    UmiOrder *orders = calloc(UMI_TRADING_MAX_ORDERS, sizeof(*orders));
    UmiPosition *positions = calloc(UMI_TRADING_MAX_POSITIONS, sizeof(*positions));
    if (orders == NULL || positions == NULL) { free(orders); free(positions); return UMI_STATUS_OUT_OF_MEMORY; }
    size_t positionCount = 0;
    for (size_t i = 0; i < s->orderCount; ++i) {
        orders[i].request = s->orders[i].request; orders[i].status = UMI_ORDER_ACCEPTED;
    }
    for (size_t i = 0; i < s->executionCount; ++i) {
        const UmiExecutionReport *fill = &s->executions[i];
        size_t order = FindOrder(s, fill->client_order_id.value);
        report->executionOrders[i] = order;
        uint32_t flags = order == SIZE_MAX ? UMI_SESSION_MISSING : 0U;
        for (size_t j = 0; j < i; ++j)
            if (strcmp(s->executions[j].execution_id.value, fill->execution_id.value) == 0) flags |= UMI_SESSION_DUPLICATE;
        if (flags == 0U) {
            UmiStatus status = umi_order_apply_execution(&orders[order], fill);
            if (status != UMI_STATUS_OK) flags |= UMI_SESSION_QUANTITY | UMI_SESSION_STATE;
            else {
                const UmiOrderRequest *request = &orders[order].request;
                size_t pos = FindPosition(positions, positionCount, request->instrument.instrument_id.value);
                if (pos == SIZE_MAX) {
                    if (positionCount == UMI_TRADING_MAX_POSITIONS) { free(orders); free(positions); return UMI_STATUS_CAPACITY_EXCEEDED; }
                    pos = positionCount++; positions[pos].instrument = request->instrument;
                }
                if (!UmiRiskInstrumentMatches(&positions[pos].instrument, &request->instrument)) flags |= UMI_SESSION_CONTRACT;
                else if (umi_position_apply_fill(&positions[pos], request->side, fill->fill_quantity, fill->fill_price) != UMI_STATUS_OK)
                    flags |= UMI_SESSION_QUANTITY | UMI_SESSION_PNL;
            }
        }
        Issue(report, UMI_SESSION_EXECUTION, fill->execution_id, flags);
    }
    for (size_t i = 0; i < s->orderCount; ++i) {
        const UmiOrder *original = &s->orders[i], *replayed = &orders[i];
        uint32_t flags = 0;
        for (size_t j = 0; j < i; ++j)
            if (strcmp(original->request.client_order_id.value, s->orders[j].request.client_order_id.value) == 0) flags |= UMI_SESSION_DUPLICATE;
        if (strcmp(original->request.account_id.value, s->account.value) != 0) flags |= UMI_SESSION_ACCOUNT;
        if (original->request.environment != s->environment) flags |= UMI_SESSION_ENVIRONMENT;
        if (original->filled_quantity != replayed->filled_quantity) flags |= UMI_SESSION_QUANTITY;
        if (original->average_fill_price != replayed->average_fill_price) flags |= UMI_SESSION_PRICE;
        if ((replayed->filled_quantity > 0 && original->status != replayed->status && original->status != UMI_ORDER_CANCELLED) ||
            (replayed->filled_quantity == 0 && (original->status == UMI_ORDER_FILLED || original->status == UMI_ORDER_PARTIALLY_FILLED)) ||
            (replayed->status == UMI_ORDER_FILLED && original->status != UMI_ORDER_FILLED)) flags |= UMI_SESSION_STATE;
        Issue(report, UMI_SESSION_ORDER, original->request.client_order_id, flags);
    }
    for (size_t i = 0; i < s->positionCount; ++i) {
        const UmiPosition *original = &s->positions[i];
        size_t pos = FindPosition(positions, positionCount, original->instrument.instrument_id.value);
        uint32_t flags = 0;
        for (size_t j = 0; j < i; ++j)
            if (strcmp(original->instrument.instrument_id.value, s->positions[j].instrument.instrument_id.value) == 0) flags |= UMI_SESSION_DUPLICATE;
        if (pos == SIZE_MAX) flags |= UMI_SESSION_MISSING;
        else {
            if (!UmiRiskInstrumentMatches(&original->instrument, &positions[pos].instrument)) flags |= UMI_SESSION_CONTRACT;
            if (original->quantity != positions[pos].quantity) flags |= UMI_SESSION_QUANTITY;
            if (original->average_price != positions[pos].average_price) flags |= UMI_SESSION_PRICE;
            if (original->realised_pnl != positions[pos].realised_pnl) flags |= UMI_SESSION_PNL;
        }
        Issue(report, UMI_SESSION_POSITION, original->instrument.instrument_id, flags);
    }
    for (size_t i = 0; i < positionCount; ++i)
        if (FindPosition(s->positions, s->positionCount, positions[i].instrument.instrument_id.value) == SIZE_MAX)
            Issue(report, UMI_SESSION_POSITION, positions[i].instrument.instrument_id, UMI_SESSION_MISSING);
    free(orders); free(positions); return UMI_STATUS_OK;
}

UmiStatus UmiTradingBuildSessionReport(const UmiTradingSessionSource *source,
    const char *filter, UmiTradingSessionReport **outReport)
{
    if (outReport == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outReport = NULL;
    if (!SourceValid(source)) return UMI_STATUS_INVALID_STATE;
    if (filter == NULL) filter = "";
    size_t length = 0;
    while (length < UMI_TRADING_WORKSPACE_FILTER_CAPACITY && filter[length] != '\0') {
        if ((unsigned char)filter[length] < 32U || (unsigned char)filter[length] == 127U) return UMI_STATUS_INVALID_ARGUMENT;
        ++length;
    }
    if (length == UMI_TRADING_WORKSPACE_FILTER_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiTradingSessionReport *report = calloc(1, sizeof(*report));
    if (report == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    report->source = *source;
    UmiTradingSessionSummary *summary = &report->summary;
    summary->account = source->account; summary->environment = source->environment; summary->revision = source->revision;
    summary->retainedOrders = source->orderCount; summary->retainedExecutions = source->executionCount; summary->retainedPositions = source->positionCount;
    memcpy(summary->instrumentFilter, filter, length + 1U);
    UmiStatus status = Reconcile(report);
    if (status != UMI_STATUS_OK) { free(report); return status; }
    for (size_t i = 0; i < source->orderCount; ++i)
        if (Matches(&source->orders[i].request.instrument, filter)) report->orderRows[summary->orders++] = i;
    for (size_t i = 0; i < source->executionCount; ++i) {
        size_t order = report->executionOrders[i];
        if ((order == SIZE_MAX && length == 0) || (order != SIZE_MAX && Matches(&source->orders[order].request.instrument, filter)))
            report->executionRows[summary->executions++] = i;
    }
    summary->totalsAvailable = summary->issues == 0;
    for (size_t i = 0; i < source->positionCount; ++i) {
        const UmiPosition *position = &source->positions[i];
        if (!Matches(&position->instrument, filter)) continue;
        report->positionRows[summary->positions++] = i;
        if (!summary->totalsAvailable) continue;
        size_t row = 0;
        while (row < summary->currencies && !umi_accounting_currency_equal(report->currencies[row].currency, position->instrument.currency)) ++row;
        if (row == summary->currencies) { report->currencies[row].currency = position->instrument.currency; ++summary->currencies; }
        ++report->currencies[row].positions;
        report->currencies[row].realisedPnl += position->realised_pnl;
        if (!isfinite(report->currencies[row].realisedPnl)) {
            Issue(report, UMI_SESSION_BOOK, source->account, UMI_SESSION_PNL);
            summary->totalsAvailable = false; summary->currencies = 0;
        }
    }
    *outReport = report; return UMI_STATUS_OK;
}
UmiStatus UmiTradingSessionReportCapture(const UmiTradingWorkspace *workspace,
    const char *filter, UmiTradingSessionReport **outReport)
{
    if (outReport == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outReport = NULL;
    if (workspace == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiTradingSessionSource *source = malloc(sizeof(*source));
    if (source == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = UmiTradingCopySessionSource(workspace, source);
    if (status == UMI_STATUS_OK) status = UmiTradingBuildSessionReport(source, filter, outReport);
    free(source); return status;
}
void UmiTradingSessionReportDestroy(UmiTradingSessionReport *report) { free(report); }
UmiStatus UmiTradingSessionReportSummary(const UmiTradingSessionReport *report, UmiTradingSessionSummary *out)
{
    if (out != NULL) memset(out, 0, sizeof(*out));
    if (report == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = report->summary; return UMI_STATUS_OK;
}
#define SESSION_QUERY(name,type,count,value) \
UmiStatus name(const UmiTradingSessionReport *report, size_t index, type *out) { \
    if (out != NULL) memset(out, 0, sizeof(*out)); \
    if (report == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT; \
    if (index >= report->summary.count) return UMI_STATUS_NOT_FOUND; \
    *out = value; return UMI_STATUS_OK; }
SESSION_QUERY(UmiTradingSessionReportOrderAt,UmiOrder,orders,report->source.orders[report->orderRows[index]])
SESSION_QUERY(UmiTradingSessionReportPositionAt,UmiPosition,positions,report->source.positions[report->positionRows[index]])
SESSION_QUERY(UmiTradingSessionReportCurrencyAt,UmiTradingSessionCurrency,currencies,report->currencies[index])
SESSION_QUERY(UmiTradingSessionReportIssueAt,UmiTradingSessionIssue,issues,report->issues[index])
#undef SESSION_QUERY
UmiStatus UmiTradingSessionReportExecutionAt(const UmiTradingSessionReport *report, size_t index, UmiTradingSessionExecution *out)
{
    if (out != NULL) memset(out, 0, sizeof(*out));
    if (report == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= report->summary.executions) return UMI_STATUS_NOT_FOUND;
    size_t source = report->executionRows[index], order = report->executionOrders[source];
    out->fill = report->source.executions[source]; out->hasOrder = order != SIZE_MAX;
    if (out->hasOrder) out->order = report->source.orders[order].request;
    return UMI_STATUS_OK;
}
UmiStatus UmiTradingSessionReportIsCurrent(const UmiTradingSessionReport *report,
    const UmiTradingWorkspace *workspace, bool *outCurrent)
{
    if (outCurrent != NULL) *outCurrent = false;
    if (report == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return UmiTradingSessionSourceCurrent(workspace, &report->source, outCurrent);
}
const char *UmiTradingSessionAreaText(UmiTradingSessionArea area)
{
    switch (area) {
    case UMI_SESSION_ORDER:return "order";
    case UMI_SESSION_EXECUTION:return "execution";
    case UMI_SESSION_POSITION:return "position";
    case UMI_SESSION_BOOK:return "book";
    default:return "unknown";
    }
}
