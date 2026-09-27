/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * File: src/research_replay/replay.c
 * Purpose: One replay owner, immutable copied input, explicitly delayed fills.
 * Reuses the canonical quote and completed-trade aggregation contracts.
 *---------------------------------------------------------------------------*/
#include "umicom/strategy_research/research_replay.h"
#include "umicom/trading/quote.h"
#include "umicom/native_launcher/sha256.h"
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

_Static_assert(sizeof(double)==8 && DBL_MANT_DIG==53 && DBL_MAX_EXP==1024,
    "Research fingerprints require binary64 double");
typedef struct ReplayModel {
    UmiStrategyBacktestState closed;
    size_t cursor;
    int direction;
    double entry;
    double entryFee;
    uint64_t entryTime;
    int pending;
    int target;
    size_t submittedIndex;
    uint64_t readyTime;
    double fees;
    double slippage;
    double equity;
    double unrealised;
    double peak;
    double drawdown;
    double drawdownPercent;
    uint64_t fills;
    uint64_t thin;
} ReplayModel;
struct UmiResearchReplay {
    UmiResearchConfig config;
    UmiResearchObservation *input;
    UmiResearchTrace *trace;
    size_t count;
    UmiResearchStrategy strategy;
    void *userData;
    ReplayModel model;
    UmiResearchState state;
    UmiStatus failure;
    int inStep;
    char fingerprint[65];
};
static int Token(const char *s,size_t capacity)
{
    const char *end=memchr(s,0,capacity);
    if(end==NULL || end==s) return 0;
    for(const char *p=s;p<end;++p) {
        unsigned char c=(unsigned char)*p;
        if(!((c>='a'&&c<='z')||(c>='A'&&c<='Z')||(c>='0'&&c<='9')||
            c=='_'||c=='-'||c=='.'||c==' ')) return 0;
    }
    return 1;
}
static int InstrumentValid(const UmiInstrument *i)
{
    return Token(i->instrument_id.value,sizeof i->instrument_id.value) &&
        Token(i->symbol,sizeof i->symbol) && Token(i->venue,sizeof i->venue) &&
        i->currency.code[3]==0 && i->currency.code[0]>='A'&&i->currency.code[0]<='Z' &&
        i->currency.code[1]>='A'&&i->currency.code[1]<='Z' &&
        i->currency.code[2]>='A'&&i->currency.code[2]<='Z' && i->multiplier==1.0;
}
static int SameInstrument(const UmiInstrument *a,const UmiInstrument *b)
{
    return strcmp(a->instrument_id.value,b->instrument_id.value)==0 &&
        strcmp(a->symbol,b->symbol)==0 && strcmp(a->venue,b->venue)==0 &&
        strcmp(a->currency.code,b->currency.code)==0 && a->multiplier==b->multiplier &&
        a->expiry_yyyymmdd==b->expiry_yyyymmdd;
}
static int ConfigValid(const UmiResearchConfig *c)
{
    return c!=NULL && isfinite(c->initialEquity)&&c->initialEquity>0.0 &&
        isfinite(c->units)&&c->units>0.0&&c->units<=1e9 &&
        isfinite(c->commissionPerUnit)&&c->commissionPerUnit>=0.0 &&
        isfinite(c->slippageBps)&&c->slippageBps>=0.0&&c->slippageBps<=1000.0 &&
        Token(c->strategyTag,sizeof c->strategyTag);
}
static void HashInteger(UmiNativeSha256 *h,uint64_t v)
{
    unsigned char bytes[8];
    for(size_t i=0;i<8;++i) bytes[i]=(unsigned char)(v>>(i*8U));
    (void)UmiNativeSha256Update(h,bytes,sizeof bytes);
}
static void HashDouble(UmiNativeSha256 *h,double v)
{
    uint64_t bits=0;
    if(v==0.0) v=0.0; /* One representation for either signed zero. */
    memcpy(&bits,&v,sizeof bits); HashInteger(h,bits);
}
static void HashText(UmiNativeSha256 *h,const char *s)
{
    HashInteger(h,(uint64_t)strlen(s)); (void)UmiNativeSha256Update(h,s,strlen(s));
}
static void Fingerprint(UmiResearchReplay *r)
{
    UmiNativeSha256 h; unsigned char digest[32]; UmiNativeSha256Init(&h);
    HashText(&h,"Umicom research replay: next observed quote, full fill, revision 1");
    HashText(&h,r->config.strategyTag);
    HashDouble(&h,r->config.initialEquity); HashDouble(&h,r->config.units);
    HashDouble(&h,r->config.commissionPerUnit); HashDouble(&h,r->config.slippageBps);
    HashInteger(&h,r->config.latencyMilliseconds); HashInteger(&h,(uint64_t)r->count);
    const UmiInstrument *i=&r->input[0].quote.instrument;
    HashText(&h,i->instrument_id.value);HashText(&h,i->symbol);HashText(&h,i->venue);
    HashText(&h,i->currency.code);HashDouble(&h,i->multiplier);
    HashInteger(&h,(uint64_t)(int64_t)i->expiry_yyyymmdd);
    for(size_t n=0;n<r->count;++n) {
        const UmiResearchObservation *e=&r->input[n];
        HashInteger(&h,e->sequence);HashInteger(&h,(uint64_t)e->quote.event_time_ms);
        HashDouble(&h,e->quote.bid);HashDouble(&h,e->quote.ask);
        HashDouble(&h,e->quote.bid_size);HashDouble(&h,e->quote.ask_size);
    }
    (void)UmiNativeSha256Final(&h,digest);UmiNativeSha256Hex(digest,r->fingerprint);
}
UmiResearchConfig UmiResearchConfigDefault(void)
{
    UmiResearchConfig c={0}; c.initialEquity=10000.0;c.units=1.0;
    c.commissionPerUnit=0.10;c.slippageBps=0.0;
    strcpy(c.strategyTag,"reviewed-native-strategy");return c;
}
UmiStatus UmiResearchReplayCreate(const UmiResearchConfig *c,
    const UmiResearchObservation *events,size_t count,UmiResearchStrategy strategy,
    void *userData,UmiResearchReplay **out)
{
    if(out==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out=NULL;
    if(!ConfigValid(c)||events==NULL||strategy==NULL||count==0)
        return UMI_STATUS_INVALID_ARGUMENT;
    if(count>UMI_RESEARCH_OBSERVATION_LIMIT) return UMI_STATUS_CAPACITY_EXCEEDED;
    for(size_t n=0;n<count;++n) {
        const UmiResearchObservation *e=&events[n];
        if(!InstrumentValid(&e->quote.instrument)||!umi_quote_valid(&e->quote)||
            e->sequence==0||e->quote.event_time_ms<0) return UMI_STATUS_INVALID_ARGUMENT;
        if(n && (!SameInstrument(&events[0].quote.instrument,&e->quote.instrument)||
            events[n-1].sequence==UINT64_MAX||e->sequence!=events[n-1].sequence+1U||
            e->quote.event_time_ms<events[n-1].quote.event_time_ms))
            return UMI_STATUS_INVALID_ARGUMENT;
    }
    UmiResearchReplay *r=calloc(1,sizeof *r);
    if(r==NULL) return UMI_STATUS_OUT_OF_MEMORY;
    r->input=calloc(count,sizeof *r->input);r->trace=calloc(count,sizeof *r->trace);
    if(r->input==NULL||r->trace==NULL) {UmiResearchReplayDestroy(r);return UMI_STATUS_OUT_OF_MEMORY;}
    memcpy(r->input,events,count*sizeof *events);r->count=count;r->config=*c;
    r->strategy=strategy;r->userData=userData;r->state=UMI_RESEARCH_READY;
    (void)umi_strategy_backtest_state_init(&r->model.closed,c->initialEquity);
    r->model.equity=c->initialEquity;r->model.peak=c->initialEquity;
    Fingerprint(r);*out=r;return UMI_STATUS_OK;
}
void UmiResearchReplayDestroy(UmiResearchReplay *r)
{
    if(r==NULL) return;
    free(r->input);free(r->trace);free(r);
}
static UmiStatus Mark(const UmiResearchReplay *r,ReplayModel *m,const UmiQuote *q)
{
    m->unrealised=m->direction ?
        ((m->direction>0?q->bid:q->ask)-m->entry)*(double)m->direction*r->config.units : 0.0;
    m->equity=m->closed.equity+m->unrealised-(m->direction?m->entryFee:0.0);
    if(!isfinite(m->unrealised)||!isfinite(m->equity)) return UMI_STATUS_CAPACITY_EXCEEDED;
    if(m->equity>m->peak) m->peak=m->equity;
    double dd=m->peak-m->equity;
    double percent=(dd/m->peak)*100.0;
    if(!isfinite(dd)||!isfinite(percent)) return UMI_STATUS_CAPACITY_EXCEEDED;
    if(dd>m->drawdown) m->drawdown=dd;
    if(percent>m->drawdownPercent) m->drawdownPercent=percent;
    return UMI_STATUS_OK;
}
static UmiStatus Fill(const UmiResearchReplay *r,ReplayModel *m,
    const UmiResearchObservation *e,UmiResearchTrace *t)
{
    if(!m->pending||m->cursor<=m->submittedIndex||(uint64_t)e->quote.event_time_ms<m->readyTime)
        return UMI_STATUS_OK;
    int change=m->target-m->direction;
    double quantity=fabs((double)change)*r->config.units;
    double available=change>0?e->quote.ask_size:e->quote.bid_size;
    if(available<quantity) {m->thin++;return UMI_STATUS_OK;}
    double raw=change>0?e->quote.ask:e->quote.bid;
    double price=raw*(1.0+(change>0?1.0:-1.0)*r->config.slippageBps/10000.0);
    double fee=quantity*r->config.commissionPerUnit;
    double sideFee=r->config.units*r->config.commissionPerUnit;
    double impact=fabs(price-raw)*quantity;
    if(!isfinite(price)||price<=0.0||!isfinite(fee)||!isfinite(impact)||
        !isfinite(m->fees+fee)||!isfinite(m->slippage+impact)) return UMI_STATUS_CAPACITY_EXCEEDED;
    if(m->direction) {
        UmiStatus s=umi_strategy_backtest_add_trade(&m->closed,m->entry,price,
            (double)m->direction*r->config.units,m->entryFee+sideFee,0.0,
            m->entryTime,(uint64_t)e->quote.event_time_ms);
        if(s!=UMI_STATUS_OK) return s;
    }
    m->direction=m->target;m->pending=0;m->target=0;
    m->entry=m->direction?price:0.0;m->entryFee=m->direction?sideFee:0.0;
    m->entryTime=m->direction?(uint64_t)e->quote.event_time_ms:0;
    m->fees+=fee;m->slippage+=impact;m->fills++;
    t->signedFillUnits=(double)change*r->config.units;t->fillPrice=price;t->stepCommission=fee;
    return UMI_STATUS_OK;
}
UmiStatus UmiResearchReplayStep(UmiResearchReplay *r)
{
    if(r==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if(r->inStep||r->state>=UMI_RESEARCH_COMPLETED) return UMI_STATUS_INVALID_STATE;
    r->inStep=1;
    ReplayModel next=r->model; UmiResearchTrace trace={0};
    const UmiResearchObservation *e=&r->input[next.cursor];
    trace.sequence=e->sequence;trace.timeMilliseconds=e->quote.event_time_ms;
    UmiStatus s=Fill(r,&next,e,&trace);
    if(s==UMI_STATUS_OK) s=Mark(r,&next,&e->quote);
    UmiResearchDirection decision=UMI_RESEARCH_HOLD;
    if(s==UMI_STATUS_OK) {
        UmiResearchView view={0};view.observationIndex=next.cursor;view.current=*e;
        view.position=next.direction;view.equity=next.equity;
        view.hasPending=next.pending;view.pendingTarget=next.target;
        s=r->strategy(&view,r->userData,&decision);
        if(s==UMI_STATUS_OK && decision!=UMI_RESEARCH_HOLD && decision!=UMI_RESEARCH_LONG &&
            decision!=UMI_RESEARCH_SHORT && decision!=UMI_RESEARCH_FLAT)
            s=UMI_STATUS_INVALID_ARGUMENT;
    }
    if(s==UMI_STATUS_OK && decision!=UMI_RESEARCH_HOLD) {
        next.pending=(int)decision!=next.direction;
        next.target=next.pending?(int)decision:0;
        next.submittedIndex=next.cursor;
        next.readyTime=(uint64_t)e->quote.event_time_ms+r->config.latencyMilliseconds;
    }
    r->inStep=0;
    if(s!=UMI_STATUS_OK) {r->state=UMI_RESEARCH_FAILED;r->failure=s;return s;}
    trace.position=next.direction;trace.hasPending=next.pending;
    trace.pendingTarget=next.target;trace.decision=(int)decision;trace.equity=next.equity;
    r->trace[next.cursor]=trace;next.cursor++;r->model=next;
    r->state=next.cursor==r->count?UMI_RESEARCH_COMPLETED:UMI_RESEARCH_RUNNING;
    return UMI_STATUS_OK;
}
UmiStatus UmiResearchReplayRun(UmiResearchReplay *r,size_t budget,size_t *outProcessed)
{
    if(outProcessed==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outProcessed=0;
    if(r==NULL||budget==0||budget>UMI_RESEARCH_OBSERVATION_LIMIT) return UMI_STATUS_INVALID_ARGUMENT;
    if(r->inStep||r->state>=UMI_RESEARCH_COMPLETED) return UMI_STATUS_INVALID_STATE;
    while(*outProcessed<budget&&r->state<UMI_RESEARCH_COMPLETED) {
        UmiStatus s=UmiResearchReplayStep(r);
        if(s!=UMI_STATUS_OK) return s;
        (*outProcessed)++;
    }
    return UMI_STATUS_OK;
}
UmiStatus UmiResearchReplayCancel(UmiResearchReplay *r)
{
    if(r==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if(r->inStep||r->state>=UMI_RESEARCH_COMPLETED) return UMI_STATUS_INVALID_STATE;
    r->state=UMI_RESEARCH_CANCELLED;return UMI_STATUS_OK;
}
UmiStatus UmiResearchReplaySnapshot(const UmiResearchReplay *r,UmiResearchSnapshot *out)
{
    if(r==NULL||out==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiResearchSnapshot v={0};v.state=r->state;v.failure=r->failure;
    v.processed=r->model.cursor;v.total=r->count;v.position=r->model.direction;
    v.hasPending=r->model.pending;v.pendingTarget=r->model.target;
    v.entryPrice=r->model.entry;v.equity=r->model.equity;v.unrealisedPnl=r->model.unrealised;
    v.totalCommission=r->model.fees;v.executionSlippage=r->model.slippage;
    v.markToMarketDrawdown=r->model.drawdown;v.markToMarketDrawdownPercent=r->model.drawdownPercent;
    v.fills=r->model.fills;v.insufficientLiquidity=r->model.thin;
    v.profitFactorAvailable=r->model.closed.grossLoss<0.0;
    memcpy(v.inputFingerprint,r->fingerprint,sizeof v.inputFingerprint);
    UmiStatus s=umi_strategy_backtest_snapshot(&r->model.closed,&v.closedTrades);
    if(s==UMI_STATUS_OK) *out=v;
    return s;
}
UmiStatus UmiResearchReplayTraceAt(const UmiResearchReplay *r,size_t index,UmiResearchTrace *out)
{
    if(r==NULL||out==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if(index>=r->model.cursor) return UMI_STATUS_NOT_FOUND;
    *out=r->trace[index];return UMI_STATUS_OK;
}
