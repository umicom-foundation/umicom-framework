/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_charges/test_workflow.c
 * PURPOSE: Verify charge approval, protected funds, journal effects and immutable reviews.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include <limits.h>
int main(int argc,char **argv)
{
    CHECK(argc==2); const char *name=argv[1]; Fixture f={0};
    OK(UmiBankOperationsOpenMemory(&f.bank)); Setup(&f);
    UmiBankCommand c=Charge(&f,"charge","statement-2026-09");
    UmiBankReceipt receipt; UmiBankChargeRequest request; UmiBankReview *review=NULL;
    if(strcmp(name,"lifecycle")==0 || strcmp(name,"idempotency")==0) {
        Send(&f,&f.maker,&c); Balance(&f,100000,0);
        OK(UmiBankOperationsChargeAt(f.bank,0,&request)); CHECK(request.state==UMI_BANK_TRANSFER_PENDING && request.amount.minor_units==250);
        if(strcmp(name,"idempotency")==0) {
            OK(UmiBankOperationsExecute(f.bank,&f.maker,&c,&receipt)); CHECK(receipt.idempotent && receipt.revision==4);
            ++c.amount.minor_units; ChargeFail(&f,&f.maker,c,UMI_STATUS_ALREADY_EXISTS); --c.amount.minor_units;
        }
        ChargeStep(&f,UMI_BANK_CHARGE_APPROVE,&f.checker); Balance(&f,100000,0);
        c=Make(&f,UMI_BANK_CHARGE_POST,"charge"); Send(&f,&f.operator,&c); Balance(&f,99750,0);
        if(strcmp(name,"idempotency")==0) {
            OK(UmiBankOperationsExecute(f.bank,&f.operator,&c,&receipt)); CHECK(receipt.idempotent && receipt.revision==6);
        }
        OK(UmiBankOperationsChargeAt(f.bank,0,&request)); CHECK(request.state==UMI_BANK_TRANSFER_EXECUTED && request.postedRevision==6);
        UmiBankJournal journal; OK(UmiBankOperationsJournalAt(f.bank,1,&journal));
        CHECK(journal.entry.line_count==2 && !journal.reversal && strcmp(journal.entry.lines[0].account_id.value,"account")==0);
        CHECK(journal.entry.lines[0].debit_minor==250 && journal.entry.lines[1].credit_minor==250);
        CHECK(strcmp(journal.entry.lines[1].account_id.value,"sys.charges.GBP")==0);
        ChargeStep(&f,UMI_BANK_CHARGE_REVERSE,&f.operator); Balance(&f,100000,0);
        OK(UmiBankOperationsJournalAt(f.bank,2,&journal)); CHECK(journal.reversal && journal.entry.lines[1].credit_minor==250);
        OK(UmiBankOperationsChargeAt(f.bank,0,&request)); CHECK(request.state==UMI_BANK_TRANSFER_REVERSED && request.postedRevision==6 && request.reversedRevision==7);
        c=Make(&f,UMI_BANK_CHARGE_REVERSE,"charge"); ChargeFail(&f,&f.operator,c,UMI_STATUS_INVALID_STATE);
    } else if(strcmp(name,"authority")==0) {
        UmiBankActor nobody=f.maker; nobody.capabilities=0;
        ChargeFail(&f,&nobody,c,UMI_STATUS_PERMISSION_DENIED); Send(&f,&f.maker,&c);
        ChargeFail(&f,&nobody,c,UMI_STATUS_PERMISSION_DENIED); /* Receipt disclosure is gated too. */
        c=Make(&f,UMI_BANK_CHARGE_POST,"charge"); ChargeFail(&f,&f.operator,c,UMI_STATUS_INVALID_STATE);
        c=Make(&f,UMI_BANK_CHARGE_APPROVE,"charge");
        UmiBankActor same=f.maker; same.capabilities=UMI_BANK_CAP_ALL; ChargeFail(&f,&same,c,UMI_STATUS_PERMISSION_DENIED);
        Send(&f,&f.checker,&c); c=Make(&f,UMI_BANK_CHARGE_POST,"charge"); ChargeFail(&f,&f.maker,c,UMI_STATUS_PERMISSION_DENIED);
    } else if(strcmp(name,"fields")==0) {
        UmiBankCommand invalid=c; invalid.ownerId.value[0]='\0'; ChargeFail(&f,&f.maker,invalid,UMI_STATUS_INVALID_ARGUMENT);
        invalid=c; invalid.sourceAccountId.value[0]='\0'; ChargeFail(&f,&f.maker,invalid,UMI_STATUS_INVALID_ARGUMENT);
        invalid=c; invalid.name[0]='\0'; ChargeFail(&f,&f.maker,invalid,UMI_STATUS_INVALID_ARGUMENT);
        invalid=c; strcpy(invalid.name,"hidden\nreason"); ChargeFail(&f,&f.maker,invalid,UMI_STATUS_INVALID_ARGUMENT);
        invalid=c; Id(&invalid.destinationAccountId,"unexpected"); ChargeFail(&f,&f.maker,invalid,UMI_STATUS_INVALID_ARGUMENT);
        invalid=c; invalid.interest.days=1; ChargeFail(&f,&f.maker,invalid,UMI_STATUS_INVALID_ARGUMENT);
        Send(&f,&f.maker,&c); c=Make(&f,UMI_BANK_CHARGE_APPROVE,"charge"); c.amount=Cash(250);
        ChargeFail(&f,&f.checker,c,UMI_STATUS_INVALID_ARGUMENT);
    } else if(strcmp(name,"money")==0) {
        c.amount.minor_units=0; ChargeFail(&f,&f.maker,c,UMI_STATUS_INVALID_ARGUMENT);
        c.amount=Cash(250); c.amount.scale=3; ChargeFail(&f,&f.maker,c,UMI_STATUS_INVALID_ARGUMENT);
        c.amount=Cash(250); memcpy(c.amount.currency.code,"USD",4); ChargeFail(&f,&f.maker,c,UMI_STATUS_INVALID_ARGUMENT);
    } else if(strcmp(name,"duplicate-reference")==0) {
        Send(&f,&f.maker,&c); c=Charge(&f,"other","statement-2026-09"); ChargeFail(&f,&f.maker,c,UMI_STATUS_ALREADY_EXISTS);
        ChargeStep(&f,UMI_BANK_CHARGE_APPROVE,&f.checker); c=Charge(&f,"other","statement-2026-09"); ChargeFail(&f,&f.maker,c,UMI_STATUS_ALREADY_EXISTS);
        ChargeStep(&f,UMI_BANK_CHARGE_POST,&f.operator); c=Charge(&f,"other","statement-2026-09"); ChargeFail(&f,&f.maker,c,UMI_STATUS_ALREADY_EXISTS);
        ChargeStep(&f,UMI_BANK_CHARGE_REVERSE,&f.operator); c=Charge(&f,"correction","statement-2026-09"); Send(&f,&f.maker,&c);
    } else if(strcmp(name,"cancel-reject")==0) {
        Send(&f,&f.maker,&c); UmiBankActor other=f.maker; Id(&other.id,"other");
        c=Make(&f,UMI_BANK_CHARGE_CANCEL,"charge"); ChargeFail(&f,&other,c,UMI_STATUS_PERMISSION_DENIED); Send(&f,&f.maker,&c);
        c=Charge(&f,"corrected","statement-2026-09"); Send(&f,&f.maker,&c);
        c=Make(&f,UMI_BANK_CHARGE_REJECT,"corrected"); Send(&f,&f.checker,&c);
        c=Make(&f,UMI_BANK_CHARGE_POST,"corrected"); ChargeFail(&f,&f.operator,c,UMI_STATUS_INVALID_STATE); Balance(&f,100000,0);
        c=Charge(&f,"third","statement-2026-09"); Send(&f,&f.maker,&c);
        c=Make(&f,UMI_BANK_CHARGE_APPROVE,"third"); Send(&f,&f.checker,&c);
        c=Make(&f,UMI_BANK_CHARGE_CANCEL,"third"); Send(&f,&f.operator,&c);
    } else if(strcmp(name,"holds")==0) {
        Send(&f,&f.maker,&c); ChargeStep(&f,UMI_BANK_CHARGE_APPROVE,&f.checker);
        c=Make(&f,UMI_BANK_HOLD_PLACE,"hold"); Id(&c.sourceAccountId,"account"); c.amount=Cash(99900); Send(&f,&f.operator,&c);
        c=Make(&f,UMI_BANK_CHARGE_POST,"charge"); ChargeFail(&f,&f.operator,c,UMI_STATUS_INVALID_STATE); Balance(&f,100000,99900);
        c=Make(&f,UMI_BANK_HOLD_RELEASE,"hold"); Send(&f,&f.operator,&c); ChargeStep(&f,UMI_BANK_CHARGE_POST,&f.operator); Balance(&f,99750,0);
    } else if(strcmp(name,"blocked-account")==0 || strcmp(name,"blocked-customer")==0) {
        Send(&f,&f.maker,&c); ChargeStep(&f,UMI_BANK_CHARGE_APPROVE,&f.checker);
        int customer=strcmp(name,"blocked-customer")==0;
        c=Make(&f,customer?UMI_BANK_CUSTOMER_SET_STATE:UMI_BANK_ACCOUNT_SET_STATE,customer?"customer":"account");
        c.state=UMI_BANK_RECORD_BLOCKED; Send(&f,&f.maker,&c);
        c=Make(&f,UMI_BANK_CHARGE_POST,"charge"); ChargeFail(&f,&f.operator,c,UMI_STATUS_INVALID_STATE);
        ChargeStep(&f,UMI_BANK_CHARGE_CANCEL,&f.operator); Balance(&f,100000,0);
    } else if(strcmp(name,"reverse-overflow")==0) {
        Send(&f,&f.maker,&c); ChargeStep(&f,UMI_BANK_CHARGE_APPROVE,&f.checker); ChargeStep(&f,UMI_BANK_CHARGE_POST,&f.operator);
        c=Make(&f,UMI_BANK_TEST_CREDIT,"large"); Id(&c.sourceAccountId,"account"); c.amount=Cash(INT64_MAX-99750); Send(&f,&f.operator,&c);
        c=Make(&f,UMI_BANK_CHARGE_REVERSE,"charge"); ChargeFail(&f,&f.operator,c,UMI_STATUS_CAPACITY_EXCEEDED); Balance(&f,INT64_MAX,0);
    } else if(strcmp(name,"close-pending")==0 || strcmp(name,"close-approved")==0) {
        Send(&f,&f.maker,&c);
        if(strcmp(name,"close-approved")==0) ChargeStep(&f,UMI_BANK_CHARGE_APPROVE,&f.checker);
        c=Charge(&f,"debit-all","different-reference"); c.amount=Cash(100000); Send(&f,&f.maker,&c);
        c=Make(&f,UMI_BANK_CHARGE_APPROVE,"debit-all"); Send(&f,&f.checker,&c);
        c=Make(&f,UMI_BANK_CHARGE_POST,"debit-all"); Send(&f,&f.operator,&c); Balance(&f,0,0);
        c=Make(&f,UMI_BANK_ACCOUNT_SET_STATE,"account"); c.state=UMI_BANK_RECORD_CLOSED; ChargeFail(&f,&f.maker,c,UMI_STATUS_BUSY);
        ChargeStep(&f,UMI_BANK_CHARGE_CANCEL,&f.maker);
        c=Make(&f,UMI_BANK_ACCOUNT_SET_STATE,"account"); c.state=UMI_BANK_RECORD_CLOSED; Send(&f,&f.maker,&c);
    } else if(strcmp(name,"capacity")==0) {
        for(unsigned i=0;i<UMI_BANK_RECORD_CAPACITY;++i) {
            char id[40]; (void)snprintf(id,sizeof(id),"charge-%u",i); c=Charge(&f,id,id); Send(&f,&f.maker,&c);
        }
        c=Charge(&f,"overflow","overflow"); ChargeFail(&f,&f.maker,c,UMI_STATUS_CAPACITY_EXCEEDED); Balance(&f,100000,0);
    } else if(strcmp(name,"review")==0 || strcmp(name,"review-post")==0 || strcmp(name,"stale-review")==0 || strcmp(name,"review-ownership")==0) {
        const UmiBankActor *actor=&f.maker;
        if(strcmp(name,"review-post")==0) {
            Send(&f,&f.maker,&c); ChargeStep(&f,UMI_BANK_CHARGE_APPROVE,&f.checker);
            c=Make(&f,UMI_BANK_CHARGE_POST,"charge"); actor=&f.operator;
        }
        OK(UmiBankOperationsReview(f.bank,actor,&c,&review));
        UmiBankReviewSnapshot *snapshot=malloc(sizeof(*snapshot)); char *text=malloc(UMI_BANK_REVIEW_TEXT_CAPACITY); CHECK(snapshot!=NULL && text!=NULL);
        OK(UmiBankReviewSnapshotRead(review,snapshot)); CHECK(snapshot->hasCharge && snapshot->chargeAfter.amount.minor_units==250);
        CHECK(strcmp(snapshot->chargeAfter.reason,"Practice service charge")==0);
        if(strcmp(name,"review-post")==0) CHECK(snapshot->hasJournal && snapshot->accountCount==1 && snapshot->accounts[0].after.booked.minor_units==99750);
        else CHECK(!snapshot->hasJournal && snapshot->accountCount==0);
        OK(UmiBankReviewDescribe(review,text,UMI_BANK_REVIEW_TEXT_CAPACITY,NULL));
        CHECK(strstr(text,"GBP 2.50")!=NULL && strstr(text,"statement-2026-09")!=NULL && strstr(text,"No funds are reserved")!=NULL);
        if(strcmp(name,"review-ownership")==0) {
            UmiBankOperationsDestroy(f.bank); f.bank=NULL; OK(UmiBankReviewSnapshotRead(review,snapshot)); CHECK(snapshot->hasCharge);
        } else if(strcmp(name,"stale-review")==0) {
            UmiBankCommand extra=Make(&f,UMI_BANK_TEST_CREDIT,"extra"); Id(&extra.sourceAccountId,"account"); extra.amount=Cash(1); Send(&f,&f.operator,&extra);
            CHECK(UmiBankOperationsExecuteReviewed(f.bank,actor,review,&receipt)==UMI_STATUS_BUSY && receipt.revision==0);
        } else {
            bool matches=false; UmiBankActor changed=*actor; changed.capabilities=UMI_BANK_CAP_ALL;
            OK(UmiBankReviewMatches(review,&changed,&c,&matches)); CHECK(!matches);
            OK(UmiBankOperationsExecuteReviewed(f.bank,actor,review,&receipt)); CHECK(!receipt.idempotent);
        }
        free(snapshot); free(text);
    } else CHECK(0);
    UmiBankReviewDestroy(review); UmiBankOperationsDestroy(f.bank); return 0;
}
