/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/open_tms/test_dashboard_service.c
 *
 * PURPOSE:
 *   Verify Open TMS dashboard composition across cash, liquidity, risk and operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/open_tms/dashboard_service.h"
int main(void)
{
 UmiOpenTmsCashBook cash;UmiOpenTmsLiquidityLadder liq;UmiOpenTmsPositionBook pos;UmiOpenTmsRiskBook risk;UmiOpenTmsSettlementBook set;UmiOpenTmsAccountingBook acc;UmiOpenTmsReconciliationBook rec;UmiOpenTmsDashboardSnapshot snap;
 umi_open_tms_cash_book_init(&cash);umi_open_tms_liquidity_ladder_init(&liq);umi_open_tms_position_book_init(&pos);umi_open_tms_risk_book_init(&risk);umi_open_tms_settlement_book_init(&set);umi_open_tms_accounting_book_init(&acc);umi_open_tms_reconciliation_book_init(&rec);
 UmiOpenTmsCashRecord c={0};(void)strcpy(c.accountId,"a");(void)strcpy(c.currency,"USD");c.projected=1000;c.minimum=100;c.eventMilliseconds=1;assert(umi_open_tms_cash_book_upsert(&cash,&c)==UMI_STATUS_OK);
 UmiOpenTmsLiquidityBucket b={"1d",1000,100,50,0};assert(umi_open_tms_liquidity_ladder_add(&liq,&b)==UMI_STATUS_OK);
 UmiOpenTmsPosition p={0};(void)strcpy(p.accountId,"a");(void)strcpy(p.instrumentId,"i");(void)strcpy(p.currency,"USD");p.quantity=1;p.marketPrice=100;p.averagePrice=90;p.version=1;assert(umi_open_tms_position_book_upsert(&pos,&p)==UMI_STATUS_OK);
 UmiOpenTmsRiskLimit l={0};(void)strcpy(l.id,"r");(void)strcpy(l.scopeId,"p");l.limit=100;l.utilisation=50;l.version=1;assert(umi_open_tms_risk_book_upsert(&risk,&l)==UMI_STATUS_OK);
 UmiOpenTmsSettlementRecord s={0};(void)strcpy(s.id,"s");(void)strcpy(s.sourceId,"x");(void)strcpy(s.accountId,"a");(void)strcpy(s.currency,"USD");s.state=UMI_OPEN_TMS_SETTLEMENT_SETTLED;s.version=1;assert(umi_open_tms_settlement_book_add(&set,&s)==UMI_STATUS_OK);
 UmiOpenTmsAccountingLine d={0},cr={0};(void)strcpy(d.accountId,"d");(void)strcpy(d.currency,"USD");(void)strcpy(d.referenceId,"x");d.debit=10;cr=d;cr.debit=0;cr.credit=10;(void)strcpy(cr.accountId,"c");assert(umi_open_tms_accounting_book_add(&acc,&d)==UMI_STATUS_OK);assert(umi_open_tms_accounting_book_add(&acc,&cr)==UMI_STATUS_OK);assert(umi_open_tms_accounting_book_approve(&acc)==UMI_STATUS_OK);assert(umi_open_tms_accounting_book_post(&acc)==UMI_STATUS_OK);
 UmiOpenTmsReconciliationItem ri={0};(void)strcpy(ri.id,"r");(void)strcpy(ri.sourceId,"s");(void)strcpy(ri.targetId,"t");ri.sourceValue=1;ri.targetValue=1;ri.tolerance=.01;assert(umi_open_tms_reconciliation_book_add(&rec,&ri)==UMI_STATUS_OK);
 assert(umi_open_tms_dashboard_snapshot(&cash,&liq,&pos,&risk,&set,&acc,&rec,500,&snap)==UMI_STATUS_OK);
 assert(snap.operationsReady);return 0;
}
