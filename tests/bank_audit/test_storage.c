/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_audit/test_storage.c
 * PURPOSE: Distinguish captured, cached, reloaded and reopened audit evidence across SQLite owners.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
int main(int argc,char **argv)
{
    CHECK(argc==3);const char *name=argv[1];Fixture f={0};FILE *file=fopen(argv[2],"wx");CHECK(file!=NULL && fclose(file)==0);
    UmiStatus status=UmiBankOperationsOpenSqlite(argv[2],&f.bank);if(status==UMI_STATUS_UNAVAILABLE){CHECK(remove(argv[2])==0);return 77;}OK(status);Setup(&f);AuditPopulate(&f);
    UmiBankAuditReport *report=AuditCapture(&f,(UmiBankAuditQuery){0});CHECK(AuditSummary(report).durable && AuditSummary(report).revision==9);
    if(strcmp(name,"reopen")==0){
        UmiBankOperationsDestroy(f.bank);f.bank=NULL;OK(UmiBankOperationsOpenSqlite(argv[2],&f.bank));
        UmiBankAuditReport *current=AuditCapture(&f,(UmiBankAuditQuery){0});CHECK(AuditSummary(current).count==9 && AuditRow(current,6).event.command.businessDate.day==29);UmiBankAuditDestroy(current);
    }else if(strcmp(name,"reload")==0){
        Fixture writer={0};writer.serial=100;writer.operator=f.operator;OK(UmiBankOperationsOpenSqlite(argv[2],&writer.bank));
        UmiBankCommand c=Make(&writer,UMI_BANK_TEST_CREDIT,"later");Id(&c.sourceAccountId,"account");c.amount=Cash(1);Send(&writer,&writer.operator,&c);
        UmiBankAuditReport *cached=AuditCapture(&f,(UmiBankAuditQuery){0});CHECK(AuditSummary(cached).count==9);UmiBankAuditDestroy(cached);
        OK(UmiBankOperationsReload(f.bank));UmiBankAuditReport *current=AuditCapture(&f,(UmiBankAuditQuery){0});CHECK(AuditSummary(current).count==10);UmiBankAuditDestroy(current);UmiBankOperationsDestroy(writer.bank);
    }else return 2;
    CHECK(AuditSummary(report).revision==9 && AuditSummary(report).count==9);UmiBankOperationsDestroy(f.bank);
    UmiBankJournal journal;OK(UmiBankAuditJournalAt(report,6,0,&journal));CHECK(journal.reversal && journal.revision==7);
    UmiBankAuditDestroy(report);CHECK(remove(argv[2])==0);return 0;
}
