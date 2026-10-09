/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_charges/test_storage.c
 * PURPOSE: Check durable replay, stale writers and failed transactions for charge journals.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "../../src/bank_operations/internal.h"
int main(int argc,char **argv)
{
    CHECK(argc==3); const char *name=argv[1]; Fixture f={0};
    FILE *created=fopen(argv[2],"wx"); CHECK(created!=NULL && fclose(created)==0);
    UmiStatus status=UmiBankOperationsOpenSqlite(argv[2],&f.bank);
    if(status==UMI_STATUS_UNAVAILABLE) { CHECK(remove(argv[2])==0); return 77; }
    OK(status); Setup(&f);
    UmiBankCommand command=Charge(&f,"charge","service-reference"); Send(&f,&f.maker,&command);
    ChargeStep(&f,UMI_BANK_CHARGE_APPROVE,&f.checker);
    command=Make(&f,UMI_BANK_CHARGE_POST,"charge"); UmiBankReview *review=NULL; UmiBankReceipt receipt;
    OK(UmiBankOperationsReview(f.bank,&f.operator,&command,&review));
    if(strcmp(name,"restart")==0 || strcmp(name,"reverse-restart")==0) {
        OK(UmiBankOperationsExecuteReviewed(f.bank,&f.operator,review,&receipt)); Balance(&f,99750,0);
        UmiBankOperationsDestroy(f.bank); f.bank=NULL; OK(UmiBankOperationsOpenSqlite(argv[2],&f.bank));
        UmiBankChargeRequest request; OK(UmiBankOperationsChargeAt(f.bank,0,&request));
        CHECK(request.state==UMI_BANK_TRANSFER_EXECUTED && request.amount.minor_units==250 && request.postedRevision==6);
        CHECK(strcmp(request.reason,"Practice service charge")==0 && strcmp(request.referenceId.value,"service-reference")==0);
        OK(UmiBankOperationsExecute(f.bank,&f.operator,&command,&receipt)); CHECK(receipt.idempotent && receipt.revision==6); Balance(&f,99750,0);
        if(strcmp(name,"reverse-restart")==0) {
            ChargeStep(&f,UMI_BANK_CHARGE_REVERSE,&f.operator); UmiBankOperationsDestroy(f.bank); f.bank=NULL;
            OK(UmiBankOperationsOpenSqlite(argv[2],&f.bank)); Balance(&f,100000,0);
            OK(UmiBankOperationsChargeAt(f.bank,0,&request)); CHECK(request.state==UMI_BANK_TRANSFER_REVERSED && request.postedRevision==6 && request.reversedRevision==7);
            UmiBankCounts counts; OK(UmiBankOperationsCounts(f.bank,&counts)); CHECK(counts.journals==3 && counts.chargeRequests==1);
        }
    } else if(strcmp(name,"write-abort")==0 || strcmp(name,"write-rollback")==0) {
        const char *trigger=strcmp(name,"write-abort")==0 ?
            "CREATE TRIGGER charge_fail BEFORE UPDATE ON umicom_kv WHEN NEW.key='bank.operations.revision' BEGIN SELECT RAISE(ABORT,'charge failure'); END;" :
            "CREATE TRIGGER charge_fail BEFORE UPDATE ON umicom_kv WHEN NEW.key='bank.operations.revision' BEGIN SELECT RAISE(ROLLBACK,'charge failure'); END;";
        OK(umi_data_server_execute(f.bank->server,trigger));
        CHECK(UmiBankOperationsExecuteReviewed(f.bank,&f.operator,review,&receipt)!=UMI_STATUS_OK && receipt.revision==0);
/* The old fixture used cached state after a transaction-loss error. Reopen the poisoned handle and then check the same saved balances; retain the original assertion for review. */
#if 0
        Balance(&f,100000,0); OK(UmiBankOperationsReload(f.bank));
#endif
        /* A whole-transaction abort leaves the banking handle deliberately
         * poisoned. Reopen through durable replay before trusting its balances;
         * statement-only ABORT still permits an explicit successful rollback. */
        if (strcmp(name, "write-rollback") == 0) {
            CHECK(f.bank->poisoned);
            UmiBankOperationsDestroy(f.bank);
            f.bank = NULL;
            OK(UmiBankOperationsOpenSqlite(argv[2], &f.bank));
        }
        Balance(&f,100000,0); OK(UmiBankOperationsReload(f.bank));
        UmiBankChargeRequest request; OK(UmiBankOperationsChargeAt(f.bank,0,&request)); CHECK(request.state==UMI_BANK_TRANSFER_APPROVED);
        UmiBankCounts counts; OK(UmiBankOperationsCounts(f.bank,&counts)); CHECK(counts.revision==5 && counts.journals==1);
        OK(umi_data_server_execute(f.bank->server,"DROP TRIGGER charge_fail;"));
        OK(UmiBankOperationsExecuteReviewed(f.bank,&f.operator,review,&receipt)); Balance(&f,99750,0);
    } else if(strcmp(name,"stale-writer")==0) {
        Fixture second={0}; OK(UmiBankOperationsOpenSqlite(argv[2],&second.bank)); second.operator=f.operator; second.serial=100;
        UmiBankCommand changed=Make(&second,UMI_BANK_HOLD_PLACE,"external-hold"); Id(&changed.sourceAccountId,"account"); changed.amount=Cash(99900); Send(&second,&second.operator,&changed);
        CHECK(UmiBankOperationsExecuteReviewed(f.bank,&f.operator,review,&receipt)==UMI_STATUS_BUSY && receipt.revision==0);
        Balance(&f,100000,0); OK(UmiBankOperationsReload(f.bank)); Balance(&f,100000,99900);
        command=Make(&f,UMI_BANK_CHARGE_POST,"charge"); ChargeFail(&f,&f.operator,command,UMI_STATUS_INVALID_STATE);
        UmiBankOperationsDestroy(second.bank);
    } else if(strcmp(name,"impossible-replay")==0) {
        UmiBankAuditEvent event; OK(UmiBankOperationsAuditAt(f.bank,3,&event));
        UmiBankCommand impossible; UmiBankCommandInit(&impossible,UMI_BANK_CHARGE_POST);
        impossible.id=event.command.id; impossible.requestId=event.command.requestId;
        impossible.businessDate=event.command.businessDate; impossible.timestampMillis=event.command.timestampMillis;
        impossible.expectedRevision=3; event.command=impossible; event.actor=f.operator;
        char encoded[BANK_RECORD_TEXT_CAPACITY]; OK(BankEncode(&event,encoded,sizeof(encoded)));
        OK(umi_data_server_set(f.bank->server,BANK_EVENT_KEY_PREFIX "00000000000000000004",encoded));
        CHECK(UmiBankOperationsReload(f.bank)==UMI_STATUS_PARSE_ERROR); Balance(&f,100000,0);
        UmiBankChargeRequest request; OK(UmiBankOperationsChargeAt(f.bank,0,&request)); CHECK(request.state==UMI_BANK_TRANSFER_APPROVED);
    } else CHECK(0);
    UmiBankReviewDestroy(review); UmiBankOperationsDestroy(f.bank); CHECK(remove(argv[2])==0); return 0;
}
