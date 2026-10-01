/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/bank_operations/audit_query.c
 * PURPOSE: Define exact audit filters and workflow families without interpreting them as financial commands.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "audit_private.h"
#include "internal.h"
#include <string.h>
static int AuditEmptyDate(UmiFinancialDate date)
{return date.year==0 && date.month==0 && date.day==0;}
const char *UmiBankAuditFamilyName(UmiBankAuditFamily family)
{
    static const char *const names[]={"all","customer","account","beneficiary","test funding","transfer","manual hold","card","interest","charge","reconciliation"};
    return (unsigned)family<sizeof names/sizeof names[0]?names[family]:NULL;
}
UmiStatus UmiBankAuditActionFamily(UmiBankAction action,UmiBankAuditFamily *out)
{
    if(out==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    UmiBankAuditFamily family;
    switch(action){
    case UMI_BANK_CUSTOMER_CREATE:case UMI_BANK_CUSTOMER_SET_STATE:family=UMI_BANK_AUDIT_CUSTOMER;break;
    case UMI_BANK_ACCOUNT_OPEN:case UMI_BANK_ACCOUNT_SET_STATE:family=UMI_BANK_AUDIT_ACCOUNT;break;
    case UMI_BANK_BENEFICIARY_CREATE:case UMI_BANK_BENEFICIARY_SET_STATE:family=UMI_BANK_AUDIT_BENEFICIARY;break;
    case UMI_BANK_TEST_CREDIT:family=UMI_BANK_AUDIT_FUNDING;break;
    case UMI_BANK_TRANSFER_SUBMIT:case UMI_BANK_TRANSFER_APPROVE:case UMI_BANK_TRANSFER_REJECT:
    case UMI_BANK_TRANSFER_CANCEL:case UMI_BANK_TRANSFER_EXECUTE:case UMI_BANK_TRANSFER_REVERSE:family=UMI_BANK_AUDIT_TRANSFER;break;
    case UMI_BANK_HOLD_PLACE:case UMI_BANK_HOLD_RELEASE:family=UMI_BANK_AUDIT_HOLD;break;
    case UMI_BANK_CARD_ISSUE:case UMI_BANK_CARD_SET_STATE:case UMI_BANK_CARD_AUTHORISE:
    case UMI_BANK_CARD_CAPTURE:case UMI_BANK_CARD_VOID:case UMI_BANK_CARD_REFUND:family=UMI_BANK_AUDIT_CARD;break;
    case UMI_BANK_INTEREST_SUBMIT:case UMI_BANK_INTEREST_APPROVE:case UMI_BANK_INTEREST_REJECT:
    case UMI_BANK_INTEREST_CANCEL:case UMI_BANK_INTEREST_POST:case UMI_BANK_INTEREST_REVERSE:family=UMI_BANK_AUDIT_INTEREST;break;
    case UMI_BANK_CHARGE_SUBMIT:case UMI_BANK_CHARGE_APPROVE:case UMI_BANK_CHARGE_REJECT:
    case UMI_BANK_CHARGE_CANCEL:case UMI_BANK_CHARGE_POST:case UMI_BANK_CHARGE_REVERSE:family=UMI_BANK_AUDIT_CHARGE;break;
    case UMI_BANK_RECONCILE:case UMI_BANK_RECONCILIATION_RESOLVE:case UMI_BANK_RECONCILIATION_REOPEN:family=UMI_BANK_AUDIT_RECONCILIATION;break;
    default:return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out=family;return UMI_STATUS_OK;
}
UmiStatus UmiBankAuditQueryValidate(const UmiBankAuditQuery *q)
{
    if(q==NULL || !BankIdValid(&q->actorId,false) || !BankIdValid(&q->entityId,false) || !BankIdValid(&q->requestId,false) ||
        UmiBankAuditFamilyName(q->family)==NULL ||
        (q->firstRevision && q->lastRevision && q->firstRevision>q->lastRevision) ||
        (!AuditEmptyDate(q->fromDate) && !umi_financial_date_is_valid(q->fromDate)) ||
        (!AuditEmptyDate(q->toDate) && !umi_financial_date_is_valid(q->toDate)) ||
        (!AuditEmptyDate(q->fromDate) && !AuditEmptyDate(q->toDate) && umi_financial_date_compare(q->fromDate,q->toDate)>0))
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiBankAuditFamily family;
    return q->action==0?UMI_STATUS_OK:UmiBankAuditActionFamily(q->action,&family);
}
int BankAuditMatches(const UmiBankAuditQuery *q,const UmiBankAuditEvent *e)
{
    UmiBankAuditFamily family;if(UmiBankAuditActionFamily(e->command.action,&family)!=UMI_STATUS_OK)return 0;
    return (!q->actorId.value[0] || strcmp(q->actorId.value,e->actor.id.value)==0) &&
        (!q->entityId.value[0] || strcmp(q->entityId.value,e->command.id.value)==0) &&
        (!q->requestId.value[0] || strcmp(q->requestId.value,e->command.requestId.value)==0) &&
        (q->family==UMI_BANK_AUDIT_ALL || q->family==family) && (!q->action || q->action==e->command.action) &&
        (!q->firstRevision || e->revision>=q->firstRevision) && (!q->lastRevision || e->revision<=q->lastRevision) &&
        (AuditEmptyDate(q->fromDate) || umi_financial_date_compare(e->command.businessDate,q->fromDate)>=0) &&
        (AuditEmptyDate(q->toDate) || umi_financial_date_compare(e->command.businessDate,q->toDate)<=0);
}
