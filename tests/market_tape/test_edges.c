/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/market_tape/test_edges.c
 * PURPOSE:
 *   Public-contract boundary tests; all checks execute in Release builds too.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Umicom Foundation | Sammy Hegab | MIT
 * Public-contract boundary tests; all checks execute in Release builds too. */
#include "umicom/trading/market_tape_practice.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"line %d: %s\n",__LINE__,#x); return 1; } } while(0)
#define OK(x) CHECK((x)==UMI_STATUS_OK)
static UmiInstrument Instrument(void)
{
    UmiInstrument i={.instrument_id={"A"},.symbol="ALPHA",.venue="PRACTICE",.currency={"GBP"},.multiplier=1.0};
    return i;
}
static UmiMarketTapePacket Packet(uint64_t sequence)
{
    UmiMarketTapePacket p={.kind=UMI_MARKET_TAPE_QUOTE,.generation=1U,.sequence=sequence,.receivedTimeMs=1000};
    p.value.quote=(UmiQuote){Instrument(),100.0,101.0,0.0,0.0,1000};return p;
}
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    const char *name=argv[1];
    UmiMarketTapePractice *practice=NULL;
    UmiMarketTapeSnapshot *before=calloc(1U,sizeof(*before)),*after=calloc(1U,sizeof(*after));
    CHECK(before!=NULL&&after!=NULL);
    if(strncmp(name,"practice_",9U)==0){
        OK(UmiMarketTapePracticeCreate(&practice));
        if(strcmp(name,"practice_controls")==0){
            UmiMarketTapeRejection reason;
            OK(UmiMarketTapePracticeAct(practice,UMI_MARKET_PRACTICE_NEXT,&reason));
            OK(UmiMarketTapePracticeRead(practice,before));CHECK(before->rows[0].acceptedEvents==3U);
            OK(UmiMarketTapePracticeSelect(practice,1U));
            OK(UmiMarketTapePracticeAct(practice,UMI_MARKET_PRACTICE_NEXT,&reason));
            OK(UmiMarketTapePracticeRead(practice,after));CHECK(after->rows[0].acceptedEvents==3U&&after->rows[1].acceptedEvents==3U);
            OK(UmiMarketTapePracticeAct(practice,UMI_MARKET_PRACTICE_AGE,&reason));CHECK(reason==UMI_MARKET_TAPE_ACCEPTED);
            OK(UmiMarketTapePracticeRead(practice,after));CHECK(!after->rows[0].tradeFresh&&!after->rows[1].quoteFresh);
            OK(UmiMarketTapePracticeAct(practice,UMI_MARKET_PRACTICE_DISCONNECT,&reason));CHECK(reason==UMI_MARKET_TAPE_ACCEPTED);
            CHECK(UmiMarketTapePracticeAct(practice,UMI_MARKET_PRACTICE_NEXT,&reason)==UMI_STATUS_INVALID_STATE);
            OK(UmiMarketTapePracticeAct(practice,UMI_MARKET_PRACTICE_NEW_EPOCH,&reason));CHECK(reason==UMI_MARKET_TAPE_ACCEPTED);
            OK(UmiMarketTapePracticeRead(practice,after));CHECK(after->rows[0].acceptedEvents==3U&&after->rows[1].acceptedEvents==0U);
        }else{
            CHECK(strcmp(name,"practice_bad_input")==0);
            CHECK(UmiMarketTapePracticeSelect(practice,2U)==UMI_STATUS_INVALID_ARGUMENT);
            OK(UmiMarketTapePracticeRead(practice,before));
            CHECK(UmiMarketTapePracticeAct(practice,(UmiMarketTapePracticeAction)999,NULL)==UMI_STATUS_INVALID_ARGUMENT);
            OK(UmiMarketTapePracticeRead(practice,after));CHECK(memcmp(before,after,sizeof(*before))==0);
        }
        UmiMarketTapePracticeDestroy(practice);free(before);free(after);return 0;
    }
    UmiMarketTape *tape=NULL;UmiMarketTapeConfig config=UmiMarketTapeConfigDefault();UmiInstrument instrument=Instrument();
    if(strcmp(name,"custom_interval")==0)config.barIntervalMs=7;
    OK(UmiMarketTapeCreate(&config,&tape));OK(UmiMarketTapeRegister(tape,&instrument));OK(UmiMarketTapeBegin(tape,&instrument.instrument_id,1U,1U));
    UmiMarketTapePacket packet=Packet(1U);UmiMarketTapeRejection reason;
    if(strcmp(name,"timestamp_zero")==0){packet.receivedTimeMs=0;packet.value.quote.event_time_ms=0;OK(UmiMarketTapeApply(tape,&packet,NULL));OK(UmiMarketTapeRead(tape,0,before));CHECK(before->rows[0].quoteFresh);}
    else if(strcmp(name,"custom_interval")==0){
        packet.kind=UMI_MARKET_TAPE_TRADE;packet.value.trade=(UmiTradeTick){instrument,1.0,1.0,1000};
        OK(UmiMarketTapeApply(tape,&packet,NULL));OK(UmiMarketTapeRead(tape,1000,before));CHECK(before->bars[0].start_time_ms==994&&before->bars[0].end_time_ms==1001);
    }else if(strcmp(name,"gap_newer_timestamp")==0){
        OK(UmiMarketTapeApply(tape,&packet,NULL));packet.sequence=3U;packet.receivedTimeMs=2000;packet.value.quote.event_time_ms=2000;
        CHECK(UmiMarketTapeApply(tape,&packet,&reason)==UMI_STATUS_INVALID_STATE);OK(UmiMarketTapeRead(tape,2000,before));
        CHECK(before->rows[0].receivedTimeMs==1000&&before->rows[0].gapObservedSequence==3U&&!before->rows[0].quoteFresh);
    }else if(strcmp(name,"snapshot_not_borrowed")==0){
        OK(UmiMarketTapeApply(tape,&packet,NULL));OK(UmiMarketTapeRead(tape,1000,before));before->rows[0].instrument.symbol[0]='Z';before->rows[0].quote.bid=1.0;
        OK(UmiMarketTapeRead(tape,1000,after));CHECK(after->rows[0].quote.bid==100.0&&strcmp(after->rows[0].instrument.symbol,"ALPHA")==0);
    }else if(strcmp(name,"clock_is_projection")==0){
        OK(UmiMarketTapeApply(tape,&packet,NULL));OK(UmiMarketTapeRead(tape,1000,before));OK(UmiMarketTapeRead(tape,5000,after));
        CHECK(before->revision==after->revision&&before->observedAtMs!=after->observedAtMs&&before->rows[0].quoteFresh&&!after->rows[0].quoteFresh);
    }else if(strcmp(name,"depth_lane_independent")==0){
        packet.kind=UMI_MARKET_TAPE_DEPTH;memset(&packet.value,0,sizeof(packet.value));
        packet.value.depth.instrument=instrument;packet.value.depth.event_time_ms=1000;packet.value.depth.bid_count=1U;packet.value.depth.ask_count=1U;
        packet.value.depth.bids[0]=(UmiDepthLevel){100.0,1.0};packet.value.depth.asks[0]=(UmiDepthLevel){101.0,1.0};
        OK(UmiMarketTapeApply(tape,&packet,NULL));packet=Packet(2U);packet.receivedTimeMs=5000;packet.value.quote.event_time_ms=5000;
        OK(UmiMarketTapeApply(tape,&packet,NULL));OK(UmiMarketTapeRead(tape,5000,before));CHECK(before->rows[0].quoteFresh&&!before->rows[0].depthFresh&&before->depth.bids[0].price==100.0);
    }else if(strcmp(name,"delayed_source")==0){
        packet.receivedTimeMs=5000;OK(UmiMarketTapeApply(tape,&packet,NULL));OK(UmiMarketTapeRead(tape,5000,before));
        CHECK(before->rows[0].state==UMI_MARKET_TAPE_CURRENT&&!before->rows[0].quoteFresh);
    }else if(strcmp(name,"idempotent_selection")==0){
        OK(UmiMarketTapeRead(tape,1000,before));OK(UmiMarketTapeSelect(tape,&instrument.instrument_id));OK(UmiMarketTapeRead(tape,1000,after));CHECK(before->revision==after->revision);
        OK(UmiMarketTapeDisconnect(tape,&instrument.instrument_id));OK(UmiMarketTapeRead(tape,1000,before));OK(UmiMarketTapeDisconnect(tape,&instrument.instrument_id));OK(UmiMarketTapeRead(tape,1000,after));CHECK(before->revision==after->revision);
    }else if(strcmp(name,"packet_mutation")==0){
        uint32_t rng=12345U;size_t refused=0U;
        for(uint64_t i=1U;i<=10000U;++i){
            OK(UmiMarketTapeBegin(tape,&instrument.instrument_id,i+1U,1U));packet=Packet(1U);packet.generation=i+1U;
            OK(UmiMarketTapeRead(tape,INT64_MAX,before));
            rng=rng*1664525U+1013904223U;
            unsigned char *bytes=(unsigned char *)&packet;
            size_t position=(size_t)(rng%(uint32_t)sizeof(packet));bytes[position]^=(unsigned char)(1U<<(rng%8U));
            UmiStatus status=UmiMarketTapeApply(tape,&packet,&reason);OK(UmiMarketTapeRead(tape,INT64_MAX,after));
            if(status!=UMI_STATUS_OK&&reason!=UMI_MARKET_TAPE_SEQUENCE_GAP){CHECK(memcmp(before,after,sizeof(*before))==0);++refused;}
            CHECK(after->rowCount==1U&&after->tradeCount<=UMI_MARKET_TAPE_TRADE_LIMIT&&after->barCount<=UMI_MARKET_TAPE_BAR_LIMIT);
        }
        CHECK(refused>100U);
    }else CHECK(false);
    UmiMarketTapeDestroy(tape);free(before);free(after);return 0;
}
