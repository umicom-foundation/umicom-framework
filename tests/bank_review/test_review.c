/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_review/test_review.c
 * PURPOSE:
 *   Test the actual service, repository and Data Server; no financial mock engine.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Test the actual service, repository and Data Server; no financial mock engine.
 */
#include "fixture.h"
#include "../../src/bank_operations/internal.h"
#include <limits.h>

static void FailUnchanged(Fixture *f,UmiBankCommand c,UmiStatus expected)
{
 UmiBankCounts before,after;UmiBankReview *r=(UmiBankReview *)(uintptr_t)1;
 OK(UmiBankOperationsCounts(f->bank,&before));
 CHECK(UmiBankOperationsReview(f->bank,&f->maker,&c,&r)==expected);CHECK(r==NULL);
 OK(UmiBankOperationsCounts(f->bank,&after));CHECK(before.revision==after.revision&&before.journals==after.journals);
}
static int NativeCase(const char *name)
{
 Fixture f={0};UmiBankReview *r=NULL;UmiBankReviewSnapshot *s=malloc(sizeof *s);
 UmiBankCommand c;UmiBankReceipt receipt;bool match;UmiBankCounts before,after;
 CHECK(s!=NULL);OK(UmiBankOperationsOpenMemory(&f.bank));Setup(&f);c=Transfer(&f);
 OK(UmiBankOperationsCounts(f.bank,&before));
 if(strcmp(name,"arguments")==0){
  CHECK(UmiBankOperationsReview(NULL,&f.maker,&c,&r)==UMI_STATUS_INVALID_ARGUMENT&&r==NULL);
  CHECK(UmiBankOperationsReview(f.bank,&f.maker,&c,NULL)==UMI_STATUS_INVALID_ARGUMENT);
  CHECK(UmiBankReviewMatches(NULL,&f.maker,&c,&match)==UMI_STATUS_INVALID_ARGUMENT&&!match);
  CHECK(UmiBankReviewSnapshotRead(NULL,s)==UMI_STATUS_INVALID_ARGUMENT);
  CHECK(UmiBankOperationsExecuteReviewed(f.bank,&f.maker,NULL,&receipt)==UMI_STATUS_INVALID_ARGUMENT&&receipt.revision==0);
  UmiBankReviewDestroy(NULL);
 } else if(strcmp(name,"insufficient")==0){c.amount.minor_units=100001;FailUnchanged(&f,c,UMI_STATUS_INVALID_STATE);
 } else if(strcmp(name,"invalid_currency")==0){memcpy(c.amount.currency.code,"USD",4);FailUnchanged(&f,c,UMI_STATUS_INVALID_ARGUMENT);
 } else if(strcmp(name,"self_approval")==0){Send(&f,&f.maker,&c);c=Make(&f,UMI_BANK_TRANSFER_APPROVE,"payment");FailUnchanged(&f,c,UMI_STATUS_PERMISSION_DENIED);
 } else if(strcmp(name,"unapproved_execution")==0){Send(&f,&f.maker,&c);c=Make(&f,UMI_BANK_TRANSFER_EXECUTE,"payment");FailUnchanged(&f,c,UMI_STATUS_INVALID_STATE);
 } else if(strcmp(name,"blocked_account")==0){UmiBankCommand block=Make(&f,UMI_BANK_ACCOUNT_SET_STATE,"payer");block.state=UMI_BANK_RECORD_BLOCKED;Send(&f,&f.maker,&block);c=Transfer(&f);FailUnchanged(&f,c,UMI_STATUS_INVALID_STATE);
 } else if(strcmp(name,"conflicting_request")==0){Send(&f,&f.maker,&c);c.amount.minor_units++;FailUnchanged(&f,c,UMI_STATUS_ALREADY_EXISTS);
 } else if(strcmp(name,"idempotent")==0){
  Send(&f,&f.maker,&c);OK(UmiBankOperationsReview(f.bank,&f.maker,&c,&r));OK(UmiBankReviewSnapshotRead(r,s));
  CHECK(s->alreadyRecorded&&s->before.revision==s->after.revision&&!s->hasJournal&&s->accountCount==0);
  OK(UmiBankOperationsExecuteReviewed(f.bank,&f.maker,r,&receipt));CHECK(receipt.idempotent&&receipt.revision==s->receiptRevision);Funds(&f,"payer",100000,25000);
 } else if(strcmp(name,"approval")==0||strcmp(name,"reject")==0||strcmp(name,"cancel")==0||strcmp(name,"execution")==0||strcmp(name,"reversal")==0){
  Send(&f,&f.maker,&c);
  if(strcmp(name,"reject")==0)c=Make(&f,UMI_BANK_TRANSFER_REJECT,"payment");
  else if(strcmp(name,"cancel")==0)c=Make(&f,UMI_BANK_TRANSFER_CANCEL,"payment");
  else c=Make(&f,UMI_BANK_TRANSFER_APPROVE,"payment");
  if(strcmp(name,"execution")==0||strcmp(name,"reversal")==0){
   Send(&f,&f.checker,&c);c=Make(&f,UMI_BANK_TRANSFER_EXECUTE,"payment");
   if(strcmp(name,"reversal")==0){Send(&f,&f.maker,&c);c=Make(&f,UMI_BANK_TRANSFER_REVERSE,"payment");}
  }
  OK(UmiBankOperationsReview(f.bank,&f.checker,&c,&r));OK(UmiBankReviewSnapshotRead(r,s));
  if(strcmp(name,"approval")==0)CHECK(!s->hasJournal&&s->accountCount==0);
  if(strcmp(name,"reject")==0||strcmp(name,"cancel")==0)CHECK(!s->hasJournal&&s->accountCount==1&&s->accounts[0].after.reserved.minor_units==0);
  if(strcmp(name,"execution")==0||strcmp(name,"reversal")==0){
   CHECK(s->hasJournal&&s->accountCount==2);CHECK(umi_accounting_journal_entry_balanced(&s->journal.entry));
   CHECK(s->journal.reversal==(strcmp(name,"reversal")==0));
  }
  CHECK(s->hasTransfer&&s->transferExistedBefore&&s->transferAfter.amount.minor_units==25000);
  OK(UmiBankOperationsExecuteReviewed(f.bank,&f.checker,r,&receipt));
  if(strcmp(name,"execution")==0){Funds(&f,"payer",75000,0);Funds(&f,"payee",25000,0);}
  else if(strcmp(name,"approval")==0)Funds(&f,"payer",100000,25000);
  else Funds(&f,"payer",100000,0);
 } else if(strcmp(name,"hold")==0||strcmp(name,"release")==0||strcmp(name,"capture")==0||strcmp(name,"refund")==0){
  c=Make(&f,UMI_BANK_HOLD_PLACE,"manual");strcpy(c.sourceAccountId.value,"payer");c.amount=Cash(10000);
  if(strcmp(name,"release")==0){Send(&f,&f.maker,&c);c=Make(&f,UMI_BANK_HOLD_RELEASE,"manual");}
  if(strcmp(name,"capture")==0||strcmp(name,"refund")==0){
   c=Make(&f,UMI_BANK_CARD_ISSUE,"card");strcpy(c.sourceAccountId.value,"payer");strcpy(c.name,"Practice card");c.amount=Cash(30000);Send(&f,&f.maker,&c);
   c=Make(&f,UMI_BANK_CARD_AUTHORISE,"authorisation");strcpy(c.ownerId.value,"card");c.amount=Cash(10000);Send(&f,&f.maker,&c);
   c=Make(&f,UMI_BANK_CARD_CAPTURE,"authorisation");c.amount=Cash(6000);
   if(strcmp(name,"refund")==0){Send(&f,&f.maker,&c);c=Make(&f,UMI_BANK_CARD_REFUND,"authorisation");}
  }
  OK(UmiBankOperationsReview(f.bank,&f.maker,&c,&r));OK(UmiBankReviewSnapshotRead(r,s));CHECK(s->accountCount==1);
  if(strcmp(name,"hold")==0)CHECK(s->accounts[0].after.reserved.minor_units==10000&&!s->hasJournal);
  if(strcmp(name,"capture")==0)CHECK(s->accounts[0].after.booked.minor_units==94000&&s->accounts[0].after.reserved.minor_units==0&&s->hasJournal);
  if(strcmp(name,"refund")==0)CHECK(s->hasJournal&&s->journal.reversal&&s->accounts[0].after.booked.minor_units==100000);
  OK(UmiBankOperationsExecuteReviewed(f.bank,&f.maker,r,&receipt));
 } else if(strcmp(name,"eligibility")==0||strcmp(name,"new_account")==0||strcmp(name,"customer_eligibility")==0){
  if(strcmp(name,"new_account")==0){c=Make(&f,UMI_BANK_ACCOUNT_OPEN,"second");strcpy(c.ownerId.value,"customer");strcpy(c.name,"Second account");c.amount=Cash(0);}
  else {c=Make(&f,strcmp(name,"eligibility")==0?UMI_BANK_ACCOUNT_SET_STATE:UMI_BANK_CUSTOMER_SET_STATE,strcmp(name,"eligibility")==0?"payer":"customer");c.state=UMI_BANK_RECORD_BLOCKED;}
  OK(UmiBankOperationsReview(f.bank,&f.maker,&c,&r));OK(UmiBankReviewSnapshotRead(r,s));CHECK(s->accountCount==1);
  if(strcmp(name,"new_account")==0)CHECK(!s->accounts[0].existedBefore&&s->accounts[0].activeAfter);
  else CHECK(s->accounts[0].activeBefore&&!s->accounts[0].activeAfter);
 } else if(strcmp(name,"reconcile_match")==0||strcmp(name,"reconcile_break")==0){
  c=Make(&f,UMI_BANK_RECONCILE,"statement-check");strcpy(c.sourceAccountId.value,"payer");c.amount=Cash(strcmp(name,"reconcile_match")==0?100000:99999);
  OK(UmiBankOperationsReview(f.bank,&f.maker,&c,&r));OK(UmiBankReviewSnapshotRead(r,s));CHECK(s->hasReconciliation&&!s->hasJournal&&s->accountCount==0);
  CHECK(s->reconciliation.matched==(strcmp(name,"reconcile_match")==0));Funds(&f,"payer",100000,0);
 } else {
  OK(UmiBankOperationsReview(f.bank,&f.maker,&c,&r));OK(UmiBankReviewSnapshotRead(r,s));
  if(strcmp(name,"preview")==0){CHECK(s->accountCount==1&&!s->hasJournal&&s->after.revision==before.revision+1);CHECK(s->hasTransfer&&!s->transferExistedBefore&&s->transferAfter.amount.minor_units==25000&&strcmp(s->transferAfter.destinationAccountId.value,"payee")==0);CHECK(s->accounts[0].after.available.minor_units==75000);Funds(&f,"payer",100000,0);}
  else if(strcmp(name,"apply")==0){OK(UmiBankOperationsExecuteReviewed(f.bank,&f.maker,r,&receipt));CHECK(receipt.revision==s->after.revision&&!receipt.idempotent);Funds(&f,"payer",100000,25000);}
  else if(strcmp(name,"copied_command")==0){c.amount.minor_units=50;OK(UmiBankReviewMatches(r,&f.maker,&c,&match));CHECK(!match);OK(UmiBankOperationsExecuteReviewed(f.bank,&f.maker,r,&receipt));Funds(&f,"payer",100000,25000);}
  else if(strcmp(name,"snapshot_copy")==0){s->accounts[0].after.booked.minor_units=999;OK(UmiBankReviewSnapshotRead(r,s));CHECK(s->accounts[0].after.booked.minor_units==100000);}
  else if(strcmp(name,"source_lifetime")==0){UmiBankOperationsDestroy(f.bank);f.bank=NULL;OK(UmiBankReviewSnapshotRead(r,s));CHECK(s->command.amount.minor_units==25000);}
  else if(strcmp(name,"revocation")==0||strcmp(name,"identity_change")==0||strcmp(name,"capability_change")==0){
   UmiBankActor actor=f.maker;if(strcmp(name,"revocation")==0)actor.capabilities=0;
   else if(strcmp(name,"identity_change")==0)Id(&actor.id,"different");else actor.capabilities=UMI_BANK_CAP_PAYMENTS;
   CHECK(UmiBankOperationsExecuteReviewed(f.bank,&actor,r,&receipt)==UMI_STATUS_PERMISSION_DENIED&&receipt.revision==0);Funds(&f,"payer",100000,0);
  } else if(strcmp(name,"stale_local")==0){UmiBankCommand extra=Make(&f,UMI_BANK_TEST_CREDIT,"extra");strcpy(extra.sourceAccountId.value,"payer");extra.amount=Cash(1);Send(&f,&f.maker,&extra);CHECK(UmiBankOperationsExecuteReviewed(f.bank,&f.maker,r,&receipt)==UMI_STATUS_BUSY&&receipt.revision==0);Funds(&f,"payer",100001,0);
  } else if(strcmp(name,"same_revision_other_history")==0||strcmp(name,"equivalent_history")==0){
   Fixture other={0};OK(UmiBankOperationsOpenMemory(&other.bank));Setup(&other);
   if(strcmp(name,"same_revision_other_history")==0)strcpy(other.bank->state->events[0].command.name,"Different origin");
   UmiStatus status=UmiBankOperationsExecuteReviewed(other.bank,&other.maker,r,&receipt);
   CHECK(status==(strcmp(name,"equivalent_history")==0?UMI_STATUS_OK:UMI_STATUS_BUSY));UmiBankOperationsDestroy(other.bank);
  } else if(strcmp(name,"repeat_apply")==0){OK(UmiBankOperationsExecuteReviewed(f.bank,&f.maker,r,&receipt));CHECK(UmiBankOperationsExecuteReviewed(f.bank,&f.maker,r,&receipt)==UMI_STATUS_BUSY&&receipt.revision==0);}
  else if(strcmp(name,"field_binding")==0){
   for(unsigned i=0;i<7;++i){UmiBankCommand changed=c;
    switch(i){case 0:changed.amount.minor_units++;break;case 1:changed.timestampMillis++;break;case 2:changed.businessDate.day--;break;
     case 3:changed.expectedRevision++;break;case 4:Id(&changed.requestId,"another-request");break;case 5:Id(&changed.id,"other-payment");break;default:Id(&changed.ownerId,"other-beneficiary");break;}
    OK(UmiBankReviewMatches(r,&f.maker,&changed,&match));CHECK(!match);
   }
   OK(UmiBankReviewMatches(r,&f.maker,&c,&match));CHECK(match);
  } else if(strcmp(name,"text")==0||strcmp(name,"text_capacity")==0){
   size_t required=0;CHECK(UmiBankReviewDescribe(r,NULL,0,&required)==UMI_STATUS_CAPACITY_EXCEEDED&&required>1);
   char *text=malloc(required);CHECK(text!=NULL);OK(UmiBankReviewDescribe(r,text,required,NULL));CHECK(strstr(text,"GBP 250.00")&&strstr(text,"predicted revision")&&strstr(text,"From payer to payee")&&strlen(text)+1==required);
   if(strcmp(name,"text_capacity")==0){text[0]='x';CHECK(UmiBankReviewDescribe(r,text,required-1,NULL)==UMI_STATUS_CAPACITY_EXCEEDED&&text[0]=='\0');}
   free(text);
  } else if(strcmp(name,"draft_discard")==0){UmiBankReviewDestroy(r);r=NULL;Funds(&f,"payer",100000,0);OK(UmiBankOperationsCounts(f.bank,&after));CHECK(after.revision==before.revision);}
  else if(strcmp(name,"review_not_reservation")==0){
   UmiBankCommand hold=Make(&f,UMI_BANK_HOLD_PLACE,"spend-first");strcpy(hold.sourceAccountId.value,"payer");hold.amount=Cash(90000);Send(&f,&f.maker,&hold);
   CHECK(UmiBankOperationsExecuteReviewed(f.bank,&f.maker,r,&receipt)==UMI_STATUS_BUSY);Funds(&f,"payer",100000,90000);
  } else CHECK(0);
 }
 UmiBankReviewDestroy(r);UmiBankOperationsDestroy(f.bank);free(s);return 0;
}

