/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_audit/test_validation.c
 * PURPOSE: Reject malformed queries and inconsistent evidence without publishing partial reports.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "../../src/bank_operations/internal.h"
int main(int argc,char **argv)
{
    CHECK(argc==2);const char *name=argv[1];Fixture f=AuditOpen();UmiBankAuditQuery q={0};UmiBankAuditReport *report=(void *)1;
    UmiStatus expected=UMI_STATUS_INVALID_ARGUMENT;
    if(strcmp(name,"id-termination")==0)memset(q.actorId.value,'x',sizeof q.actorId.value);
    else if(strcmp(name,"id-alphabet")==0)strcpy(q.entityId.value,"not an id");
    else if(strcmp(name,"family")==0)q.family=(UmiBankAuditFamily)-1;
    else if(strcmp(name,"action")==0)q.action=(UmiBankAction)999;
    else if(strcmp(name,"date")==0)q.fromDate=(UmiFinancialDate){2026,2,30};
    else if(strcmp(name,"partial-date")==0)q.toDate=(UmiFinancialDate){0,1,0};
    else if(strcmp(name,"date-order")==0){q.fromDate=(UmiFinancialDate){2026,10,1};q.toDate=(UmiFinancialDate){2026,9,30};}
    else if(strcmp(name,"revision-order")==0){q.firstRevision=9;q.lastRevision=2;}
    else if(strcmp(name,"orphan")==0){f.bank->state->journals[0].revision=100;expected=UMI_STATUS_INVALID_STATE;}
    else if(strcmp(name,"journal-reference")==0){Id(&f.bank->state->journals[0].referenceId,"wrong");expected=UMI_STATUS_INVALID_STATE;}
    else if(strcmp(name,"journal-scale")==0){f.bank->state->journals[0].scale=255;expected=UMI_STATUS_INVALID_STATE;}
    else if(strcmp(name,"journal-balance")==0){++f.bank->state->journals[0].entry.lines[1].credit_minor;expected=UMI_STATUS_INVALID_STATE;}
    else if(strcmp(name,"journal-overflow")==0){
        UmiBankJournal *j=&f.bank->state->journals[0];j->entry.lines[0].debit_minor=INT64_MAX;
        j->entry.lines[1].debit_minor=1;j->entry.lines[1].credit_minor=0;expected=UMI_STATUS_CAPACITY_EXCEEDED;
    }
    else if(strcmp(name,"event-sequence")==0){f.bank->state->events[0].revision=9;expected=UMI_STATUS_INVALID_STATE;}
    else if(strcmp(name,"count")==0){f.bank->state->counts.events=UMI_BANK_EVENT_CAPACITY+1U;expected=UMI_STATUS_INVALID_STATE;}
    else if(strcmp(name,"bounds")==0){
        report=AuditCapture(&f,q);UmiBankAuditRow row;memset(&row,0x5a,sizeof row);UmiBankAuditRow unchanged=row;
        CHECK(UmiBankAuditRowAt(report,9,&row)==UMI_STATUS_NOT_FOUND && memcmp(&row,&unchanged,sizeof row)==0);
        UmiBankJournal j;memset(&j,0x5a,sizeof j);CHECK(UmiBankAuditJournalAt(report,0,0,&j)==UMI_STATUS_NOT_FOUND && ((unsigned char *)&j)[0]==0x5a);
        UmiBankAuditFamily family=UMI_BANK_AUDIT_CARD;CHECK(UmiBankAuditActionFamily((UmiBankAction)999,&family)==UMI_STATUS_INVALID_ARGUMENT && family==UMI_BANK_AUDIT_CARD);
        for(unsigned i=1;i<=UMI_BANK_ACTION_LAST;++i){OK(UmiBankAuditActionFamily((UmiBankAction)i,&family));CHECK(family!=UMI_BANK_AUDIT_ALL);}
        UmiBankAuditDestroy(report);UmiBankOperationsDestroy(f.bank);return 0;
    }else return 2;
    CHECK(UmiBankAuditCapture(f.bank,&q,&report)==expected && report==NULL);
    UmiBankOperationsDestroy(f.bank);return 0;
}
