/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT */
#ifndef UMICOM_BANK_REVIEW_FIXTURE_H
#define UMICOM_BANK_REVIEW_FIXTURE_H
#include "umicom/bank_operations/review.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#x); exit(1); } } while(0)
#define OK(x) CHECK((x) == UMI_STATUS_OK)
typedef struct Fixture { UmiBankOperations *bank; uint64_t serial; UmiBankActor maker, checker; } Fixture;
static void Id(UmiFinancialId *id, const char *text) { OK(umi_financial_id_assign(id,text)); }
static UmiMoney Cash(int64_t amount) { UmiMoney m = {0}; memcpy(m.currency.code,"GBP",4);m.scale=2;m.minor_units=amount;return m; }
static UmiBankCommand Make(Fixture *f, UmiBankAction action, const char *id)
{
 UmiBankCommand c;UmiBankCounts n;OK(UmiBankOperationsCounts(f->bank,&n));
 UmiBankCommandInit(&c,action);Id(&c.id,id);
 (void)snprintf(c.requestId.value,sizeof c.requestId.value,"test-request-%" PRIu64,++f->serial);
 c.expectedRevision=n.revision;c.businessDate=(UmiFinancialDate){2026,9U,28U};c.timestampMillis=(int64_t)f->serial;return c;
}
static void Send(Fixture *f,const UmiBankActor *actor,UmiBankCommand *c)
{ UmiBankReceipt receipt; OK(UmiBankOperationsExecute(f->bank,actor,c,&receipt)); }
static void Setup(Fixture *f)
{
 UmiBankCommand c;f->maker.capabilities=f->checker.capabilities=UMI_BANK_CAP_ALL;
 Id(&f->maker.id,"maker");Id(&f->checker.id,"checker");
 for(unsigned i=0;i<2;++i){
  c=Make(f,UMI_BANK_CUSTOMER_CREATE,i==0?"customer":"supplier");strcpy(c.name,i==0?"Workshop":"Supplier");Send(f,&f->maker,&c);
  c=Make(f,UMI_BANK_ACCOUNT_OPEN,i==0?"payer":"payee");strcpy(c.ownerId.value,i==0?"customer":"supplier");strcpy(c.name,"Account");c.amount=Cash(0);Send(f,&f->maker,&c);
 }
 c=Make(f,UMI_BANK_BENEFICIARY_CREATE,"beneficiary");strcpy(c.ownerId.value,"customer");strcpy(c.destinationAccountId.value,"payee");strcpy(c.name,"Supplier");Send(f,&f->maker,&c);
 c=Make(f,UMI_BANK_TEST_CREDIT,"funding");strcpy(c.sourceAccountId.value,"payer");c.amount=Cash(100000);Send(f,&f->maker,&c);
}
static UmiBankCommand Transfer(Fixture *f)
{ UmiBankCommand c=Make(f,UMI_BANK_TRANSFER_SUBMIT,"payment");strcpy(c.sourceAccountId.value,"payer");strcpy(c.ownerId.value,"beneficiary");c.amount=Cash(25000);return c; }
static void Funds(Fixture *f,const char *id,int64_t booked,int64_t reserved)
{UmiBankBalance b;OK(UmiBankOperationsBalance(f->bank,id,&b));CHECK(b.booked.minor_units==booked&&b.reserved.minor_units==reserved&&b.available.minor_units==booked-reserved);}
#endif
