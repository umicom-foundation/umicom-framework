/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_interest/test_storage.c
 * PURPOSE: Check replay, canonical encoding and rollback of interest commands.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "../../src/bank_operations/internal.h"
int main(int argc, char **argv)
{
    Fixture f={0}; UmiBankCommand c; UmiBankReceipt receipt; UmiBankReview *review=NULL; UmiStatus status;
    CHECK(argc==3); FILE *file=fopen(argv[2],"wx"); CHECK(file!=NULL && fclose(file)==0);
    status=UmiBankOperationsOpenSqlite(argv[2],&f.bank);
    if(status==UMI_STATUS_UNAVAILABLE){CHECK(remove(argv[2])==0);return 77;} OK(status); Setup(&f);
    c=Interest(&f,"interest","2026-09"); Send(&f,&f.maker,&c); Step(&f,UMI_BANK_INTEREST_APPROVE,&f.checker);
    c=Make(&f,UMI_BANK_INTEREST_POST,"interest"); OK(UmiBankOperationsReview(f.bank,&f.operator,&c,&review));
    if(strcmp(argv[1],"restart")==0){
        OK(UmiBankOperationsExecuteReviewed(f.bank,&f.operator,review,&receipt)); UmiBankOperationsDestroy(f.bank); f.bank=NULL;
        OK(UmiBankOperationsOpenSqlite(argv[2],&f.bank)); Balance(&f,100410,0);
        UmiBankInterestRequest request; OK(UmiBankOperationsInterestAt(f.bank,0,&request)); CHECK(request.amount.minor_units==410 && request.state==UMI_BANK_TRANSFER_EXECUTED);
        OK(UmiBankOperationsExecute(f.bank,&f.operator,&c,&receipt)); CHECK(receipt.idempotent); Step(&f,UMI_BANK_INTEREST_REVERSE,&f.operator);
        UmiBankOperationsDestroy(f.bank); f.bank=NULL; OK(UmiBankOperationsOpenSqlite(argv[2],&f.bank)); Balance(&f,100000,0);
    }else if(strcmp(argv[1],"write-failure")==0){
        OK(umi_data_server_execute(f.bank->server,"CREATE TRIGGER interest_fail BEFORE UPDATE ON umicom_kv WHEN NEW.key='bank.operations.revision' BEGIN SELECT RAISE(ABORT,'interest failure'); END;"));
        CHECK(UmiBankOperationsExecuteReviewed(f.bank,&f.operator,review,&receipt)!=UMI_STATUS_OK && receipt.revision==0); Balance(&f,100000,0);
        OK(UmiBankOperationsReload(f.bank)); UmiBankInterestRequest request; OK(UmiBankOperationsInterestAt(f.bank,0,&request)); CHECK(request.state==UMI_BANK_TRANSFER_APPROVED);
        OK(umi_data_server_execute(f.bank->server,"DROP TRIGGER interest_fail;"));
        OK(UmiBankOperationsExecuteReviewed(f.bank,&f.operator,review,&receipt)); Balance(&f,100410,0);
    }else if(strcmp(argv[1],"stale-writer")==0){
        Fixture second={0}; OK(UmiBankOperationsOpenSqlite(argv[2],&second.bank)); second.operator=f.operator; second.serial=100;
        UmiBankCommand extra=Make(&second,UMI_BANK_TEST_CREDIT,"external"); Id(&extra.sourceAccountId,"account"); extra.amount=Cash(1); Send(&second,&second.operator,&extra);
        CHECK(UmiBankOperationsExecuteReviewed(f.bank,&f.operator,review,&receipt)==UMI_STATUS_BUSY && receipt.revision==0);
        Balance(&f,100000,0); OK(UmiBankOperationsReload(f.bank)); Balance(&f,100001,0); UmiBankOperationsDestroy(second.bank);
    }else if(strcmp(argv[1],"codec")==0){
        UmiBankAuditEvent event, decoded; char original[BANK_RECORD_TEXT_CAPACITY], encoded[BANK_RECORD_TEXT_CAPACITY];
        OK(UmiBankOperationsAuditAt(f.bank,3,&event)); OK(BankEncode(&event,original,sizeof original)); OK(BankDecode(original,&decoded));
        CHECK(decoded.command.interest.days==30 && decoded.command.interest.annualRateBps==500);
        OK(BankEncode(&decoded,encoded,sizeof encoded)); CHECK(strcmp(encoded,original)==0);
        original[strlen(original)-2]='f'; original[strlen(original)-1]='f'; CHECK(BankDecode(original,&decoded)==UMI_STATUS_PARSE_ERROR);
        OK(UmiBankOperationsAuditAt(f.bank,0,&event)); OK(BankEncode(&event,original,sizeof original)); OK(BankDecode(original,&decoded));
        CHECK(decoded.command.interest.days==0); OK(BankEncode(&decoded,encoded,sizeof encoded)); CHECK(strcmp(encoded,original)==0);
        strcat(original,"00000000"); CHECK(BankDecode(original,&decoded)==UMI_STATUS_PARSE_ERROR);
    }else CHECK(0);
    UmiBankReviewDestroy(review); UmiBankOperationsDestroy(f.bank); CHECK(remove(argv[2])==0); return 0;
}
