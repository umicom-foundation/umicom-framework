/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_interest/test_workflow.c
 * PURPOSE: Exercise interest approval, immutable reviews and compensating journals.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include <limits.h>
int main(int argc, char **argv)
{
    Fixture f = {0}; UmiBankCommand c; UmiBankInterestRequest request; UmiBankReceipt receipt;
    UmiBankReview *review = NULL; UmiBankReviewSnapshot *s = calloc(1,sizeof *s);
    CHECK(argc == 2 && s != NULL); OK(UmiBankOperationsOpenMemory(&f.bank)); Setup(&f);
    c = Interest(&f,"interest","2026-09");
    if (strcmp(argv[1],"invalid") == 0) {
        c.interest.days = 0; Fail(&f,&f.maker,c,UMI_STATUS_INVALID_ARGUMENT);
        c.interest.days = 30; c.interest.dayCountBasis = 366; Fail(&f,&f.maker,c,UMI_STATUS_INVALID_ARGUMENT);
        c.interest.dayCountBasis = 365; c.interest.annualRateBps = 10001; Fail(&f,&f.maker,c,UMI_STATUS_INVALID_ARGUMENT);
        c.interest.annualRateBps = 1; c.interest.days = 1; Fail(&f,&f.maker,c,UMI_STATUS_INVALID_ARGUMENT);
        c.interest.annualRateBps = 500; c.interest.days = 30; c.amount = Cash(410); Fail(&f,&f.maker,c,UMI_STATUS_INVALID_ARGUMENT);
        c = Make(&f,UMI_BANK_TEST_CREDIT,"extra"); Id(&c.sourceAccountId,"account"); c.amount = Cash(1); c.interest.days = 30;
        Fail(&f,&f.operator,c,UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(argv[1],"authority") == 0) {
        Fail(&f,&f.checker,c,UMI_STATUS_PERMISSION_DENIED); Send(&f,&f.maker,&c);
        c = Make(&f,UMI_BANK_INTEREST_APPROVE,"interest"); UmiBankActor forged = f.maker; forged.capabilities = UMI_BANK_CAP_ALL;
        Fail(&f,&forged,c,UMI_STATUS_PERMISSION_DENIED);
        c = Make(&f,UMI_BANK_INTEREST_POST,"interest"); Fail(&f,&f.operator,c,UMI_STATUS_INVALID_STATE);
        Step(&f,UMI_BANK_INTEREST_APPROVE,&f.checker);
        c = Make(&f,UMI_BANK_INTEREST_POST,"interest"); Fail(&f,&f.maker,c,UMI_STATUS_PERMISSION_DENIED);
        Step(&f,UMI_BANK_INTEREST_POST,&f.operator); Balance(&f,100410,0);
    } else if (strcmp(argv[1],"duplicate") == 0) {
        Send(&f,&f.maker,&c); OK(UmiBankOperationsExecute(f.bank,&f.maker,&c,&receipt)); CHECK(receipt.idempotent);
        c.interest.days++; Fail(&f,&f.maker,c,UMI_STATUS_ALREADY_EXISTS);
        c = Interest(&f,"different-id","2026-09"); Fail(&f,&f.maker,c,UMI_STATUS_ALREADY_EXISTS);
        Step(&f,UMI_BANK_INTEREST_APPROVE,&f.checker); Step(&f,UMI_BANK_INTEREST_POST,&f.operator);
        c = Interest(&f,"different-id","2026-09"); Fail(&f,&f.maker,c,UMI_STATUS_ALREADY_EXISTS);
        Step(&f,UMI_BANK_INTEREST_REVERSE,&f.operator);
        c = Interest(&f,"correction","2026-09"); Send(&f,&f.maker,&c); Balance(&f,100000,0);
    } else if (strcmp(argv[1],"cancel-reject") == 0) {
        Send(&f,&f.maker,&c); c = Make(&f,UMI_BANK_INTEREST_CANCEL,"interest"); UmiBankActor other = f.maker; Id(&other.id,"other");
        Fail(&f,&other,c,UMI_STATUS_PERMISSION_DENIED); Send(&f,&f.maker,&c);
        c = Interest(&f,"correction","2026-09"); Send(&f,&f.maker,&c);
        c = Make(&f,UMI_BANK_INTEREST_REJECT,"correction"); Send(&f,&f.checker,&c);
        c = Make(&f,UMI_BANK_INTEREST_POST,"correction"); Fail(&f,&f.operator,c,UMI_STATUS_INVALID_STATE); Balance(&f,100000,0);
    } else if (strcmp(argv[1],"blocked") == 0) {
        Send(&f,&f.maker,&c); c = Make(&f,UMI_BANK_ACCOUNT_SET_STATE,"account"); c.state = UMI_BANK_RECORD_BLOCKED; Send(&f,&f.maker,&c);
        c = Make(&f,UMI_BANK_INTEREST_APPROVE,"interest"); Fail(&f,&f.checker,c,UMI_STATUS_INVALID_STATE);
        c = Make(&f,UMI_BANK_INTEREST_CANCEL,"interest"); Send(&f,&f.operator,&c); Balance(&f,100000,0);
    } else if (strcmp(argv[1],"funds") == 0) {
        Send(&f,&f.maker,&c); Step(&f,UMI_BANK_INTEREST_APPROVE,&f.checker); Step(&f,UMI_BANK_INTEREST_POST,&f.operator);
        c = Make(&f,UMI_BANK_HOLD_PLACE,"reservation"); Id(&c.sourceAccountId,"account"); c.amount = Cash(100001); Send(&f,&f.operator,&c);
        c = Make(&f,UMI_BANK_INTEREST_REVERSE,"interest"); Fail(&f,&f.operator,c,UMI_STATUS_INVALID_STATE); Balance(&f,100410,100001);
        c = Make(&f,UMI_BANK_HOLD_RELEASE,"reservation"); Send(&f,&f.operator,&c); Step(&f,UMI_BANK_INTEREST_REVERSE,&f.operator); Balance(&f,100000,0);
    } else if (strcmp(argv[1],"overflow") == 0) {
        c = Make(&f,UMI_BANK_TEST_CREDIT,"large"); Id(&c.sourceAccountId,"account"); c.amount = Cash(INT64_MAX-100000); Send(&f,&f.operator,&c);
        c = Interest(&f,"interest","2026-09"); Fail(&f,&f.maker,c,UMI_STATUS_CAPACITY_EXCEEDED); Balance(&f,INT64_MAX,0);
    } else if (strcmp(argv[1],"post-overflow") == 0) {
        Send(&f,&f.maker,&c); Step(&f,UMI_BANK_INTEREST_APPROVE,&f.checker);
        c=Make(&f,UMI_BANK_TEST_CREDIT,"large"); Id(&c.sourceAccountId,"account"); c.amount=Cash(INT64_MAX-100000); Send(&f,&f.operator,&c);
        c=Make(&f,UMI_BANK_INTEREST_POST,"interest"); Fail(&f,&f.operator,c,UMI_STATUS_CAPACITY_EXCEEDED);
        OK(UmiBankOperationsInterestAt(f.bank,0,&request)); CHECK(request.state==UMI_BANK_TRANSFER_APPROVED); Balance(&f,INT64_MAX,0);
    } else if (strcmp(argv[1],"close-pending") == 0) {
        Send(&f,&f.maker,&c);
        c=Make(&f,UMI_BANK_CARD_ISSUE,"card"); Id(&c.sourceAccountId,"account"); strcpy(c.name,"Practice card"); c.amount=Cash(100000); Send(&f,&f.maker,&c);
        c=Make(&f,UMI_BANK_CARD_AUTHORISE,"hold"); Id(&c.ownerId,"card"); c.amount=Cash(100000); Send(&f,&f.operator,&c);
        c=Make(&f,UMI_BANK_CARD_CAPTURE,"hold"); c.amount=Cash(100000); Send(&f,&f.operator,&c); Balance(&f,0,0);
        c=Make(&f,UMI_BANK_ACCOUNT_SET_STATE,"account"); c.state=UMI_BANK_RECORD_CLOSED; Fail(&f,&f.maker,c,UMI_STATUS_BUSY);
        Step(&f,UMI_BANK_INTEREST_CANCEL,&f.maker);
        c=Make(&f,UMI_BANK_ACCOUNT_SET_STATE,"account"); c.state=UMI_BANK_RECORD_CLOSED; Send(&f,&f.maker,&c);
    } else if (strcmp(argv[1],"capacity") == 0) {
        for (unsigned i=0;i<UMI_BANK_RECORD_CAPACITY;++i) {
            char id[32], period[32]; snprintf(id,sizeof id,"interest-%u",i); snprintf(period,sizeof period,"period-%u",i);
            c=Interest(&f,id,period); Send(&f,&f.maker,&c);
        }
        c=Interest(&f,"overflow","new-period"); Fail(&f,&f.maker,c,UMI_STATUS_CAPACITY_EXCEEDED); Balance(&f,100000,0);
    } else if (strcmp(argv[1],"review") == 0 || strcmp(argv[1],"stale-review") == 0) {
        bool matches; char *text = malloc(UMI_BANK_REVIEW_TEXT_CAPACITY); CHECK(text != NULL);
        OK(UmiBankOperationsReview(f.bank,&f.maker,&c,&review)); OK(UmiBankReviewSnapshotRead(review,s));
        CHECK(s->hasInterest && !s->interestExistedBefore && !s->hasJournal && s->accountCount==0 && s->interestAfter.amount.minor_units==410);
        OK(UmiBankReviewDescribe(review,text,UMI_BANK_REVIEW_TEXT_CAPACITY,NULL)); CHECK(strstr(text,"2026-09") && strstr(text,"Fixed principal") && strstr(text,"500 basis points"));
        c.interest.days++; OK(UmiBankReviewMatches(review,&f.maker,&c,&matches)); CHECK(!matches); c.interest.days--;
        Balance(&f,100000,0);
        if (strcmp(argv[1],"stale-review")==0) {
            UmiBankCommand extra=Make(&f,UMI_BANK_TEST_CREDIT,"extra"); Id(&extra.sourceAccountId,"account"); extra.amount=Cash(1); Send(&f,&f.operator,&extra);
            CHECK(UmiBankOperationsExecuteReviewed(f.bank,&f.maker,review,&receipt)==UMI_STATUS_BUSY && receipt.revision==0);
        } else { OK(UmiBankOperationsExecuteReviewed(f.bank,&f.maker,review,&receipt)); CHECK(!receipt.idempotent); }
        free(text);
    } else if (strcmp(argv[1],"lifecycle") == 0 || strcmp(argv[1],"fixed-principal") == 0) {
        Send(&f,&f.maker,&c); Balance(&f,100000,0); Step(&f,UMI_BANK_INTEREST_APPROVE,&f.checker);
        if (strcmp(argv[1],"fixed-principal")==0) {
            c=Make(&f,UMI_BANK_TEST_CREDIT,"later-funding"); Id(&c.sourceAccountId,"account"); c.amount=Cash(100000); Send(&f,&f.operator,&c);
            Step(&f,UMI_BANK_INTEREST_POST,&f.operator); Balance(&f,200410,0);
        } else {
            c=Make(&f,UMI_BANK_INTEREST_POST,"interest"); OK(UmiBankOperationsReview(f.bank,&f.operator,&c,&review));
            OK(UmiBankReviewSnapshotRead(review,s)); CHECK(s->hasInterest && s->hasJournal && s->accountCount==1);
            CHECK(s->accounts[0].after.booked.minor_units==100410 && umi_accounting_journal_entry_balanced(&s->journal.entry));
            OK(UmiBankOperationsExecuteReviewed(f.bank,&f.operator,review,&receipt)); Balance(&f,100410,0);
            UmiBankStatement *statement=malloc(sizeof *statement); CHECK(statement!=NULL);
            OK(UmiBankOperationsStatement(f.bank,"account",1,receipt.revision,statement)); CHECK(statement->count==2 && statement->closing.minor_units==100410);
            Step(&f,UMI_BANK_INTEREST_REVERSE,&f.operator); Balance(&f,100000,0);
            c=Make(&f,UMI_BANK_INTEREST_REVERSE,"interest"); Fail(&f,&f.operator,c,UMI_STATUS_INVALID_STATE); free(statement);
        }
        OK(UmiBankOperationsReload(f.bank)); OK(UmiBankOperationsInterestAt(f.bank,0,&request));
        CHECK(request.amount.minor_units==410 && request.principal.minor_units==100000);
        CHECK(request.postedRevision!=0);
    } else CHECK(0);
    UmiBankReviewDestroy(review); UmiBankOperationsDestroy(f.bank); free(s); return 0;
}
