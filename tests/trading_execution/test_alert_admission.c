/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_execution/test_alert_admission.c
 *
 * PURPOSE:
 *   Verify alert revision admission, retained-rule validation and atomic child preparation during workspace removal.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../trading/test_trading_common.h"
#include "../../src/trading/trade_tape.c"
/* Intercept only the workspace allocation seam, not the alert implementation.
 * The failure switch is enabled immediately around the operation under test. */
static int refuse_workspace_allocation;
static void *AlertWorkspaceAllocate(size_t size)
{
    return refuse_workspace_allocation ? NULL : malloc(size);
}
#define malloc AlertWorkspaceAllocate
#include "../../src/trading/workspace.c"
#undef malloc
#define ALERT_CHECK(x) do { if (!(x)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#x); result=1; goto done; } } while(0)

static int DirectAlert(const char *name)
{
    int result=0;
    UmiTradingAlertBook *book=malloc(sizeof(*book)), *before=malloc(sizeof(*before));
    ALERT_CHECK(book!=NULL && before!=NULL);
    umi_trading_alert_book_init(book);
    const char *ids[]={"one","two","three"};
    for(size_t i=0;i<3U;++i) {
        UmiTradingPriceAlert alert;
        ALERT_CHECK(umi_trading_price_alert_init(&alert,ids[i],i==1U?"BBB":"AAA",
            UMI_TRADING_PRICE_ALERT_CROSSES_ABOVE,100,0)==UMI_STATUS_OK);
        ALERT_CHECK(umi_trading_price_alert_seed(&alert,90)==UMI_STATUS_OK);
        alert.last_observed_at_ms=1000;
        ALERT_CHECK(umi_trading_alert_book_add(book,&alert)==UMI_STATUS_OK);
    }
    UmiTradingPriceAlert addition;
    ALERT_CHECK(umi_trading_price_alert_init(&addition,"four","AAA",
        UMI_TRADING_PRICE_ALERT_CROSSES_ABOVE,110,0)==UMI_STATUS_OK);
    if(!strncmp(name,"ack:",4U))book->alerts[0].active=1;
    int last=strstr(name,":last")!=NULL, full=strstr(name,":exhausted")!=NULL;
    int corrupt=strstr(name,":corrupt")!=NULL, noop=strstr(name,":noop")||strstr(name,":absent")||strstr(name,":stale");
    book->revision=last?UINT64_MAX-1U:full||noop?UINT64_MAX:42U;
    if(corrupt)memset(book->alerts[2].instrument_id,'x',sizeof(book->alerts[2].instrument_id));
    if(!strcmp(name,"evaluate:occurrences"))book->alerts[0].occurrences=UINT64_MAX;
    int invalidCount=!strcmp(name,"add:count");
    if(invalidCount)book->count=UMI_TRADING_MAX_ALERTS+1U;
    memcpy(before,book,sizeof(*before));
    UmiStatus expected=corrupt||invalidCount?UMI_STATUS_INVALID_ARGUMENT:full?UMI_STATUS_CAPACITY_EXCEEDED:UMI_STATUS_OK;
    UmiStatus status;
    if(!strncmp(name,"add:",4U))status=umi_trading_alert_book_add(book,&addition);
    else if(!strncmp(name,"remove:",7U))status=umi_trading_alert_book_remove(book,"one");
    else if(!strncmp(name,"enable:",7U))status=umi_trading_alert_book_set_enabled(book,"one",noop?1:0);
    else if(!strncmp(name,"ack:",4U))status=umi_trading_alert_book_acknowledge(book,"one");
    else if(!strncmp(name,"batch:",6U))status=UmiTradingAlertBookRemoveInstrument(book,noop?"absent":"AAA");
    else status=umi_trading_alert_book_evaluate(book,strstr(name,":absent")?"absent":"AAA",101,strstr(name,":stale")?999:2000);
    ALERT_CHECK(status==expected);
    if(expected!=UMI_STATUS_OK || noop)ALERT_CHECK(!memcmp(before,book,sizeof(*book)));
    else {
        ALERT_CHECK(book->revision==before->revision+1U);
        if(!strncmp(name,"add:",4U))ALERT_CHECK(book->count==4U);
        if(!strncmp(name,"remove:",7U))ALERT_CHECK(book->count==2U && !strcmp(book->alerts[0].alert_id,"two"));
        if(!strncmp(name,"enable:",7U))ALERT_CHECK(!book->alerts[0].enabled);
        if(!strncmp(name,"ack:",4U))ALERT_CHECK(!book->alerts[0].active && book->alerts[0].acknowledged);
        if(!strncmp(name,"batch:",6U))ALERT_CHECK(book->count==1U && !strcmp(book->alerts[0].alert_id,"two"));
        if(!strncmp(name,"evaluate:",9U)) {
            ALERT_CHECK(book->alerts[0].active && book->alerts[2].active);
            ALERT_CHECK(book->alerts[0].last_value==101 && book->alerts[2].last_value==101);
            ALERT_CHECK(book->alerts[1].last_value==90 && book->alerts[1].last_observed_at_ms==1000);
            ALERT_CHECK(book->alerts[0].occurrences==(!strcmp(name,"evaluate:occurrences")?UINT64_MAX:1U));
        }
    }
done:
    free(before);free(book);return result;
}
static int WorkspaceAlert(const char *name)
{
    int result=0;
    UmiTradingWorkspace *w=NULL,*before=NULL;
    UmiTradingTradeTape *tapeBefore=NULL;
    UmiInstrument instrument=test_instrument();
    ALERT_CHECK(umi_trading_workspace_create(NULL,&w)==UMI_STATUS_OK);
    ALERT_CHECK(umi_trading_workspace_add_instrument(w,&instrument)==UMI_STATUS_OK);
    UmiQuote quote={0};quote.instrument=instrument;quote.bid=89;quote.ask=91;quote.event_time_ms=1000;
    ALERT_CHECK(umi_trading_workspace_update_quote(w,&quote)==UMI_STATUS_OK);
    int noAlerts=!strcmp(name,"workspace:no-alerts")||!strcmp(name,"workspace:quote-empty");
    int noTape=!strcmp(name,"workspace:no-tape");
    if(!noAlerts) {
        ALERT_CHECK(umi_trading_workspace_add_price_alert(w,"one",instrument.instrument_id.value,UMI_TRADING_PRICE_ALERT_CROSSES_ABOVE,100,0)==UMI_STATUS_OK);
        ALERT_CHECK(umi_trading_workspace_add_price_alert(w,"two",instrument.instrument_id.value,UMI_TRADING_PRICE_ALERT_CROSSES_ABOVE,105,0)==UMI_STATUS_OK);
    }
    if(!noTape) {
        UmiTradeTick tick={0};tick.instrument=instrument;tick.price=90;tick.size=1;tick.event_time_ms=1000;
        UmiTradingTradeTapeRecord record;
        ALERT_CHECK(umi_trading_trade_tape_record_init(&record,1,&tick,UMI_TRADING_TRADE_DIRECTION_UNKNOWN,"regular")==UMI_STATUS_OK);
        ALERT_CHECK(umi_trading_workspace_update_trade(w,&record)==UMI_STATUS_OK);
    }
    int last=!strcmp(name,"workspace:last"), allocation=!strcmp(name,"workspace:allocation");
    int quoteCase=!strncmp(name,"workspace:quote-",16U);
    w->revision=last?UINT64_MAX-1U:42U;
    w->alerts.revision=last?UINT64_MAX-1U:(!strcmp(name,"workspace:alert-capacity")||noAlerts||quoteCase)?UINT64_MAX:30U;
    w->trade_tape->revision=last?UINT64_MAX-1U:(!strcmp(name,"workspace:tape-capacity")||noTape)?UINT64_MAX:20U;
    before=malloc(sizeof(*before));tapeBefore=malloc(sizeof(*tapeBefore));
    ALERT_CHECK(before!=NULL && tapeBefore!=NULL);memcpy(before,w,sizeof(*before));memcpy(tapeBefore,w->trade_tape,sizeof(*tapeBefore));
    refuse_workspace_allocation=allocation;
    quote.bid=109;quote.ask=111;quote.event_time_ms=2000;
    UmiStatus status=quoteCase?umi_trading_workspace_update_quote(w,&quote):
        umi_trading_workspace_remove_instrument(w,instrument.instrument_id.value);
    refuse_workspace_allocation=0;
    UmiStatus expected=allocation?UMI_STATUS_OUT_OF_MEMORY:
        !strcmp(name,"workspace:alert-capacity")||!strcmp(name,"workspace:tape-capacity")||!strcmp(name,"workspace:quote-capacity")?UMI_STATUS_CAPACITY_EXCEEDED:UMI_STATUS_OK;
    ALERT_CHECK(status==expected);
    if(expected!=UMI_STATUS_OK) {
        ALERT_CHECK(!memcmp(before,w,sizeof(*w)));
        ALERT_CHECK(!memcmp(tapeBefore,w->trade_tape,sizeof(*tapeBefore)));
    } else {
        ALERT_CHECK(w->revision==before->revision+1U);
        if(quoteCase)ALERT_CHECK(w->markets[0].quote.bid==109 && w->alerts.revision==UINT64_MAX);
        else {
            ALERT_CHECK(w->market_count==0U && w->alerts.count==0U && w->trade_tape->count==0U);
            ALERT_CHECK(w->alerts.revision==(noAlerts?before->alerts.revision:before->alerts.revision+1U));
            ALERT_CHECK(w->trade_tape->revision==(noTape?tapeBefore->revision:tapeBefore->revision+1U));
        }
    }
done:
    refuse_workspace_allocation=0;
    free(tapeBefore);free(before);if(w!=NULL)umi_trading_workspace_destroy(w);
    return result;
}
int main(int argc,char **argv)
{
    static const char *known[]={"add:ordinary", "add:last", "add:exhausted", "add:corrupt", "remove:ordinary", "remove:last", "remove:exhausted", "remove:corrupt", "enable:ordinary", "enable:last", "enable:exhausted", "enable:corrupt", "ack:ordinary", "ack:last", "ack:exhausted", "ack:corrupt", "evaluate:ordinary", "evaluate:last", "evaluate:exhausted", "evaluate:corrupt", "batch:ordinary", "batch:last", "batch:exhausted", "batch:corrupt", "enable:noop", "batch:absent", "evaluate:absent", "evaluate:stale", "evaluate:occurrences", "add:count", "workspace:alert-capacity", "workspace:tape-capacity", "workspace:last", "workspace:allocation", "workspace:no-alerts", "workspace:no-tape", "workspace:quote-capacity", "workspace:quote-empty"};
    if(argc!=2)return 2;
    int recognized=0;for(size_t i=0;i<sizeof(known)/sizeof(known[0]);++i)if(!strcmp(argv[1],known[i]))recognized=1;
    if(!recognized)return 2;
    return !strncmp(argv[1],"workspace:",10U)?WorkspaceAlert(argv[1]):DirectAlert(argv[1]);
}
