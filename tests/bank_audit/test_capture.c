/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_audit/test_capture.c
 * PURPOSE: Check exact intersecting queries, same-ID workflow isolation, original order and independent ownership.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
int main(int argc,char **argv)
{
    CHECK(argc==2);const char *name=argv[1];Fixture f=AuditOpen();UmiBankAuditQuery q={0};size_t expected=9,journals=3;
    if(strcmp(name,"all")==0){}
    else if(strcmp(name,"family")==0){q.family=UMI_BANK_AUDIT_CHARGE;expected=4;journals=2;}
    else if(strcmp(name,"entity")==0){Id(&q.entityId,"shared");expected=6;journals=2;}
    else if(strcmp(name,"scoped-entity")==0){Id(&q.entityId,"shared");q.family=UMI_BANK_AUDIT_HOLD;expected=2;journals=0;}
    else if(strcmp(name,"actor")==0){Id(&q.actorId,"checker");expected=1;journals=0;}
    else if(strcmp(name,"request")==0){Id(&q.requestId,"interest-request-6");expected=1;journals=1;}
    else if(strcmp(name,"action")==0){q.action=UMI_BANK_CHARGE_REVERSE;expected=1;journals=1;}
    else if(strcmp(name,"revision")==0){q.firstRevision=5;q.lastRevision=7;expected=3;journals=2;}
    else if(strcmp(name,"date")==0){q.fromDate=(UmiFinancialDate){2026,10,1};q.toDate=q.fromDate;expected=1;journals=1;}
    else if(strcmp(name,"intersection")==0){q.family=UMI_BANK_AUDIT_CHARGE;Id(&q.actorId,"operator");q.lastRevision=6;expected=1;journals=1;}
    else if(strcmp(name,"mismatch")==0){q.family=UMI_BANK_AUDIT_CARD;q.action=UMI_BANK_CHARGE_POST;expected=0;journals=0;}
    else if(strcmp(name,"exact")==0){Id(&q.entityId,"share");expected=0;journals=0;}
    else if(strcmp(name,"case-sensitive")==0){Id(&q.actorId,"Maker");expected=0;journals=0;}
    else if(strcmp(name,"future")==0){q.firstRevision=UINT64_MAX;expected=0;journals=0;}
    else if(strcmp(name,"ownership")!=0 && strcmp(name,"idempotent")!=0 && strcmp(name,"denied")!=0)return 2;
    UmiBankCounts before,after;OK(UmiBankOperationsCounts(f.bank,&before));
    UmiBankAuditReport *report=AuditCapture(&f,q);UmiBankAuditSummary s=AuditSummary(report);
    CHECK(s.count==expected && s.journalCount==journals && s.totalEvents==9 && s.revision==9 && !s.durable);
    uint64_t previous=0;
    for(size_t i=0;i<s.count;++i){UmiBankAuditRow row=AuditRow(report,i);CHECK(row.event.revision>previous);previous=row.event.revision;}
    if(strcmp(name,"all")==0 || strcmp(name,"family")==0){
        size_t at=strcmp(name,"all")==0?3:0;
        CHECK(!AuditRow(report,at).journalCount && !AuditRow(report,at+1).journalCount);
        CHECK(AuditRow(report,at+2).event.command.businessDate.month==10 && AuditRow(report,at+3).event.command.businessDate.day==29);
        UmiBankJournal posted,reversed;OK(UmiBankAuditJournalAt(report,at+2,0,&posted));OK(UmiBankAuditJournalAt(report,at+3,0,&reversed));
        CHECK(posted.revision==6 && reversed.revision==7 && !posted.reversal && reversed.reversal);
        CHECK(posted.entry.lines[0].debit_minor==250 && reversed.entry.lines[1].credit_minor==250);
    }
    if(strcmp(name,"denied")==0){
        UmiBankCommand denied=Make(&f,UMI_BANK_CHARGE_POST,"shared");UmiBankReceipt receipt;
        CHECK(UmiBankOperationsExecute(f.bank,&f.maker,&denied,&receipt)==UMI_STATUS_PERMISSION_DENIED && receipt.revision==0);
        UmiBankAuditReport *again=AuditCapture(&f,q);CHECK(AuditSummary(again).count==9);UmiBankAuditDestroy(again);
    }
    if(strcmp(name,"idempotent")==0){
        UmiBankAuditRow row=AuditRow(report,5);UmiBankReceipt receipt;
        OK(UmiBankOperationsExecute(f.bank,&row.event.actor,&row.event.command,&receipt));CHECK(receipt.idempotent && receipt.revision==6);
        UmiBankAuditReport *again=AuditCapture(&f,q);CHECK(AuditSummary(again).count==9);UmiBankAuditDestroy(again);
    }
    OK(UmiBankOperationsCounts(f.bank,&after));CHECK(before.revision==after.revision && before.events==after.events && before.journals==after.journals);
    Balance(&f,100000,0);
    if(strcmp(name,"ownership")==0){
        UmiBankCommand c=Make(&f,UMI_BANK_TEST_CREDIT,"later");Id(&c.sourceAccountId,"account");c.amount=Cash(1);Send(&f,&f.operator,&c);
        UmiBankAuditRow copy=AuditRow(report,0);copy.event.command.id.value[0]='x';CHECK(AuditRow(report,0).event.command.id.value[0]=='c');
        UmiBankOperationsDestroy(f.bank);f.bank=NULL;CHECK(AuditSummary(report).revision==9 && AuditSummary(report).count==9);
        UmiBankJournal j;OK(UmiBankAuditJournalAt(report,2,0,&j));CHECK(j.revision==3);
    }
    UmiBankAuditDestroy(report);UmiBankOperationsDestroy(f.bank);return 0;
}