static int SqliteCase(const char *name,const char *path)
{
 Fixture f={0};UmiBankReview *r=NULL;UmiBankReceipt receipt;UmiBankCommand c;
 FILE *newFile=fopen(path,"wx");CHECK(newFile!=NULL);CHECK(fclose(newFile)==0);
 UmiStatus status=UmiBankOperationsOpenSqlite(path,&f.bank);
 if(status==UMI_STATUS_UNAVAILABLE){CHECK(remove(path)==0);return 77;}OK(status);Setup(&f);c=Transfer(&f);
 OK(UmiBankOperationsReview(f.bank,&f.maker,&c,&r));
 if(strcmp(name,"sqlite_restart")==0){
  OK(UmiBankOperationsExecuteReviewed(f.bank,&f.maker,r,&receipt));UmiBankOperationsDestroy(f.bank);f.bank=NULL;
  OK(UmiBankOperationsOpenSqlite(path,&f.bank));Funds(&f,"payer",100000,25000);
  UmiBankReviewDestroy(r);r=NULL;OK(UmiBankOperationsReview(f.bank,&f.maker,&c,&r));OK(UmiBankOperationsExecuteReviewed(f.bank,&f.maker,r,&receipt));CHECK(receipt.idempotent);
 } else if(strcmp(name,"sqlite_stale_writer")==0){
  Fixture other={0};OK(UmiBankOperationsOpenSqlite(path,&other.bank));other.maker=f.maker;other.serial=100;
  UmiBankCommand extra=Make(&other,UMI_BANK_TEST_CREDIT,"other-credit");strcpy(extra.sourceAccountId.value,"payer");extra.amount=Cash(1);Send(&other,&other.maker,&extra);
  CHECK(UmiBankOperationsExecuteReviewed(f.bank,&f.maker,r,&receipt)==UMI_STATUS_BUSY&&receipt.revision==0);Funds(&f,"payer",100000,0);
  OK(UmiBankOperationsReload(f.bank));CHECK(UmiBankOperationsExecuteReviewed(f.bank,&f.maker,r,&receipt)==UMI_STATUS_BUSY);Funds(&f,"payer",100001,0);UmiBankOperationsDestroy(other.bank);
 } else if(strcmp(name,"sqlite_write_failure")==0){
  OK(umi_data_server_execute(f.bank->server,"CREATE TRIGGER review_fail BEFORE UPDATE ON umicom_kv WHEN NEW.key='bank.operations.revision' BEGIN SELECT RAISE(ABORT,'test failure'); END;"));
  CHECK(UmiBankOperationsExecuteReviewed(f.bank,&f.maker,r,&receipt)!=UMI_STATUS_OK&&receipt.revision==0);Funds(&f,"payer",100000,0);
  OK(umi_data_server_execute(f.bank->server,"DROP TRIGGER review_fail;"));OK(UmiBankOperationsExecuteReviewed(f.bank,&f.maker,r,&receipt));Funds(&f,"payer",100000,25000);
 } else if(strcmp(name,"sqlite_review_read_only")==0){
  UmiBankCounts before,after;OK(UmiBankOperationsCounts(f.bank,&before));UmiBankReviewDestroy(r);r=NULL;
  OK(umi_data_server_execute(f.bank->server,"PRAGMA query_only=ON;"));OK(UmiBankOperationsReview(f.bank,&f.maker,&c,&r));
  CHECK(UmiBankOperationsExecuteReviewed(f.bank,&f.maker,r,&receipt)!=UMI_STATUS_OK);OK(UmiBankOperationsCounts(f.bank,&after));CHECK(after.revision==before.revision);
 } else if(strcmp(name,"sqlite_history_changed")==0||strcmp(name,"sqlite_history_missing")==0||strcmp(name,"sqlite_orphan")==0||strcmp(name,"sqlite_corrupt")==0){
  UmiDataServer *external=NULL;OK(umi_data_server_create_sqlite(path,&external));
  if(strcmp(name,"sqlite_history_changed")==0){
   UmiBankAuditEvent event;char text[BANK_RECORD_TEXT_CAPACITY];OK(UmiBankOperationsAuditAt(f.bank,0,&event));strcpy(event.command.name,"Changed outside the running service");OK(BankEncode(&event,text,sizeof text));
   OK(umi_data_server_set(external,"bank.operations.event.00000000000000000001",text));
  } else if(strcmp(name,"sqlite_history_missing")==0)OK(umi_data_server_delete(external,"bank.operations.event.00000000000000000001"));
  else if(strcmp(name,"sqlite_orphan")==0)OK(umi_data_server_set(external,"bank.operations.event.00000000000000000099","orphan"));
  else OK(umi_data_server_set(external,"bank.operations.event.00000000000000000001","invalid"));
  status=UmiBankOperationsExecuteReviewed(f.bank,&f.maker,r,&receipt);CHECK(status!=UMI_STATUS_OK&&receipt.revision==0);Funds(&f,"payer",100000,0);
  UmiBankCounts n;OK(UmiBankOperationsCounts(f.bank,&n));CHECK(n.revision==6);
  umi_data_server_destroy(external);
 } else CHECK(0);
 UmiBankReviewDestroy(r);UmiBankOperationsDestroy(f.bank);CHECK(remove(path)==0);return 0;
}

