/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/session_csv.c
 * PURPOSE: Export immutable session evidence with separate currency totals and visible discrepancies.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "umicom/trading/session_report.h"
#include "umicom/trading/environment.h"
#include "umicom/trading/order_type.h"
#define SESSION_COLUMNS 29U
static void Empty(UmiCsvCell *cells)
{ for(size_t i=0;i<SESSION_COLUMNS;++i) cells[i]=UmiCsvText(""); }
static void Instrument(UmiCsvCell *cells,const UmiInstrument *instrument)
{
    cells[5]=UmiCsvText(instrument->instrument_id.value);cells[6]=UmiCsvText(instrument->symbol);
    cells[7]=UmiCsvText(instrument->venue);cells[8]=UmiCsvText(instrument->currency.code);cells[9]=UmiCsvReal(instrument->multiplier);
}
UmiStatus UmiTradingSessionReportExportCsv(const UmiTradingSessionReport *report,UmiCsvDocument **outDocument)
{
    if(outDocument==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    *outDocument=NULL;if(report==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    UmiTradingSessionSummary s;UmiStatus status=UmiTradingSessionReportSummary(report,&s);
    if(status!=UMI_STATUS_OK)return status;
    UmiCsvDocument *document=NULL;status=UmiCsvDocumentCreate(UMI_CSV_MAX_BYTES,&document);
    if(status!=UMI_STATUS_OK)return status;
    const char *names[SESSION_COLUMNS]={"record","revision","account","environment","instrument_filter","instrument_id","symbol","venue","currency","multiplier",
        "order_id","execution_id","side","type","time_in_force","status","quantity","price","limit_price","stop_price","filled_quantity","average_price","gross_realised_pnl",
        "event_time_ms","order_version","issue_flags","count","scope","issue_entity_id"};
    UmiCsvCell cells[SESSION_COLUMNS];for(size_t i=0;i<SESSION_COLUMNS;++i)cells[i]=UmiCsvText(names[i]);
    status=UmiCsvDocumentAppendRow(document,cells,SESSION_COLUMNS);
    /* Every row carries provenance. This metadata also survives an empty filter. */
#define BASE(kind) do { Empty(cells);cells[0]=UmiCsvText(kind);cells[1]=UmiCsvUnsigned(s.revision);cells[2]=UmiCsvText(s.account.value); \
    cells[3]=UmiCsvText(umi_trading_environment_text(s.environment));cells[4]=UmiCsvText(s.instrumentFilter); } while(0)
#define APPEND() do { if(status==UMI_STATUS_OK)status=UmiCsvDocumentAppendRow(document,cells,SESSION_COLUMNS); } while(0)
    BASE("session-summary");cells[26]=UmiCsvUnsigned(s.issues);cells[27]=UmiCsvText("LOCAL RETAINED EVIDENCE; whole-book issues; not broker reconciliation; gross P&L excludes costs");APPEND();
    const char *kinds[]={"retained-orders","retained-executions","retained-positions","matching-orders","matching-executions","matching-positions"};
    size_t counts[]={s.retainedOrders,s.retainedExecutions,s.retainedPositions,s.orders,s.executions,s.positions};
    for(size_t i=0;i<6;++i){BASE(kinds[i]);cells[26]=UmiCsvUnsigned(counts[i]);APPEND();}
    for(size_t i=0;status==UMI_STATUS_OK&&i<s.orders;++i){
        UmiOrder o;(void)UmiTradingSessionReportOrderAt(report,i,&o);BASE("order");Instrument(cells,&o.request.instrument);
        cells[2]=UmiCsvText(o.request.account_id.value);cells[3]=UmiCsvText(umi_trading_environment_text(o.request.environment));cells[10]=UmiCsvText(o.request.client_order_id.value);
        cells[12]=UmiCsvText(umi_trading_side_text(o.request.side));cells[13]=UmiCsvText(umi_trading_order_type_text(o.request.type));
        cells[14]=UmiCsvText(umi_trading_time_in_force_text(o.request.tif));cells[15]=UmiCsvText(umi_trading_order_status_text(o.status));
        cells[16]=UmiCsvReal(o.request.quantity);cells[18]=UmiCsvReal(o.request.limit_price);cells[19]=UmiCsvReal(o.request.stop_price);
        cells[20]=UmiCsvReal(o.filled_quantity);cells[21]=UmiCsvReal(o.average_fill_price);cells[24]=UmiCsvUnsigned(o.version);APPEND();
    }
    for(size_t i=0;status==UMI_STATUS_OK&&i<s.executions;++i){
        UmiTradingSessionExecution e;(void)UmiTradingSessionReportExecutionAt(report,i,&e);BASE("execution");
        if(e.hasOrder){Instrument(cells,&e.order.instrument);cells[2]=UmiCsvText(e.order.account_id.value);cells[3]=UmiCsvText(umi_trading_environment_text(e.order.environment));cells[12]=UmiCsvText(umi_trading_side_text(e.order.side));}
        cells[10]=UmiCsvText(e.fill.client_order_id.value);cells[11]=UmiCsvText(e.fill.execution_id.value);
        cells[16]=UmiCsvReal(e.fill.fill_quantity);cells[17]=UmiCsvReal(e.fill.fill_price);cells[23]=UmiCsvSigned(e.fill.event_time_ms);APPEND();
    }
    for(size_t i=0;status==UMI_STATUS_OK&&i<s.positions;++i){
        UmiPosition p;(void)UmiTradingSessionReportPositionAt(report,i,&p);BASE("position");Instrument(cells,&p.instrument);
        cells[16]=UmiCsvReal(p.quantity);cells[21]=UmiCsvReal(p.average_price);cells[22]=UmiCsvReal(p.realised_pnl);APPEND();
    }
    for(size_t i=0;status==UMI_STATUS_OK&&i<s.currencies;++i){
        UmiTradingSessionCurrency c;(void)UmiTradingSessionReportCurrencyAt(report,i,&c);BASE("currency-total");
        cells[8]=UmiCsvText(c.currency.code);cells[22]=UmiCsvReal(c.realisedPnl);cells[26]=UmiCsvUnsigned(c.positions);cells[27]=UmiCsvText("matching retained positions; no FX conversion");APPEND();
    }
    if(!s.totalsAvailable){BASE("totals-withheld");cells[27]=UmiCsvText("Whole-book consistency issues; source rows are not repaired");APPEND();}
    for(size_t i=0;status==UMI_STATUS_OK&&i<s.issues;++i){
        UmiTradingSessionIssue issue;(void)UmiTradingSessionReportIssueAt(report,i,&issue);BASE("issue");
        cells[28]=UmiCsvText(issue.id.value);cells[25]=UmiCsvUnsigned(issue.flags);cells[27]=UmiCsvText(UmiTradingSessionAreaText(issue.area));APPEND();
    }
#undef APPEND
#undef BASE
    if(status!=UMI_STATUS_OK){UmiCsvDocumentDestroy(document);return status;}
    *outDocument=document;return UMI_STATUS_OK;
}
