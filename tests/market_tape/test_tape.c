/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/market_tape/test_tape.c
 * PURPOSE:
 *   Tests use public contracts and explicit checks, including Release builds.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Umicom Foundation | Sammy Hegab | MIT
 * Tests use public contracts and explicit checks, including Release builds. */
#include "umicom/trading/market_tape.h"
#include "umicom/trading/market_tape_practice.h"
#include "umicom/trading/market_data_quality.h"
#include "umicom/platform/threading.h"
#include <float.h>
#include <math.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)
#define OK(x) CHECK((x) == UMI_STATUS_OK)
static UmiInstrument Instrument(const char *id)
{
    UmiInstrument i = {.symbol="ALPHA", .venue="PRACTICE", .currency={"GBP"}, .multiplier=1.0};
    (void)snprintf(i.instrument_id.value, sizeof(i.instrument_id.value), "%s", id);
    return i;
}
static UmiMarketTapePacket Quote(UmiInstrument i, uint64_t seq, int64_t time)
{
    UmiMarketTapePacket p = {0}; p.kind=UMI_MARKET_TAPE_QUOTE; p.generation=1U;
    p.sequence=seq; p.receivedTimeMs=time;
    p.value.quote=(UmiQuote){i,100.0,101.0,5.0,6.0,time}; return p;
}
static UmiMarketTapePacket Trade(UmiInstrument i, uint64_t seq, int64_t time, double price, double size)
{
    UmiMarketTapePacket p={0};p.kind=UMI_MARKET_TAPE_TRADE;p.generation=1U;p.sequence=seq;p.receivedTimeMs=time;
    p.value.trade=(UmiTradeTick){i,price,size,time};return p;
}
static UmiMarketTapePacket Depth(UmiInstrument i, uint64_t seq, int64_t time)
{
    UmiMarketTapePacket p={0};p.kind=UMI_MARKET_TAPE_DEPTH;p.generation=1U;p.sequence=seq;p.receivedTimeMs=time;
    p.value.depth.instrument=i;p.value.depth.event_time_ms=time;p.value.depth.bid_count=2U;p.value.depth.ask_count=2U;
    p.value.depth.bids[0]=(UmiDepthLevel){100.0,5.0};p.value.depth.bids[1]=(UmiDepthLevel){99.0,4.0};
    p.value.depth.asks[0]=(UmiDepthLevel){101.0,7.0};p.value.depth.asks[1]=(UmiDepthLevel){102.0,8.0};return p;
}
static int Unchanged(UmiMarketTape *tape,UmiMarketTapeSnapshot *before,UmiMarketTapeSnapshot *after,int64_t now)
{
    if (UmiMarketTapeRead(tape,now,after)!=UMI_STATUS_OK) return 0;
    return memcmp(before,after,sizeof(*after))==0;
}
static int Quality(int which)
{
    UmiQuote q=Quote(Instrument("A"),1U,0).value.quote;
    if(which==0){q.event_time_ms=INT64_MIN;CHECK(umi_market_data_quality_score(&q,INT64_MAX,INT64_MAX)==0.0);}
    if(which==1){q.event_time_ms=INT64_MAX;CHECK(umi_market_data_quality_score(&q,INT64_MIN,1)==0.0);}
    if(which==2){q.event_time_ms=-10;CHECK(umi_market_data_quality_score(&q,10,40)==0.5);}
    if(which==3){q.event_time_ms=10;CHECK(umi_market_data_quality_score(&q,30,20)==0.0);CHECK(umi_market_data_quality_score(&q,10,20)==1.0);}
    if(which==4){CHECK(umi_market_data_quality_score(NULL,0,100)==0.0);q.bid=NAN;CHECK(umi_market_data_quality_score(&q,0,100)==0.0);}
    if(which==5){CHECK(umi_market_data_quality_score(&q,0,0)==0.0);CHECK(umi_market_data_quality_score(&q,0,-1)==0.0);}
    if(which==6){
        for(int64_t e=-50;e<=50;++e)for(int64_t n=-50;n<=50;++n){
            q.event_time_ms=e;double expected=n<e||n-e>=20?0.0:1.0-(double)(n-e)/20.0;
            CHECK(umi_market_data_quality_score(&q,n,20)==expected);
        }
    }
    return 0;
}
typedef struct Worker { UmiMarketTape *tape; UmiInstrument i; atomic_bool finished; int result; } Worker;
static int Produce(void *data)
{
    Worker *w=data;w->result=0;
    for(uint64_t n=1U;n<=2000U;++n) {
        UmiMarketTapePacket p=Trade(w->i,n,(int64_t)n,100.0+(double)(n%5U),1.0);
        if(UmiMarketTapeApply(w->tape,&p,NULL)!=UMI_STATUS_OK){w->result=1;break;}
    }
    atomic_store_explicit(&w->finished,true,memory_order_release);return w->result;
}
static int Scenario(const char *name)
{
    UmiMarketTape *t=NULL;
    UmiMarketTapeConfig c=UmiMarketTapeConfigDefault();
    UmiInstrument a=Instrument("A"), b=Instrument("B");
    UmiMarketTapeSnapshot *v=calloc(1U,sizeof(*v)),*saved=calloc(1U,sizeof(*saved));
    UmiMarketTapeRejection why=UMI_MARKET_TAPE_BAD_INPUT;
    CHECK(v!=NULL&&saved!=NULL);
    if(strcmp(name,"config")==0){
        c.barIntervalMs=0;CHECK(UmiMarketTapeCreate(&c,&t)==UMI_STATUS_INVALID_ARGUMENT&&t==NULL);
        c.barIntervalMs=86400001;CHECK(UmiMarketTapeCreate(&c,&t)==UMI_STATUS_INVALID_ARGUMENT);
        c.barIntervalMs=1;c.staleAfterMs=0;CHECK(UmiMarketTapeCreate(&c,&t)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiMarketTapeCreate(NULL,&t)==UMI_STATUS_INVALID_ARGUMENT);CHECK(UmiMarketTapeCreate(&c,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        goto done;
    }
    OK(UmiMarketTapeCreate(&c,&t));
    if(strcmp(name,"empty")==0){OK(UmiMarketTapeRead(t,0,v));CHECK(v->rowCount==0U&&v->barCount==0U);goto done;}
    if(strcmp(name,"invalid_instrument")==0){
        UmiInstrument bad=a;memset(bad.instrument_id.value,'X',sizeof(bad.instrument_id.value));
        CHECK(UmiMarketTapeRegister(t,&bad)==UMI_STATUS_INVALID_ARGUMENT);
        bad=a;bad.symbol[0]='\n';CHECK(UmiMarketTapeRegister(t,&bad)==UMI_STATUS_INVALID_ARGUMENT);
        bad=a;bad.currency.code[3]='A';CHECK(UmiMarketTapeRegister(t,&bad)==UMI_STATUS_INVALID_ARGUMENT);
        bad=a;bad.multiplier=NAN;CHECK(UmiMarketTapeRegister(t,&bad)==UMI_STATUS_INVALID_ARGUMENT);goto done;
    }
    OK(UmiMarketTapeRegister(t,&a));
    if(strcmp(name,"capacity")==0){
        for(size_t k=1U;k<UMI_MARKET_TAPE_INSTRUMENT_LIMIT;++k){char id[20];(void)snprintf(id,sizeof(id),"I%zu",k);b=Instrument(id);OK(UmiMarketTapeRegister(t,&b));}
        b=Instrument("OVER");CHECK(UmiMarketTapeRegister(t,&b)==UMI_STATUS_CAPACITY_EXCEEDED);goto done;
    }
    if(strcmp(name,"duplicate_instrument")==0){CHECK(UmiMarketTapeRegister(t,&a)==UMI_STATUS_ALREADY_EXISTS);goto done;}
    if(strcmp(name,"unknown")==0){
        CHECK(UmiMarketTapeSelect(t,&b.instrument_id)==UMI_STATUS_NOT_FOUND);
        CHECK(UmiMarketTapeBegin(t,&b.instrument_id,1U,1U)==UMI_STATUS_NOT_FOUND);
        UmiMarketTapePacket p=Quote(b,1U,1000);CHECK(UmiMarketTapeApply(t,&p,&why)==UMI_STATUS_NOT_FOUND&&why==UMI_MARKET_TAPE_UNKNOWN_INSTRUMENT);goto done;
    }
    if(strcmp(name,"unopened")==0){
        UmiMarketTapePacket p=Quote(a,1U,1000);CHECK(UmiMarketTapeApply(t,&p,&why)==UMI_STATUS_INVALID_STATE);
        OK(UmiMarketTapeRead(t,1000,v));CHECK(v->rows[0].state==UMI_MARKET_TAPE_DISCONNECTED);goto done;
    }
    OK(UmiMarketTapeBegin(t,&a.instrument_id,1U,1U));
    UmiMarketTapePacket p=Quote(a,1U,1000);
    if(strcmp(name,"waiting")==0){OK(UmiMarketTapeRead(t,1000,v));CHECK(v->rows[0].state==UMI_MARKET_TAPE_WAITING&&!v->rows[0].haveQuote);goto done;}
    if(strcmp(name,"begin_validation")==0){
        CHECK(UmiMarketTapeBegin(t,&a.instrument_id,1U,1U)==UMI_STATUS_INVALID_STATE);
        CHECK(UmiMarketTapeBegin(t,&a.instrument_id,0U,1U)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiMarketTapeBegin(t,&a.instrument_id,2U,0U)==UMI_STATUS_INVALID_ARGUMENT);goto done;
    }
    if(strcmp(name,"bad_packets")==0){
        CHECK(UmiMarketTapeApply(t,NULL,&why)==UMI_STATUS_INVALID_ARGUMENT);
        p.kind=(UmiMarketTapeKind)999;CHECK(UmiMarketTapeApply(t,&p,&why)==UMI_STATUS_INVALID_ARGUMENT);
        p=Quote(a,0U,1000);CHECK(UmiMarketTapeApply(t,&p,&why)==UMI_STATUS_INVALID_ARGUMENT);
        p=Quote(a,1U,1000);p.generation=0U;CHECK(UmiMarketTapeApply(t,&p,&why)==UMI_STATUS_INVALID_ARGUMENT);goto done;
    }
    if(strcmp(name,"nonfinite_quote")==0){p.value.quote.ask=INFINITY;CHECK(UmiMarketTapeApply(t,&p,&why)==UMI_STATUS_INVALID_ARGUMENT);goto done;}
    if(strcmp(name,"crossed_quote")==0){p.value.quote.ask=99.0;CHECK(UmiMarketTapeApply(t,&p,&why)==UMI_STATUS_INVALID_ARGUMENT);goto done;}
    if(strcmp(name,"negative_time")==0){p.value.quote.event_time_ms=-1;CHECK(UmiMarketTapeApply(t,&p,&why)==UMI_STATUS_INVALID_ARGUMENT);goto done;}
    if(strcmp(name,"future_time")==0){p.value.quote.event_time_ms=1001;CHECK(UmiMarketTapeApply(t,&p,&why)==UMI_STATUS_INVALID_ARGUMENT);goto done;}
    if(strcmp(name,"identity_mismatch")==0){p.value.quote.instrument.venue[0]='X';CHECK(UmiMarketTapeApply(t,&p,&why)==UMI_STATUS_INVALID_ARGUMENT);goto done;}
    if(strcmp(name,"bad_trade")==0){p=Trade(a,1U,1000,NAN,1.0);CHECK(UmiMarketTapeApply(t,&p,&why)==UMI_STATUS_INVALID_ARGUMENT);p.value.trade.price=100.0;p.value.trade.size=0.0;CHECK(UmiMarketTapeApply(t,&p,&why)==UMI_STATUS_INVALID_ARGUMENT);goto done;}
    if(strncmp(name,"depth_",6U)==0){
        p=Depth(a,1U,1000);
        if(strcmp(name,"depth_sort")==0)p.value.depth.bids[1].price=101.0;
        if(strcmp(name,"depth_duplicate")==0)p.value.depth.asks[1].price=101.0;
        if(strcmp(name,"depth_crossed")==0)p.value.depth.asks[0].price=99.5;
        if(strcmp(name,"depth_count")==0)p.value.depth.bid_count=UMI_TRADING_MAX_DEPTH+1U;
        if(strcmp(name,"depth_nonfinite")==0)p.value.depth.asks[0].size=INFINITY;
        if(strcmp(name,"depth_empty")==0){p.value.depth.bid_count=0U;OK(UmiMarketTapeApply(t,&p,&why));OK(UmiMarketTapeRead(t,1000,v));CHECK(v->rows[0].haveDepth&&!v->rows[0].depthFresh);}
        else if(strcmp(name,"depth_valid")==0){OK(UmiMarketTapeApply(t,&p,&why));OK(UmiMarketTapeRead(t,1000,v));CHECK(v->depth.bid_count==2U&&v->rows[0].depthFresh);}
        else CHECK(UmiMarketTapeApply(t,&p,&why)==UMI_STATUS_INVALID_ARGUMENT);
        goto done;
    }
    if(strcmp(name,"sequence_max")==0){
        OK(UmiMarketTapeBegin(t,&a.instrument_id,2U,UINT64_MAX));p.generation=2U;p.sequence=UINT64_MAX;OK(UmiMarketTapeApply(t,&p,&why));
        OK(UmiMarketTapeRead(t,1000,v));CHECK(v->rows[0].sequenceExhausted&&v->rows[0].state==UMI_MARKET_TAPE_EXHAUSTED);
        CHECK(UmiMarketTapeApply(t,&p,&why)==UMI_STATUS_CAPACITY_EXCEEDED);goto done;
    }
    if(strcmp(name,"gap_first")==0){p.sequence=2U;CHECK(UmiMarketTapeApply(t,&p,&why)==UMI_STATUS_INVALID_STATE);OK(UmiMarketTapeRead(t,1000,v));CHECK(v->rows[0].gapLatched&&!v->rows[0].haveQuote);goto done;}
    if(strcmp(name,"concurrency")==0){
        Worker w={.tape=t,.i=a};atomic_init(&w.finished,false);UmiThread *thread=NULL;OK(umi_thread_start(Produce,&w,&thread));
        size_t reads=0U;
        do{OK(UmiMarketTapeRead(t,100000,v));CHECK(v->rowCount==1U);CHECK(v->rows[0].acceptedEvents==v->rows[0].lastSequence);
            if(v->tradeCount>0U) { CHECK(v->trades[v->tradeCount-1U].event_time_ms==(int64_t)v->rows[0].lastSequence); }
            ++reads;
        }while(!atomic_load_explicit(&w.finished,memory_order_acquire));
        int result=99;OK(umi_thread_join(thread,&result));umi_thread_destroy(thread);CHECK(result==0&&reads>0U);goto done;
    }
    if(strcmp(name,"bars_ohlcv")==0||strcmp(name,"bars_boundary")==0||strcmp(name,"bars_empty_time")==0||strcmp(name,"volume_overflow")==0||strcmp(name,"time_overflow")==0||strcmp(name,"ring_retention")==0||strcmp(name,"snapshot_lifetime")==0||strcmp(name,"bar_reference")==0){
        if(strcmp(name,"time_overflow")==0){p=Trade(a,1U,INT64_MAX,100.0,1.0);CHECK(UmiMarketTapeApply(t,&p,&why)==UMI_STATUS_CAPACITY_EXCEEDED);goto done;}
        if(strcmp(name,"ring_retention")==0){
            for(uint64_t n=1U;n<=150U;++n){p=Trade(a,n,(int64_t)n*1000,100.0+(double)n,1.0);OK(UmiMarketTapeApply(t,&p,&why));}
            OK(UmiMarketTapeRead(t,150000,v));CHECK(v->tradeCount==64U&&v->barCount==64U);CHECK(v->trades[0].price==187.0&&v->bars[0].open==187.0);
            CHECK(v->rows[0].discardedTrades==86U&&v->rows[0].discardedBars==86U);goto done;
        }
        if(strcmp(name,"bar_reference")==0){
            double opens[20]={0},highs[20]={0},lows[20]={0},closes[20]={0},volumes[20]={0};
            for(uint64_t n=0U;n<10000U;++n){int64_t time=1000+(int64_t)n;size_t k=(size_t)(n/1000U);double price=20.0+(double)((n*17U)%47U);double size=1.0+(double)(n%7U);
                p=Trade(a,n+1U,time,price,size);OK(UmiMarketTapeApply(t,&p,&why));if(n%1000U==0U){opens[k]=price;highs[k]=price;lows[k]=price;}
                if(price>highs[k]) { highs[k]=price; }
                if(price<lows[k]) { lows[k]=price; }
                closes[k]=price;volumes[k]+=size;
            }
            OK(UmiMarketTapeRead(t,11000,v));CHECK(v->barCount==10U);
            for(size_t k=0U;k<10U;++k){CHECK(v->bars[k].open==opens[k]&&v->bars[k].high==highs[k]&&v->bars[k].low==lows[k]&&v->bars[k].close==closes[k]&&v->bars[k].volume==volumes[k]);}
            goto done;
        }
        p=Trade(a,1U,1000,100.0,strcmp(name,"volume_overflow")==0?DBL_MAX:2.0);OK(UmiMarketTapeApply(t,&p,&why));
        if(strcmp(name,"volume_overflow")==0){OK(UmiMarketTapeRead(t,1500,saved));p=Trade(a,2U,1200,99.0,DBL_MAX);CHECK(UmiMarketTapeApply(t,&p,&why)==UMI_STATUS_CAPACITY_EXCEEDED);CHECK(Unchanged(t,saved,v,1500));goto done;}
        int64_t second=strcmp(name,"bars_empty_time")==0?5000:(strcmp(name,"bars_boundary")==0?2000:1500);
        p=Trade(a,2U,second,102.0,3.0);OK(UmiMarketTapeApply(t,&p,&why));OK(UmiMarketTapeRead(t,second,v));
        if(strcmp(name,"bars_boundary")==0||strcmp(name,"bars_empty_time")==0)CHECK(v->barCount==2U&&v->bars[1].start_time_ms==second);
        else CHECK(v->barCount==1U&&v->bars[0].open==100.0&&v->bars[0].close==102.0&&v->bars[0].volume==5.0);
        if(strcmp(name,"snapshot_lifetime")==0){UmiMarketTapeDestroy(t);t=NULL;CHECK(v->bars[0].open==100.0&&strcmp(v->rows[0].instrument.instrument_id.value,"A")==0);}
        goto done;
    }
    OK(UmiMarketTapeApply(t,&p,&why));OK(UmiMarketTapeRead(t,1100,saved));
    if(strcmp(name,"quote")==0){CHECK(saved->rows[0].haveQuote&&saved->rows[0].quoteFresh&&saved->rows[0].lastSequence==1U);}
    else if(strcmp(name,"duplicate")==0){CHECK(UmiMarketTapeApply(t,&p,&why)==UMI_STATUS_ALREADY_EXISTS&&why==UMI_MARKET_TAPE_DUPLICATE_OR_OLD);CHECK(Unchanged(t,saved,v,1100));}
    else if(strcmp(name,"old_generation")==0){p.sequence=2U;p.generation=2U;CHECK(UmiMarketTapeApply(t,&p,&why)==UMI_STATUS_INVALID_STATE);CHECK(Unchanged(t,saved,v,1100));}
    else if(strcmp(name,"receive_regression")==0){p=Trade(a,2U,999,100.0,1.0);CHECK(UmiMarketTapeApply(t,&p,&why)==UMI_STATUS_INVALID_ARGUMENT);CHECK(Unchanged(t,saved,v,1100));}
    else if(strcmp(name,"event_regression")==0){p.sequence=2U;p.receivedTimeMs=1100;p.value.quote.event_time_ms=999;CHECK(UmiMarketTapeApply(t,&p,&why)==UMI_STATUS_INVALID_ARGUMENT);CHECK(Unchanged(t,saved,v,1100));}
    else if(strcmp(name,"invalid_no_sequence_consumption")==0){p.sequence=2U;p.value.quote.ask=NAN;CHECK(UmiMarketTapeApply(t,&p,&why)==UMI_STATUS_INVALID_ARGUMENT);p.value.quote.ask=101.0;OK(UmiMarketTapeApply(t,&p,&why));OK(UmiMarketTapeRead(t,1100,v));CHECK(v->rows[0].lastSequence==2U);}
    else if(strcmp(name,"gap_latch")==0){p.sequence=3U;p.value.quote.bid=100.5;CHECK(UmiMarketTapeApply(t,&p,&why)==UMI_STATUS_INVALID_STATE);OK(UmiMarketTapeRead(t,1100,v));CHECK(v->rows[0].gapLatched&&v->rows[0].quote.bid==100.0&&v->rows[0].expectedSequence==2U&&!v->rows[0].quoteFresh);p.sequence=2U;CHECK(UmiMarketTapeApply(t,&p,&why)==UMI_STATUS_INVALID_STATE);}
    else if(strcmp(name,"reset")==0){OK(UmiMarketTapeBegin(t,&a.instrument_id,2U,10U));OK(UmiMarketTapeRead(t,1100,v));CHECK(!v->rows[0].haveQuote&&v->rows[0].expectedSequence==10U&&v->rows[0].generation==2U);CHECK(UmiMarketTapeApply(t,&p,&why)==UMI_STATUS_INVALID_STATE);p.generation=2U;p.sequence=10U;OK(UmiMarketTapeApply(t,&p,&why));}
    else if(strcmp(name,"disconnect")==0){OK(UmiMarketTapeDisconnect(t,&a.instrument_id));p.sequence=2U;CHECK(UmiMarketTapeApply(t,&p,&why)==UMI_STATUS_INVALID_STATE);OK(UmiMarketTapeRead(t,1100,v));CHECK(v->rows[0].haveQuote&&!v->rows[0].quoteFresh&&v->rows[0].state==UMI_MARKET_TAPE_DISCONNECTED);}
    else if(strcmp(name,"stale_threshold")==0){OK(UmiMarketTapeRead(t,2999,v));CHECK(v->rows[0].quoteFresh);OK(UmiMarketTapeRead(t,3000,v));CHECK(!v->rows[0].quoteFresh&&v->rows[0].state==UMI_MARKET_TAPE_STALE);}
    else if(strcmp(name,"lane_freshness")==0){p=Trade(a,2U,4000,101.0,1.0);OK(UmiMarketTapeApply(t,&p,&why));OK(UmiMarketTapeRead(t,4000,v));CHECK(v->rows[0].tradeFresh&&!v->rows[0].quoteFresh&&v->rows[0].state==UMI_MARKET_TAPE_CURRENT);}
    else if(strcmp(name,"read_clock")==0){memset(v,0x55,sizeof(*v));memcpy(saved,v,sizeof(*v));CHECK(UmiMarketTapeRead(t,999,v)==UMI_STATUS_INVALID_ARGUMENT);CHECK(memcmp(v,saved,sizeof(*v))==0);}
    else if(strcmp(name,"selected_context")==0){OK(UmiMarketTapeRegister(t,&b));OK(UmiMarketTapeBegin(t,&b.instrument_id,1U,1U));p=Trade(b,1U,1001,55.0,2.0);OK(UmiMarketTapeApply(t,&p,&why));OK(UmiMarketTapeSelect(t,&b.instrument_id));OK(UmiMarketTapeRead(t,1100,v));CHECK(v->selectedIndex==1U&&v->rows[1].lastTrade.price==55.0&&v->trades[0].price==55.0&&v->rows[0].quote.bid==100.0);}
    else {fprintf(stderr,"Unknown case: %s\n",name);CHECK(false);}
 done:
    UmiMarketTapeDestroy(t);free(v);free(saved);return 0;
}
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    if(strncmp(argv[1],"quality_",8U)==0)return Quality(atoi(argv[1]+8));
    return Scenario(argv[1]);
}
