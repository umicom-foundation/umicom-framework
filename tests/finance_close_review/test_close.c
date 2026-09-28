/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Reuse the retained financial fixture and its complete helper implementation.
 * The separately registered original suite remains unchanged and executable. */
#define main UmiRetainedFinanceFixtureEntry
#include "../finance_operations/test_operations.c"
#undef main
#include "umicom/finance_operations/close_review.h"

static UmiFinanceCloseReview *Capture(Fixture *f, const char *period)
{
    UmiFinanceCloseReview *r = NULL; OK(UmiFinanceCloseReviewCreate(f->operations, period, &r)); CHECK(r); return r;
}
static UmiFinanceCloseReviewInfo Info(UmiFinanceCloseReview *r)
{
    UmiFinanceCloseReviewInfo i; OK(UmiFinanceCloseReviewGetInfo(r, &i)); return i;
}
static size_t Count(UmiFinanceCloseReview *r, UmiFinanceCloseIssueKind kind)
{
    size_t count = 0;
    for (size_t n = 0; n < Info(r).issueCount; ++n) {
        UmiFinanceCloseIssue issue; OK(UmiFinanceCloseReviewIssueAt(r, n, &issue));
        if (issue.kind == kind) ++count;
    }
    return count;
}
static void Case(const char *name, Fixture *f)
{
    UmiFinanceCloseReview *r = NULL;
    if (!strcmp(name,"arguments")) {
        CHECK(UmiFinanceCloseReviewCreate(NULL,"x",&r)==UMI_STATUS_INVALID_ARGUMENT && r==NULL);
        CHECK(UmiFinanceCloseReviewCreate(f->operations,NULL,&r)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiFinanceCloseReviewCreate(f->operations,"x",NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiFinanceCloseReviewCreate(f->operations,"",&r)!=UMI_STATUS_OK);
        CHECK(UmiFinanceCloseReviewCreate(f->operations,"missing",&r)==UMI_STATUS_NOT_FOUND);
        UmiFinanceCloseReviewDestroy(NULL); return;
    }
    if (!strcmp(name,"empty_period")) {
        Period(f,"september",9U);r=Capture(f,"september");CHECK(Info(r).checksPass && Info(r).currencyCount==0U);
        Begin(f,UMI_FINANCE_PREPARE_CLOSE,"september","preparer");Apply(f);
        UmiFinanceCloseReviewDestroy(r);return;
    }
    Setup(f);
    if (!strcmp(name,"initial_reconciliation")) {
        r=Capture(f,"september");CHECK(Count(r,UMI_FINANCE_CLOSE_MISSING_RECONCILIATION)==2U);
    } else if (!strcmp(name,"draft") || !strcmp(name,"approved")) {
        Journal(f,"new","control","buyer.cash",10);
        if (!strcmp(name,"approved")) {Begin(f,UMI_FINANCE_APPROVE_JOURNAL,"new","checker");Apply(f);}
        r=Capture(f,"september");CHECK(Count(r,UMI_FINANCE_CLOSE_UNPOSTED_JOURNAL)==1U);
    } else if (!strcmp(name,"open_order") || !strcmp(name,"cancel_order")) {
        Order(f,"buy","buyer",UMI_SIDE_BUY,100,1);
        if (!strcmp(name,"cancel_order")) Cancel(f,"buy","buyer");
        r=Capture(f,"september");CHECK(Count(r,UMI_FINANCE_CLOSE_OPEN_ORDER)==(!strcmp(name,"open_order")?1U:0U));
    } else if (!strcmp(name,"matched") || !strcmp(name,"cleared") || !strcmp(name,"settled")) {
        Match(f);
        if (!strcmp(name,"cleared")) {Begin(f,UMI_FINANCE_CLEAR_FILL,"system.fill.000001","clearing");Apply(f);}
        if (!strcmp(name,"settled")) Settle(f,"system.fill.000001");
        r=Capture(f,"september");CHECK(Count(r,UMI_FINANCE_CLOSE_OPEN_ORDER)==1U);
        CHECK(Count(r,UMI_FINANCE_CLOSE_UNSETTLED_FILL)==(!strcmp(name,"settled")?0U:1U));
    } else if (!strcmp(name,"mismatch")) {
        Reconcile(f,"bad","buyer.cash",49999);r=Capture(f,"september");
        CHECK(Count(r,UMI_FINANCE_CLOSE_MISMATCHED_RECONCILIATION)==1U);
    } else if (!strcmp(name,"stale")) {
        ReconcileAll(f);Journal(f,"more","control","buyer.cash",10);ApprovePost(f,"more");
        r=Capture(f,"september");CHECK(Count(r,UMI_FINANCE_CLOSE_STALE_RECONCILIATION)==2U);
    } else if (!strcmp(name,"latest_evidence")) {
        ReconcileAll(f);Reconcile(f,"bad.latest","buyer.cash",49999);r=Capture(f,"september");
        CHECK(Count(r,UMI_FINANCE_CLOSE_MISMATCHED_RECONCILIATION)==1U);
        UmiFinanceCloseIssue x;OK(UmiFinanceCloseReviewIssueAt(r,0,&x));CHECK(!strcmp(x.evidenceId.value,"bad.latest"));
    } else if (!strcmp(name,"prior_period")) {
        Period(f,"august",8U);r=Capture(f,"september");CHECK(Count(r,UMI_FINANCE_CLOSE_EARLIER_PERIOD)==1U);
    } else if (!strcmp(name,"future_order")) {
        Period(f,"october",10U);
        OrderCommand(f,"future","buyer",UMI_SIDE_BUY,100,1);f->command.date=(UmiFinancialDate){2026,10U,1U};Apply(f);
        r=Capture(f,"september");CHECK(Count(r,UMI_FINANCE_CLOSE_OPEN_ORDER)==0U);
    } else if (!strcmp(name,"end_date_inclusive")) {
        OrderCommand(f,"lastday","buyer",UMI_SIDE_BUY,100,1);f->command.date=(UmiFinancialDate){2026,9U,30U};Apply(f);
        r=Capture(f,"september");CHECK(Count(r,UMI_FINANCE_CLOSE_OPEN_ORDER)==1U);
    } else if (!strcmp(name,"two_currencies")) {
        Account(f,"usd.control",UMI_ACCOUNTING_ASSET,USD);Account(f,"usd.cash",UMI_ACCOUNTING_LIABILITY,USD);
        Journal(f,"usd","usd.control","usd.cash",12345);ApprovePost(f,"usd");
        r=Capture(f,"september");CHECK(Info(r).currencyCount==2U);
        UmiFinanceTrialBalance b;OK(UmiFinanceCloseReviewTrialBalanceAt(r,1,&b));
        CHECK(!strcmp(b.currency.code,"USD") && b.periodDebitMinor==12345 && b.periodCreditMinor==12345);
    } else if (!strcmp(name,"reversed_history")) {
        Begin(f,UMI_FINANCE_REVERSE_JOURNAL,"reverse","maker");Id(&f->command.referenceId,"funding");
        Id(&f->command.periodId,"september");f->command.date=DAY;Apply(f);ApprovePost(f,"reverse");
        r=Capture(f,"september");CHECK(Count(r,UMI_FINANCE_CLOSE_UNPOSTED_JOURNAL)==0U);
        UmiFinanceTrialBalance b;OK(UmiFinanceCloseReviewTrialBalanceAt(r,0,&b));
        CHECK(b.periodDebitMinor==100000 && b.closingDebitMinor==0);
    } else if (!strcmp(name,"immutable")) {
        r=Capture(f,"september");uint64_t old=Info(r).revision;ReconcileAll(f);
        CHECK(Info(r).revision==old && Info(r).issueCount==2U);
        UmiFinanceCloseReview *newer=Capture(f,"september");CHECK(Info(newer).issueCount==0U);UmiFinanceCloseReviewDestroy(newer);
    } else if (!strcmp(name,"outlives_service")) {
        r=Capture(f,"september");Destroy(f);CHECK(Info(r).issueCount==2U);
    } else if (!strcmp(name,"no_mutation")) {
        uint64_t rev=Revision(f);size_t count=umi_data_server_count(f->server);
        r=Capture(f,"september");CHECK(Revision(f)==rev && umi_data_server_count(f->server)==count);
    } else if (!strcmp(name,"format")) {
        r=Capture(f,"september");size_t size=0;CHECK(UmiFinanceCloseReviewFormat(r,NULL,0,&size)==UMI_STATUS_CAPACITY_EXCEEDED);
        char *text=malloc(size);CHECK(text);OK(UmiFinanceCloseReviewFormat(r,text,size,NULL));
        CHECK(strstr(text,"buyer.cash") && strstr(text,"blocking records: 2") && strstr(text,"balanced"));free(text);
    } else if (!strcmp(name,"format_capacity")) {
        r=Capture(f,"september");size_t size=0;CHECK(UmiFinanceCloseReviewFormat(r,NULL,0,&size)==UMI_STATUS_CAPACITY_EXCEEDED);
        char *text=malloc(size+1U);CHECK(text);
        for(size_t n=1;n<size;++n) {memset(text,'x',size+1U);CHECK(UmiFinanceCloseReviewFormat(r,text,n,NULL)==UMI_STATUS_CAPACITY_EXCEEDED);CHECK(text[0]==0 && text[n]=='x');}
        free(text);
    } else if (!strcmp(name,"query_bounds")) {
        r=Capture(f,"september");UmiFinanceCloseIssue issue={0},before={0};issue.status=UMI_STATUS_IO_ERROR;before=issue;
        CHECK(UmiFinanceCloseReviewIssueAt(r,SIZE_MAX,&issue)==UMI_STATUS_NOT_FOUND && !memcmp(&issue,&before,sizeof issue));
        CHECK(UmiFinanceCloseReviewGetInfo(r,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!strcmp(UmiFinanceCloseIssueText((UmiFinanceCloseIssueKind)99),"Unknown close issue"));
    } else if (!strcmp(name,"prepare_finalise") || !strcmp(name,"same_actor") || !strcmp(name,"closed")) {
        ReconcileAll(f);r=Capture(f,"september");CHECK(Info(r).canPrepare && !Info(r).canFinalise);UmiFinanceCloseReviewDestroy(r);
        Begin(f,UMI_FINANCE_PREPARE_CLOSE,"september","preparer");Apply(f);r=Capture(f,"september");
        CHECK(!Info(r).canPrepare && Info(r).canFinalise);
        if (!strcmp(name,"same_actor")) {Begin(f,UMI_FINANCE_CLOSE_PERIOD,"september","preparer");Reject(f,UMI_STATUS_PERMISSION_DENIED);}
        if (!strcmp(name,"closed")) {Begin(f,UMI_FINANCE_CLOSE_PERIOD,"september","closer");Apply(f);UmiFinanceCloseReviewDestroy(r);r=Capture(f,"september");CHECK(!Info(r).canPrepare && !Info(r).canFinalise);}
    } else if (!strcmp(name,"recheck_after_review")) {
        ReconcileAll(f);r=Capture(f,"september");CHECK(Info(r).canPrepare);
        Order(f,"after.review","buyer",UMI_SIDE_BUY,100,1);
        Begin(f,UMI_FINANCE_PREPARE_CLOSE,"september","preparer");Reject(f,UMI_STATUS_INVALID_STATE);
    } else if (!strcmp(name,"blocked_reload")) {
        OK(umi_data_server_set(f->server,"umicom.finance-operations.revision","broken"));
        CHECK(UmiFinanceOperationsReload(f->operations)!=UMI_STATUS_OK);
        r=Capture(f,"september");CHECK(Info(r).writesBlocked && !Info(r).canPrepare);
    } else if (!strcmp(name,"poisoned")) {
        f->operations->poisoned=true;CHECK(UmiFinanceCloseReviewCreate(f->operations,"september",&r)==UMI_STATUS_INVALID_STATE && !r);
    } else if (!strcmp(name,"corrupt_count")) {
        size_t old=f->operations->state->counts.accounts;f->operations->state->counts.accounts=SIZE_MAX;
        CHECK(UmiFinanceCloseReviewCreate(f->operations,"september",&r)==UMI_STATUS_PARSE_ERROR && !r);
        f->operations->state->counts.accounts=old;
    } else if (!strcmp(name,"corrupt_lines")) {
        f->operations->state->journals[0].entry.line_count=SIZE_MAX;
        CHECK(UmiFinanceCloseReviewCreate(f->operations,"september",&r)==UMI_STATUS_PARSE_ERROR && !r);
    } else if (!strcmp(name,"policy_agreement")) {
        for(size_t n=0;n<30U;++n) {
            r=Capture(f,"september");UmiStatus expected=Info(r).evaluationStatus;
            CHECK(FinanceEvaluateClose(f->operations->state,0,NULL)==expected);
            UmiFinanceCloseReviewDestroy(r);r=NULL;
            char id[40];(void)snprintf(id,sizeof id,"evidence.%zu",n);
            Reconcile(f,id,n%2U?"control":"buyer.cash",n%3U?50000:49999);
        }
    } else if (!strcmp(name,"stored_prefix_changed") || !strcmp(name,"stored_prefix_missing") ||
               !strcmp(name,"stored_prefix_extra")) {
        UmiFinanceOperationCommand original=f->operations->state->commands[4];
        char key[96],value[FINANCE_RECORD_CAPACITY];
        (void)snprintf(key,sizeof key,FINANCE_EVENT_PREFIX "%020u",5U);
        if (!strcmp(name,"stored_prefix_missing")) OK(umi_data_server_delete(f->server,key));
        else if (!strcmp(name,"stored_prefix_extra")) {
            OK(umi_data_server_set(f->server,FINANCE_EVENT_PREFIX "00000000000000000400","extra"));
        } else {
            original.lines[0].debitMinor=100000;original.lines[1].creditMinor=100000;
            OK(FinanceEncode(&original,value,sizeof value));OK(umi_data_server_set(f->server,key,value));
        }
        uint64_t revision=Revision(f);Begin(f,UMI_FINANCE_SET_MARKET_STATE,"practice","operator");
        Reject(f,!strcmp(name,"stored_prefix_changed")?UMI_STATUS_BUSY:UMI_STATUS_PARSE_ERROR);
        CHECK(f->receipt.revision==0U&&!umi_data_server_in_transaction(f->server));
        CHECK(Revision(f)==revision);int64_t balance=0;
        OK(UmiFinanceOperationsAccountBalance(f->operations,"buyer.cash",&balance));CHECK(balance==50000);
        if (!strcmp(name,"stored_prefix_changed")) {
            OK(UmiFinanceOperationsReload(f->operations));
            OK(UmiFinanceOperationsAccountBalance(f->operations,"buyer.cash",&balance));CHECK(balance==100000);
        }
    } else {fprintf(stderr,"unknown close test %s\n",name);exit(2);}
    UmiFinanceCloseReviewDestroy(r);
}
int main(int argc,char **argv)
{
    if(argc!=2) return 2;
    Fixture f;Create(&f,NULL);Case(argv[1],&f);Destroy(&f);
    printf("PASS %s\n",argv[1]);return 0;
}
