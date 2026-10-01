/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_charges/test_contract.c
 * PURPOSE: Keep stable action numbers, canonical event encoding and statement effects visible.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "../../src/bank_operations/internal.h"
#include "umicom/bank_operations/statement_csv.h"
_Static_assert(UMI_BANK_RECONCILE==22 && UMI_BANK_INTEREST_SUBMIT==23 && UMI_BANK_INTEREST_REVERSE==28,
    "Historical bank action numbers must stay stable");
_Static_assert(UMI_BANK_CHARGE_SUBMIT==29 && UMI_BANK_CHARGE_REVERSE==34,"Charge action extension changed");
int main(int argc,char **argv)
{
    CHECK(argc==2); Fixture f={0}; OK(UmiBankOperationsOpenMemory(&f.bank)); Setup(&f);
    UmiBankCommand command=Charge(&f,"charge","service-reference"); Send(&f,&f.maker,&command);
    if(strcmp(argv[1],"codec")==0) {
        UmiBankAuditEvent event,decoded; char a[BANK_RECORD_TEXT_CAPACITY],b[BANK_RECORD_TEXT_CAPACITY];
        for(size_t index=0;index<4;++index) {
            OK(UmiBankOperationsAuditAt(f.bank,index,&event)); OK(BankEncode(&event,a,sizeof(a))); OK(BankDecode(a,&decoded));
            OK(BankEncode(&decoded,b,sizeof(b))); CHECK(strcmp(a,b)==0);
            if(index==3) CHECK(decoded.command.action==UMI_BANK_CHARGE_SUBMIT && decoded.command.amount.minor_units==250 && strcmp(decoded.command.name,"Practice service charge")==0);
        }
        event.command.ownerId.value[0]='\0'; OK(BankEncode(&event,a,sizeof(a))); CHECK(BankDecode(a,&decoded)==UMI_STATUS_PARSE_ERROR);
    } else if(strcmp(argv[1],"query")==0) {
        UmiBankChargeRequest copy; OK(UmiBankOperationsChargeAt(f.bank,0,&copy)); copy.amount.minor_units=1;
        OK(UmiBankOperationsChargeAt(f.bank,0,&copy)); CHECK(copy.amount.minor_units==250);
        CHECK(UmiBankOperationsChargeAt(f.bank,1,&copy)==UMI_STATUS_NOT_FOUND && copy.id.value[0]=='\0');
        CHECK(UmiBankOperationsChargeAt(NULL,0,&copy)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiBankOperationsChargeAt(f.bank,0,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiBankActionFields(UMI_BANK_CHARGE_SUBMIT)==(UMI_BANK_FIELD_OWNER|UMI_BANK_FIELD_SOURCE|UMI_BANK_FIELD_NAME|UMI_BANK_FIELD_MONEY));
        for(int action=UMI_BANK_CHARGE_APPROVE;action<=UMI_BANK_CHARGE_REVERSE;++action) {
            CHECK(UmiBankActionFields((UmiBankAction)action)==0 && strcmp(UmiBankActionName((UmiBankAction)action),"Unknown")!=0);
        }
    } else if(strcmp(argv[1],"statement")==0) {
        ChargeStep(&f,UMI_BANK_CHARGE_APPROVE,&f.checker); ChargeStep(&f,UMI_BANK_CHARGE_POST,&f.operator); ChargeStep(&f,UMI_BANK_CHARGE_REVERSE,&f.operator);
        UmiBankStatement *statement=malloc(sizeof(*statement)); CHECK(statement!=NULL);
        OK(UmiBankOperationsStatement(f.bank,"account",6,7,statement));
        CHECK(statement->count==2 && statement->opening.minor_units==100000 && statement->closing.minor_units==100000);
        CHECK(statement->lines[0].debitMinor==250 && statement->lines[0].creditMinor==0 && statement->lines[0].balanceMinor==99750);
        CHECK(statement->lines[1].creditMinor==250 && statement->lines[1].balanceMinor==100000);
        UmiCsvDocument *csv=NULL; OK(UmiBankOperationsExportStatementCsv(f.bank,"account",6,7,&csv));
        CHECK(strstr(UmiCsvDocumentData(csv),"charge")!=NULL && strstr(UmiCsvDocumentData(csv),"99750")!=NULL);
        UmiCsvDocumentDestroy(csv); free(statement);
    } else CHECK(0);
    UmiBankOperationsDestroy(f.bank); return 0;
}
