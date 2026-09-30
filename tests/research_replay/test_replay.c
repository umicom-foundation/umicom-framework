/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/research_replay/test_replay.c
 * PURPOSE:
 *   Check research replay state and recorded market-data handling.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT */
#include "umicom/strategy_research/research_replay.h"
#include "umicom/strategy_research/research_csv.h"
#include "strategy.h"
#include <float.h>
#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if(!(x)) {fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#x);return 1;} } while(0)
#define OK(x) CHECK((x)==UMI_STATUS_OK)
#define CLOSE(a,b) CHECK(fabs((a)-(b))<1e-8)
static UmiStatus Hold(const UmiResearchView *v,void *d,UmiResearchDirection *t)
{(void)v;(void)d;*t=UMI_RESEARCH_HOLD;return UMI_STATUS_OK;}
static UmiStatus Schedule(const UmiResearchView *v,void *d,UmiResearchDirection *t)
{const int *directions=d;*t=(UmiResearchDirection)directions[v->observationIndex];return UMI_STATUS_OK;}
static UmiStatus Fail(const UmiResearchView *v,void *d,UmiResearchDirection *t)
{(void)v;(void)d;(void)t;return UMI_STATUS_CANCELLED;}
static UmiStatus Invalid(const UmiResearchView *v,void *d,UmiResearchDirection *t)
{(void)v;(void)d;*t=(UmiResearchDirection)17;return UMI_STATUS_OK;}
static UmiStatus Reenter(const UmiResearchView *v,void *d,UmiResearchDirection *t)
{
    UmiResearchReplay *r=*(UmiResearchReplay **)d;(void)v;*t=UMI_RESEARCH_HOLD;
    return UmiResearchReplayStep(r)==UMI_STATUS_INVALID_STATE&&
        UmiResearchReplayCancel(r)==UMI_STATUS_INVALID_STATE?UMI_STATUS_OK:UMI_STATUS_INTERNAL_ERROR;
}
static int Run(int mode,UmiResearchConfig *c,UmiResearchObservation *e,size_t n,
    UmiResearchStrategy f,void *u,UmiResearchSnapshot *s)
{
    UmiResearchReplay *r=NULL;OK(UmiResearchReplayCreate(c,e,n,f,u,&r));
    if(mode) {for(size_t i=0;i<n;i++) OK(UmiResearchReplayStep(r));}
    else {size_t done=0;OK(UmiResearchReplayRun(r,n,&done));CHECK(done==n);}
    OK(UmiResearchReplaySnapshot(r,s));UmiResearchReplayDestroy(r);return 0;
}
static int Basic(const char *name)
{
    UmiResearchConfig c=UmiResearchConfigDefault();UmiResearchObservation e[8];PracticeQuotes(e);
    PracticeThresholds p={100,102};UmiResearchReplay *r=NULL;UmiResearchSnapshot s;
    if(strcmp(name,"invalid_arguments")==0) {
        CHECK(UmiResearchReplayCreate(NULL,e,8,Hold,NULL,&r)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(r==NULL);CHECK(UmiResearchReplayStep(NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiResearchReplaySnapshot(NULL,&s)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiResearchReplayCreate(&c,e,0,Hold,NULL,&r)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiResearchReplayCreate(&c,e,8,NULL,NULL,&r)==UMI_STATUS_INVALID_ARGUMENT);
    } else if(strcmp(name,"config_finite")==0) {
        double bad[4]={NAN,INFINITY,-1,0};
        for(size_t i=0;i<4;i++) {c.initialEquity=bad[i];CHECK(UmiResearchReplayCreate(&c,e,8,Hold,NULL,&r)!=UMI_STATUS_OK);}
        c=UmiResearchConfigDefault();c.slippageBps=1001;CHECK(UmiResearchReplayCreate(&c,e,8,Hold,NULL,&r)!=UMI_STATUS_OK);
        c.slippageBps=0;c.commissionPerUnit=NAN;CHECK(UmiResearchReplayCreate(&c,e,8,Hold,NULL,&r)!=UMI_STATUS_OK);
        c.commissionPerUnit=0;memset(c.strategyTag,'x',sizeof c.strategyTag);CHECK(UmiResearchReplayCreate(&c,e,8,Hold,NULL,&r)!=UMI_STATUS_OK);
    } else if(strcmp(name,"identity")==0) {
        strcpy(e[3].quote.instrument.symbol,"OTHER");CHECK(UmiResearchReplayCreate(&c,e,8,Hold,NULL,&r)!=UMI_STATUS_OK);
        PracticeQuotes(e);e[0].quote.instrument.multiplier=10;CHECK(UmiResearchReplayCreate(&c,e,8,Hold,NULL,&r)!=UMI_STATUS_OK);
        PracticeQuotes(e);memset(e[0].quote.instrument.symbol,'a',sizeof e[0].quote.instrument.symbol);
        CHECK(UmiResearchReplayCreate(&c,e,8,Hold,NULL,&r)!=UMI_STATUS_OK);
    } else if(strcmp(name,"sequence")==0) {
        e[3].sequence++;CHECK(UmiResearchReplayCreate(&c,e,8,Hold,NULL,&r)!=UMI_STATUS_OK);
        PracticeQuotes(e);e[0].sequence=0;CHECK(UmiResearchReplayCreate(&c,e,8,Hold,NULL,&r)!=UMI_STATUS_OK);
        PracticeQuotes(e);e[2].sequence=e[1].sequence;CHECK(UmiResearchReplayCreate(&c,e,8,Hold,NULL,&r)!=UMI_STATUS_OK);
    } else if(strcmp(name,"timestamp")==0) {
        e[3].quote.event_time_ms=50;CHECK(UmiResearchReplayCreate(&c,e,8,Hold,NULL,&r)!=UMI_STATUS_OK);
        PracticeQuotes(e);e[0].quote.event_time_ms=-1;CHECK(UmiResearchReplayCreate(&c,e,8,Hold,NULL,&r)!=UMI_STATUS_OK);
        PracticeQuotes(e);for(size_t i=0;i<8;i++) e[i].quote.event_time_ms=0;
        CHECK(Run(0,&c,e,8,PracticeStrategy,&p,&s)==0);CHECK(s.closedTrades.tradeCount==2);
    } else if(strcmp(name,"copied_input")==0) {
        OK(UmiResearchReplayCreate(&c,e,8,PracticeStrategy,&p,&r));memset(e,0,sizeof e);
        size_t done;OK(UmiResearchReplayRun(r,8,&done));OK(UmiResearchReplaySnapshot(r,&s));CLOSE(s.closedTrades.netProfit,4.6);
    } else if(strcmp(name,"next_observation")==0) {
        OK(UmiResearchReplayCreate(&c,e,8,PracticeStrategy,&p,&r));OK(UmiResearchReplayStep(r));
        OK(UmiResearchReplaySnapshot(r,&s));CHECK(s.position==0&&s.hasPending&&s.fills==0);
        UmiResearchTrace t;CHECK(UmiResearchReplayTraceAt(r,1,&t)==UMI_STATUS_NOT_FOUND);
        OK(UmiResearchReplayStep(r));OK(UmiResearchReplayTraceAt(r,1,&t));CLOSE(t.fillPrice,101);CLOSE(t.signedFillUnits,1);
    } else if(strcmp(name,"latency")==0) {
        c.latencyMilliseconds=150;OK(UmiResearchReplayCreate(&c,e,8,PracticeStrategy,&p,&r));
        OK(UmiResearchReplayStep(r));OK(UmiResearchReplayStep(r));OK(UmiResearchReplaySnapshot(r,&s));CHECK(s.fills==0);
        OK(UmiResearchReplayStep(r));OK(UmiResearchReplaySnapshot(r,&s));CHECK(s.fills==1);CLOSE(s.entryPrice,104);
    } else if(strcmp(name,"commission")==0) {
        CHECK(Run(0,&c,e,8,PracticeStrategy,&p,&s)==0);CHECK(s.state==UMI_RESEARCH_COMPLETED);
        CHECK(s.closedTrades.tradeCount==2);CLOSE(s.closedTrades.netProfit,4.6);CLOSE(s.totalCommission,.4);CLOSE(s.equity,10004.6);
    } else if(strcmp(name,"slippage")==0) {
        c.slippageBps=10;CHECK(Run(0,&c,e,8,PracticeStrategy,&p,&s)==0);
        CLOSE(s.closedTrades.netProfit,4.6-.407);CLOSE(s.executionSlippage,.407);
        CLOSE(s.closedTrades.slippage,0); /* Already in execution prices. */
    } else if(strcmp(name,"short")==0) {
        const int d[8]={-1,2,0,2,2,2,2,2};CHECK(Run(0,&c,e,8,Schedule,(void *)d,&s)==0);
        CLOSE(s.closedTrades.netProfit,-5.2);CHECK(s.position==0);
    } else if(strcmp(name,"reversal")==0) {
        const int d[8]={1,2,-1,2,0,2,2,2};CHECK(Run(0,&c,e,8,Schedule,(void *)d,&s)==0);
        CHECK(s.closedTrades.tradeCount==2&&s.fills==3);CLOSE(s.totalCommission,.4);CLOSE(s.closedTrades.netProfit,6.6);
    } else if(strcmp(name,"liquidity")==0) {
        e[1].quote.ask_size=.5;OK(UmiResearchReplayCreate(&c,e,8,PracticeStrategy,&p,&r));
        OK(UmiResearchReplayStep(r));OK(UmiResearchReplayStep(r));OK(UmiResearchReplaySnapshot(r,&s));
        CHECK(s.position==0&&s.hasPending&&s.insufficientLiquidity==1);
        OK(UmiResearchReplayStep(r));OK(UmiResearchReplaySnapshot(r,&s));CLOSE(s.entryPrice,104);
    } else if(strcmp(name,"cancel")==0) {
        OK(UmiResearchReplayCreate(&c,e,8,Hold,NULL,&r));OK(UmiResearchReplayStep(r));OK(UmiResearchReplayCancel(r));
        CHECK(UmiResearchReplayStep(r)==UMI_STATUS_INVALID_STATE);OK(UmiResearchReplaySnapshot(r,&s));
        CHECK(s.state==UMI_RESEARCH_CANCELLED&&s.processed==1);
    } else if(strcmp(name,"callback_error")==0||strcmp(name,"invalid_decision")==0) {
        OK(UmiResearchReplayCreate(&c,e,8,strcmp(name,"callback_error")==0?Fail:Invalid,NULL,&r));
        CHECK(UmiResearchReplayStep(r)!=UMI_STATUS_OK);OK(UmiResearchReplaySnapshot(r,&s));
        CHECK(s.state==UMI_RESEARCH_FAILED&&s.processed==0&&s.fills==0);
        CHECK(UmiResearchReplayStep(r)==UMI_STATUS_INVALID_STATE);
    } else if(strcmp(name,"reentry")==0) {
        OK(UmiResearchReplayCreate(&c,e,8,Reenter,&r,&r));size_t done;OK(UmiResearchReplayRun(r,8,&done));CHECK(done==8);
    } else if(strcmp(name,"end_of_data")==0) {
        const int d[8]={2,2,2,2,2,2,2,1};CHECK(Run(0,&c,e,8,Schedule,(void *)d,&s)==0);
        CHECK(s.hasPending&&s.position==0&&s.fills==0);
    } else if(strcmp(name,"open_position")==0) {
        const int d[8]={1,2,2,2,2,2,2,2};CHECK(Run(0,&c,e,8,Schedule,(void *)d,&s)==0);
        CHECK(s.position==1&&s.closedTrades.tradeCount==0);CLOSE(s.totalCommission,.1);
        CLOSE(s.unrealisedPnl,1);CLOSE(s.equity,10000.9);
    } else if(strcmp(name,"step_budget")==0) {
        OK(UmiResearchReplayCreate(&c,e,8,Hold,NULL,&r));size_t done;
        OK(UmiResearchReplayRun(r,3,&done));CHECK(done==3);OK(UmiResearchReplaySnapshot(r,&s));CHECK(s.state==UMI_RESEARCH_RUNNING);
        OK(UmiResearchReplayRun(r,8,&done));CHECK(done==5);CHECK(UmiResearchReplayRun(r,1,&done)==UMI_STATUS_INVALID_STATE);
    } else if(strcmp(name,"numeric_failure")==0) {
        c.commissionPerUnit=DBL_MAX;c.units=2;const int d[8]={1,2,-1,2,2,2,2,2};
        OK(UmiResearchReplayCreate(&c,e,8,Schedule,(void *)d,&r));OK(UmiResearchReplayStep(r));
        CHECK(UmiResearchReplayStep(r)==UMI_STATUS_CAPACITY_EXCEEDED);OK(UmiResearchReplaySnapshot(r,&s));
        CHECK(s.processed==1&&s.fills==0&&s.state==UMI_RESEARCH_FAILED);
    } else if(strcmp(name,"timestamp_boundary")==0) {
        e[0].quote.event_time_ms=INT64_MAX;e[1].quote.event_time_ms=INT64_MAX;
        c.latencyMilliseconds=UINT32_MAX;int d[2]={1,2};CHECK(Run(0,&c,e,2,Schedule,d,&s)==0);
        CHECK(s.fills==0&&s.hasPending);
    } else if(strcmp(name,"limit")==0) {
        CHECK(UmiResearchReplayCreate(&c,e,UMI_RESEARCH_OBSERVATION_LIMIT+1U,Hold,NULL,&r)==UMI_STATUS_CAPACITY_EXCEEDED);
    } else if(strcmp(name,"pending_cancel")==0) {
        int d[8]={1,0,2,2,2,2,2,2};c.latencyMilliseconds=200;
        CHECK(Run(0,&c,e,8,Schedule,d,&s)==0);CHECK(s.fills==0&&!s.hasPending);
    } else if(strcmp(name,"fingerprint")==0) {
        UmiResearchSnapshot a,b;CHECK(Run(0,&c,e,8,Hold,NULL,&a)==0);CHECK(Run(1,&c,e,8,Hold,NULL,&b)==0);
        CHECK(strcmp(a.inputFingerprint,b.inputFingerprint)==0);
        c.latencyMilliseconds=1;CHECK(Run(0,&c,e,8,Hold,NULL,&b)==0);CHECK(strcmp(a.inputFingerprint,b.inputFingerprint)!=0);
        c.latencyMilliseconds=0;e[7].quote.bid+=.2;CHECK(Run(0,&c,e,8,Hold,NULL,&b)==0);CHECK(strcmp(a.inputFingerprint,b.inputFingerprint)!=0);
    } else if(strcmp(name,"determinism")==0) {
        UmiResearchSnapshot a,b;CHECK(Run(0,&c,e,8,PracticeStrategy,&p,&a)==0);CHECK(Run(1,&c,e,8,PracticeStrategy,&p,&b)==0);
        CLOSE(a.equity,b.equity);CHECK(a.fills==b.fills&&a.processed==b.processed);
    } else if(strcmp(name,"no_future")==0) {
        UmiResearchObservation altered[8];memcpy(altered,e,sizeof e);altered[7].quote.bid=900;altered[7].quote.ask=901;
        UmiResearchReplay *b=NULL;OK(UmiResearchReplayCreate(&c,e,8,PracticeStrategy,&p,&r));
        OK(UmiResearchReplayCreate(&c,altered,8,PracticeStrategy,&p,&b));
        for(size_t i=0;i<7;i++) {OK(UmiResearchReplayStep(r));OK(UmiResearchReplayStep(b));
            UmiResearchTrace x,y;OK(UmiResearchReplayTraceAt(r,i,&x));OK(UmiResearchReplayTraceAt(b,i,&y));
            CLOSE(x.equity,y.equity);CLOSE(x.fillPrice,y.fillPrice);CHECK(x.position==y.position&&x.decision==y.decision);}
        UmiResearchReplayDestroy(b);
    } else return 2;
    UmiResearchReplayDestroy(r);return 0;
}
static int Accumulator(const char *name)
{
    UmiStrategyBacktestState s,previous;UmiStrategyBacktestSnapshot output;
    OK(umi_strategy_backtest_state_init(&s,10000));previous=s;
    if(strcmp(name,"backtest_nonfinite")==0) {
        const double bad[2]={NAN,INFINITY};
        for(size_t j=0;j<2;j++) {
            CHECK(umi_strategy_backtest_state_init(&s,bad[j])==UMI_STATUS_INVALID_ARGUMENT);
            CHECK(memcmp(&s,&previous,sizeof s)==0);
            for(size_t i=0;i<5;i++) {
                double v[5]={100,110,10,2,1};v[i]=bad[j];
                CHECK(umi_strategy_backtest_add_trade(&s,v[0],v[1],v[2],v[3],v[4],0,1)==UMI_STATUS_INVALID_ARGUMENT);
                CHECK(memcmp(&s,&previous,sizeof s)==0);
            }
        }
    } else if(strcmp(name,"backtest_overflow")==0) {
        CHECK(umi_strategy_backtest_add_trade(&s,DBL_MAX,DBL_MAX,DBL_MAX,0,0,0,1)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(memcmp(&s,&previous,sizeof s)==0);
        s.exposureMilliseconds=UINT64_MAX;previous=s;
        CHECK(umi_strategy_backtest_add_trade(&s,100,101,1,0,0,0,1)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(memcmp(&s,&previous,sizeof s)==0);
        s.tradeCount=UINT64_MAX;s.winCount=UINT64_MAX;previous=s;
        CHECK(umi_strategy_backtest_add_trade(&s,100,101,1,0,0,0,0)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(memcmp(&s,&previous,sizeof s)==0);
    } else if(strcmp(name,"backtest_snapshot")==0) {
        memset(&output,0x5a,sizeof output);UmiStrategyBacktestSnapshot before=output;
        s.equity=NAN;CHECK(umi_strategy_backtest_snapshot(&s,&output)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(&output,&before,sizeof output)==0);
    } else if(strcmp(name,"backtest_legacy")==0) {
        OK(umi_strategy_backtest_add_trade(&s,100,110,10,2,1,1000,2000));
        OK(umi_strategy_backtest_add_trade(&s,110,105,10,2,1,2100,3100));
        OK(umi_strategy_backtest_snapshot(&s,&output));CLOSE(output.netProfit,44);
        CLOSE(output.grossProfit,97);CLOSE(output.grossLoss,-53);CLOSE(output.maxDrawdown,53);
        CHECK(output.tradeCount==2&&output.winCount==1&&output.lossCount==1);CLOSE(output.winRate,50);
    } else return 2;
    return 0;
}
static int Csv(const char *name)
{
    const char *header="sequence,time_ms,bid,ask,bid_size,ask_size\n";
    char data[1024];UmiInstrument i=PracticeInstrument();UmiResearchObservation *e=NULL;size_t n=0,line=0;
    if(strcmp(name,"csv_valid")==0) {
        snprintf(data,sizeof data,"%s1,0,99,100,10,10\r\n2,100,100,101,10,10",header);
        OK(UmiResearchParseCsv(data,strlen(data),&i,&e,&n,&line));CHECK(n==2);CLOSE(e[0].quote.bid,99);free(e);
    } else if(strcmp(name,"csv_reject")==0) {
        const char *rows[]={"1,0,nan,100,1,1","1,0,99,inf,1,1","18446744073709551616,0,99,100,1,1",
            "1,9223372036854775808,99,100,1,1","1,0,0x1p2,100,1,1","1,0,99,100,1,1,extra",
            "1,0,99,100,1","1,0,1e999,100,1,1","1,0,99,100,1,1\n\n","1,0,\"99\",100,1,1"};
        for(size_t j=0;j<sizeof rows/sizeof rows[0];j++) {
            snprintf(data,sizeof data,"%s%s",header,rows[j]);
            CHECK(UmiResearchParseCsv(data,strlen(data),&i,&e,&n,&line)==UMI_STATUS_PARSE_ERROR);CHECK(e==NULL&&n==0&&line>=2);
        }
    } else if(strcmp(name,"csv_limits")==0) {
        CHECK(UmiResearchParseCsv(header,UMI_RESEARCH_CSV_BYTE_LIMIT+1U,&i,&e,&n,&line)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(UmiResearchParseCsv("a\0b",3,&i,&e,&n,&line)==UMI_STATUS_PARSE_ERROR);
        CHECK(UmiResearchParseCsv(header,strlen(header),&i,&e,&n,&line)==UMI_STATUS_PARSE_ERROR);
    } else return 2;
    return 0;
}
static UmiStatus Cycle(const UmiResearchView *v,void *d,UmiResearchDirection *t)
{
    (void)d;*t=v->observationIndex%4U==0?UMI_RESEARCH_LONG:
        v->observationIndex%4U==2?UMI_RESEARCH_FLAT:UMI_RESEARCH_HOLD;
    return UMI_STATUS_OK;
}
static int Reference(void)
{
    size_t count=4000;UmiResearchObservation *e=calloc(count,sizeof *e);CHECK(e!=NULL);
    uint32_t state=123;long double expected=0;double entry=0;
    for(size_t n=0;n<count;n++) {
        state=state*1664525U+1013904223U;
        double bid=90.0+(double)(state%2000U)/100.0;
        e[n].quote.instrument=PracticeInstrument();e[n].sequence=(uint64_t)n+1U;
        e[n].quote.event_time_ms=(int64_t)n*10;e[n].quote.bid=bid;e[n].quote.ask=bid+.25;
        e[n].quote.bid_size=100;e[n].quote.ask_size=100;
        if(n%4==1) entry=bid+.25;
        if(n%4==3) expected+=(long double)bid-(long double)entry-.2L;
    }
    UmiResearchConfig c=UmiResearchConfigDefault();UmiResearchSnapshot s;
    CHECK(Run(0,&c,e,count,Cycle,NULL,&s)==0);CHECK(s.closedTrades.tradeCount==1000);
    CHECK(fabsl((long double)s.closedTrades.netProfit-expected)<1e-8L);free(e);return 0;
}
static int Mutations(void)
{
    const char *original="sequence,time_ms,bid,ask,bid_size,ask_size\n1,0,99,100,10,10\n";
    UmiInstrument i=PracticeInstrument();uint32_t rng=77;
    for(size_t n=0;n<5000;n++) {
        char data[128];strcpy(data,original);rng=rng*1664525U+1013904223U;
        size_t at=rng%strlen(data);data[at]=(char)((rng>>24)&127U);
        UmiResearchObservation *e=NULL;size_t count,line;
        UmiStatus s=UmiResearchParseCsv(data,strlen(original),&i,&e,&count,&line);
        if(s==UMI_STATUS_OK) {CHECK(count==1);free(e);} else CHECK(e==NULL&&count==0);
    }
    return 0;
}
int main(int argc,char **argv)
{
    if(argc!=2) return 2;
    if(strcmp(argv[1],"independent_reference")==0) return Reference();
    if(strcmp(argv[1],"parser_mutations")==0) return Mutations();
    if(strncmp(argv[1],"backtest_",9)==0) return Accumulator(argv[1]);
    if(strncmp(argv[1],"csv_",4)==0) return Csv(argv[1]);
    return Basic(argv[1]);
}
