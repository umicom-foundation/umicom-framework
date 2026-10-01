/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_audit/fixture.h
 * PURPOSE: Create a complete charge timeline and a same-ID hold with genuine compensating journals.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BANK_AUDIT_TEST_FIXTURE_H
#define UMICOM_BANK_AUDIT_TEST_FIXTURE_H
#include "../bank_interest/fixture.h"
#include "umicom/bank_operations/audit_report.h"
static inline void AuditPopulate(Fixture *f)
{
    UmiBankCommand c=Make(f,UMI_BANK_CHARGE_SUBMIT,"shared");Id(&c.sourceAccountId,"account");Id(&c.ownerId,"reference");
    strcpy(c.name,"=Fee, \"review\" \\ note");c.amount=Cash(250);Send(f,&f->maker,&c);
    c=Make(f,UMI_BANK_CHARGE_APPROVE,"shared");Send(f,&f->checker,&c);
    c=Make(f,UMI_BANK_CHARGE_POST,"shared");c.businessDate=(UmiFinancialDate){2026,10,1};Send(f,&f->operator,&c);
    c=Make(f,UMI_BANK_CHARGE_REVERSE,"shared");c.businessDate=(UmiFinancialDate){2026,9,29};Send(f,&f->operator,&c);
    c=Make(f,UMI_BANK_HOLD_PLACE,"shared");Id(&c.sourceAccountId,"account");c.amount=Cash(100);Send(f,&f->operator,&c);
    c=Make(f,UMI_BANK_HOLD_RELEASE,"shared");Send(f,&f->operator,&c);
    Balance(f,100000,0);
}
static inline Fixture AuditOpen(void)
{Fixture f={0};OK(UmiBankOperationsOpenMemory(&f.bank));Setup(&f);AuditPopulate(&f);return f;}
static inline UmiBankAuditReport *AuditCapture(Fixture *f,UmiBankAuditQuery query)
{UmiBankAuditReport *report=NULL;OK(UmiBankAuditCapture(f->bank,&query,&report));CHECK(report!=NULL);return report;}
static inline UmiBankAuditSummary AuditSummary(UmiBankAuditReport *report)
{UmiBankAuditSummary s;OK(UmiBankAuditReadSummary(report,&s));return s;}
static inline UmiBankAuditRow AuditRow(UmiBankAuditReport *report,size_t index)
{UmiBankAuditRow row;OK(UmiBankAuditRowAt(report,index,&row));return row;}
#endif
