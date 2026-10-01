/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_session/fixture.h
 * PURPOSE: Share actual workspace fills and captured-source helpers for session review cases.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_TRADING_SESSION_TEST_FIXTURE_H
#define UMICOM_TRADING_SESSION_TEST_FIXTURE_H
#include "../trading_execution/order_review_fixture.h"
#include "umicom/trading/session_report.h"
#include "../../src/trading/session_report_private.h"
#define CHECK REVIEW_CHECK
#define OK(x) CHECK((x)==UMI_STATUS_OK)
static inline void Fill(ReviewFixture *f,const char *order,const char *id,double quantity,double price,int64_t time)
{
    UmiExecutionReport fill=ReviewFixtureFill(order);strcpy(fill.execution_id.value,id);
    fill.fill_quantity=quantity;fill.fill_price=price;fill.event_time_ms=time;
    OK(umi_trading_workspace_record_execution(f->workspace,&fill));
}
static inline void Submit(ReviewFixture *f,UmiSide side,double quantity,double price,char id[UMI_FINANCE_ID_CAPACITY])
{
    OK(umi_trading_workspace_select_instrument(f->workspace,"CME.NQ.202609"));
    OK(umi_trading_workspace_set_draft_side(f->workspace,side));
    OK(umi_trading_workspace_set_draft_quantity(f->workspace,quantity));
    OK(umi_trading_workspace_set_draft_prices(f->workspace,price,0));
    UmiRiskDecision decision;OK(umi_trading_workspace_submit_order(f->workspace,2000,&decision));CHECK(decision.allowed);
    UmiTradingWorkspaceSnapshot snapshot;OK(umi_trading_workspace_snapshot(f->workspace,&snapshot));strcpy(id,snapshot.selected_order_id);
}
static inline UmiTradingSessionSource *Source(ReviewFixture *f)
{
    UmiTradingSessionSource *s=malloc(sizeof(*s));CHECK(s!=NULL);OK(UmiTradingCopySessionSource(f->workspace,s));return s;
}
static inline void HasIssue(UmiTradingSessionReport *report,UmiTradingSessionArea area,uint32_t flag)
{
    UmiTradingSessionSummary summary;OK(UmiTradingSessionReportSummary(report,&summary));CHECK(!summary.totalsAvailable && summary.issues>0);
    bool found=false;for(size_t i=0;i<summary.issues;++i){UmiTradingSessionIssue issue;OK(UmiTradingSessionReportIssueAt(report,i,&issue));if(issue.area==area&&(issue.flags&flag))found=true;}
    CHECK(found && summary.currencies==0);
}
#endif
