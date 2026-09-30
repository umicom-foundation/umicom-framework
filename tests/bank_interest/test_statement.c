/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_interest/test_statement.c
 * PURPOSE: Verify readable revision ranges, opening balances and bounded output.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/bank_operations/statement_text.h"
int main(int argc, char **argv)
{
    Fixture f={0}; UmiBankCommand c; UmiBankCounts before,after; size_t required=0;
    char *text=malloc(UMI_BANK_STATEMENT_TEXT_CAPACITY); CHECK(argc==2 && text!=NULL);
    OK(UmiBankOperationsOpenMemory(&f.bank)); Setup(&f);
    c=Interest(&f,"interest","2026-09"); Send(&f,&f.maker,&c); Step(&f,UMI_BANK_INTEREST_APPROVE,&f.checker); Step(&f,UMI_BANK_INTEREST_POST,&f.operator);
    OK(UmiBankOperationsCounts(f.bank,&before)); CHECK(before.revision==6);
    if(strcmp(argv[1],"range")==0){
        OK(UmiBankOperationsDescribeStatement(f.bank,"account",6,6,text,UMI_BANK_STATEMENT_TEXT_CAPACITY,&required));
        CHECK(strstr(text,"Opening: GBP 1000.00") && strstr(text,"Closing: GBP 1004.10") && strstr(text,"Entries in this range: 1"));
        CHECK(strstr(text,"reference: interest") && strstr(text,"credit: GBP 4.10") && !strstr(text,"reference: funding"));
    }else if(strcmp(argv[1],"empty")==0){
        OK(UmiBankOperationsDescribeStatement(f.bank,"account",4,5,text,UMI_BANK_STATEMENT_TEXT_CAPACITY,NULL));
        CHECK(strstr(text,"Entries in this range: 0") && strstr(text,"Opening: GBP 1000.00") && strstr(text,"Closing: GBP 1000.00"));
    }else if(strcmp(argv[1],"capacity")==0){
        OK(UmiBankOperationsDescribeStatement(f.bank,"account",1,6,NULL,0,&required)); CHECK(required>1 && required<UMI_BANK_STATEMENT_TEXT_CAPACITY);
        CHECK(UmiBankOperationsDescribeStatement(f.bank,"account",1,6,text,required-1,NULL)==UMI_STATUS_CAPACITY_EXCEEDED && text[0]=='\0');
        OK(UmiBankOperationsDescribeStatement(f.bank,"account",1,6,text,required,NULL)); CHECK(strlen(text)+1==required);
    }else if(strcmp(argv[1],"invalid")==0){
        strcpy(text,"old"); CHECK(UmiBankOperationsDescribeStatement(f.bank,"missing",1,6,text,32,&required)==UMI_STATUS_NOT_FOUND && text[0]=='\0' && required==0);
        CHECK(UmiBankOperationsDescribeStatement(f.bank,"account",0,6,text,32,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiBankOperationsDescribeStatement(f.bank,"account",6,5,text,32,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiBankOperationsDescribeStatement(f.bank,"account",1,7,text,32,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiBankOperationsDescribeStatement(f.bank,"account",1,6,NULL,1,NULL)==UMI_STATUS_INVALID_ARGUMENT);
    }else if(strcmp(argv[1],"reversal")==0){
        Step(&f,UMI_BANK_INTEREST_REVERSE,&f.operator);
        OK(UmiBankOperationsDescribeStatement(f.bank,"account",6,7,text,UMI_BANK_STATEMENT_TEXT_CAPACITY,NULL));
        CHECK(strstr(text,"Opening: GBP 1000.00") && strstr(text,"Closing: GBP 1000.00"));
        CHECK(strstr(text,"Debit: GBP 4.10") && strstr(text,"Entries in this range: 2"));
        OK(UmiBankOperationsCounts(f.bank,&before));
    }else if(strcmp(argv[1],"snapshot")==0){
        OK(UmiBankOperationsDescribeStatement(f.bank,"account",1,6,text,UMI_BANK_STATEMENT_TEXT_CAPACITY,NULL));
        c=Make(&f,UMI_BANK_TEST_CREDIT,"new"); Id(&c.sourceAccountId,"account"); c.amount=Cash(1); Send(&f,&f.operator,&c);
        CHECK(strstr(text,"Captured service revision: 6") && strstr(text,"Closing: GBP 1004.10"));
        OK(UmiBankOperationsCounts(f.bank,&before));
    }else CHECK(0);
    OK(UmiBankOperationsCounts(f.bank,&after)); CHECK(after.revision==before.revision && after.journals==before.journals);
    free(text); UmiBankOperationsDestroy(f.bank); return 0;
}