static void Reviewed(Fixture *f, const UmiBankActor *actor, UmiBankCommand *c)
{
 UmiBankReview *r=NULL;UmiBankReceipt receipt;UmiBankReviewSnapshot *s=malloc(sizeof *s);
 CHECK(s!=NULL);OK(UmiBankOperationsReview(f->bank,actor,c,&r));OK(UmiBankReviewSnapshotRead(r,s));
 OK(UmiBankOperationsExecuteReviewed(f->bank,actor,r,&receipt));CHECK(receipt.revision==s->after.revision);
 for(size_t i=0;i<s->accountCount;++i){UmiBankBalance b;OK(UmiBankOperationsBalance(f->bank,s->accounts[i].accountId.value,&b));
  CHECK(b.booked.minor_units==s->accounts[i].after.booked.minor_units&&b.reserved.minor_units==s->accounts[i].after.reserved.minor_units&&b.available.minor_units==s->accounts[i].after.available.minor_units);}
 free(s);UmiBankReviewDestroy(r);
}
static int ReferenceCase(const char *name)
{
 Fixture f={0};OK(UmiBankOperationsOpenMemory(&f.bank));Setup(&f);
 if(strcmp(name,"reference_cycles")==0){
  for(unsigned i=0;i<60U;++i){char id[32];(void)snprintf(id,sizeof id,"cycle-%u",i);UmiBankCommand c=Transfer(&f);Id(&c.id,id);
   c.amount=Cash((int64_t)i+1);Reviewed(&f,&f.maker,&c);Funds(&f,"payer",100000,(int64_t)i+1);Funds(&f,"payee",0,0);
   c=Make(&f,UMI_BANK_TRANSFER_APPROVE,id);Reviewed(&f,&f.checker,&c);Funds(&f,"payer",100000,(int64_t)i+1);
   c=Make(&f,UMI_BANK_TRANSFER_EXECUTE,id);Reviewed(&f,&f.maker,&c);Funds(&f,"payer",100000-((int64_t)i+1),0);Funds(&f,"payee",(int64_t)i+1,0);
   c=Make(&f,UMI_BANK_TRANSFER_REVERSE,id);Reviewed(&f,&f.maker,&c);Funds(&f,"payer",100000,0);Funds(&f,"payee",0,0);
  }
  UmiBankCounts n;OK(UmiBankOperationsCounts(f.bank,&n));CHECK(n.revision==246U&&n.transfers==60U&&n.journals==121U);
 }else if(strcmp(name,"maximum_accounts")==0){
  for(unsigned i=2;i<64U;++i){char id[32];(void)snprintf(id,sizeof id,"account-%u",i);UmiBankCommand c=Make(&f,UMI_BANK_ACCOUNT_OPEN,id);
   Id(&c.ownerId,"customer");strcpy(c.name,"Another account");c.amount=Cash(0);Send(&f,&f.maker,&c);}
  UmiBankCommand c=Make(&f,UMI_BANK_CUSTOMER_SET_STATE,"customer");c.state=UMI_BANK_RECORD_BLOCKED;UmiBankReview *r=NULL;
  UmiBankReviewSnapshot *s=malloc(sizeof *s);char *text=malloc(UMI_BANK_REVIEW_TEXT_CAPACITY);CHECK(s&&text);
  OK(UmiBankOperationsReview(f.bank,&f.maker,&c,&r));OK(UmiBankReviewSnapshotRead(r,s));CHECK(s->accountCount==63U);
  size_t required=0;OK(UmiBankReviewDescribe(r,text,UMI_BANK_REVIEW_TEXT_CAPACITY,&required));CHECK(required<UMI_BANK_REVIEW_TEXT_CAPACITY);
  CHECK(strstr(text,"account-63")!=NULL);Funds(&f,"payer",100000,0);free(s);free(text);UmiBankReviewDestroy(r);
 }else if(strcmp(name,"event_limit")==0){
  UmiBankCommand last={0};for(unsigned i=6;i<256U;++i){char id[32];(void)snprintf(id,sizeof id,"credit-%u",i);
   last=Make(&f,UMI_BANK_TEST_CREDIT,id);Id(&last.sourceAccountId,"payer");last.amount=Cash(1);Send(&f,&f.maker,&last);}
  UmiBankCommand c=Make(&f,UMI_BANK_TEST_CREDIT,"no-room");Id(&c.sourceAccountId,"payer");c.amount=Cash(1);FailUnchanged(&f,c,UMI_STATUS_CAPACITY_EXCEEDED);
  UmiBankReview *r=NULL;UmiBankReceipt receipt;OK(UmiBankOperationsReview(f.bank,&f.maker,&last,&r));OK(UmiBankOperationsExecuteReviewed(f.bank,&f.maker,r,&receipt));CHECK(receipt.idempotent);UmiBankReviewDestroy(r);Funds(&f,"payer",100250,0);
 }else CHECK(0);
 UmiBankOperationsDestroy(f.bank);return 0;
}

int main(int argc,char **argv)
{
 CHECK(argc>=2);
 if(strcmp(argv[1],"reference_cycles")==0||strcmp(argv[1],"maximum_accounts")==0||strcmp(argv[1],"event_limit")==0)return ReferenceCase(argv[1]);
 if(strncmp(argv[1],"sqlite_",7)==0){CHECK(argc==3);return SqliteCase(argv[1],argv[2]);}
 return NativeCase(argv[1]);
}
