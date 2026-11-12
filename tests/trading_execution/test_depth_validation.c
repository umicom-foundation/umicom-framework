/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_execution/test_depth_validation.c
 *
 * PURPOSE:
 *   Verify canonical depth identity, active numeric levels and complete workspace refusal on invalid evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../trading/test_trading_common.h"
/* The real owner permits whole-state refusal checks without exposing private
 * mutable storage in the installed public API. */
#include "../../src/trading/workspace.c"
#define DEPTH_CHECK(x) do { if (!(x)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#x); result=1; goto done; } } while(0)
int main(int argc,char **argv)
{
    const char *known[]={"valid-basic", "valid-empty", "valid-bid-only", "valid-ask-only", "valid-zero", "valid-negative", "valid-unordered", "valid-crossed", "valid-negative-time", "valid-inactive-tail", "valid-full", "bad-id-text", "bad-symbol-text", "bad-venue-text", "bad-currency-text", "bad-empty-id", "bad-empty-symbol", "bad-empty-venue", "bad-multiplier-zero", "bad-multiplier-nan", "bad-bid-count", "bad-ask-count", "bad-bid-price-nan", "bad-ask-price-nan", "bad-bid-price-inf", "bad-ask-price-inf", "bad-bid-size-nan", "bad-ask-size-nan", "bad-bid-size-inf", "bad-ask-size-inf", "bad-bid-size-negative", "bad-ask-size-negative", "meta-symbol", "meta-venue", "meta-currency", "meta-multiplier", "meta-expiry", "missing", "null"};
    if(argc!=2)return 2;
    const char *name=argv[1];int recognized=0,result=0;
    for(size_t i=0;i<sizeof(known)/sizeof(known[0]);++i)if(!strcmp(name,known[i]))recognized=1;
    if(!recognized)return 2;
    UmiTradingWorkspace *workspace=NULL,*before=NULL;
    UmiInstrument instrument=test_instrument();
    UmiMarketDepth value={0};value.instrument=instrument;value.bid_count=1;value.ask_count=1;
    value.bids[0]=(UmiDepthLevel){100,2};value.asks[0]=(UmiDepthLevel){101,3};value.event_time_ms=1000;
    DEPTH_CHECK(umi_trading_workspace_create(NULL,&workspace)==UMI_STATUS_OK);
    DEPTH_CHECK(umi_trading_workspace_add_instrument(workspace,&instrument)==UMI_STATUS_OK);
    DEPTH_CHECK(umi_trading_workspace_update_depth(workspace,&value)==UMI_STATUS_OK);
    before=malloc(sizeof(*before));DEPTH_CHECK(before!=NULL);
    memcpy(before,workspace,sizeof(*before));
    if(!strcmp(name,"valid-empty")){value.bid_count=0; value.ask_count=0;}
    if(!strcmp(name,"valid-bid-only")){value.ask_count=0;}
    if(!strcmp(name,"valid-ask-only")){value.bid_count=0;}
    if(!strcmp(name,"valid-zero")){value.bids[0]=(UmiDepthLevel){0,0}; value.asks[0]=(UmiDepthLevel){0,0};}
    if(!strcmp(name,"valid-negative")){value.bids[0].price=-3; value.asks[0].price=-2;}
    if(!strcmp(name,"valid-unordered")){value.bid_count=2; value.bids[1]=(UmiDepthLevel){110,2};}
    if(!strcmp(name,"valid-crossed")){value.bids[0].price=110; value.asks[0].price=100;}
    if(!strcmp(name,"valid-negative-time")){value.event_time_ms=-1;}
    if(!strcmp(name,"valid-inactive-tail")){value.bids[UMI_TRADING_MAX_DEPTH-1U]=(UmiDepthLevel){NAN,-1}; value.asks[UMI_TRADING_MAX_DEPTH-1U]=(UmiDepthLevel){INFINITY,NAN};}
    if(!strcmp(name,"valid-full")){value.bid_count=UMI_TRADING_MAX_DEPTH; value.ask_count=UMI_TRADING_MAX_DEPTH; for(size_t i=0;i<UMI_TRADING_MAX_DEPTH;++i){value.bids[i]=(UmiDepthLevel){100,1};value.asks[i]=(UmiDepthLevel){101,1};}}
    if(!strcmp(name,"bad-id-text")){memset(value.instrument.instrument_id.value,'x',sizeof(value.instrument.instrument_id.value));}
    if(!strcmp(name,"bad-symbol-text")){memset(value.instrument.symbol,'x',sizeof(value.instrument.symbol));}
    if(!strcmp(name,"bad-venue-text")){memset(value.instrument.venue,'x',sizeof(value.instrument.venue));}
    if(!strcmp(name,"bad-currency-text")){memset(value.instrument.currency.code,'x',sizeof(value.instrument.currency.code));}
    if(!strcmp(name,"bad-empty-id")){value.instrument.instrument_id.value[0]='\0';}
    if(!strcmp(name,"bad-empty-symbol")){value.instrument.symbol[0]='\0';}
    if(!strcmp(name,"bad-empty-venue")){value.instrument.venue[0]='\0';}
    if(!strcmp(name,"bad-multiplier-zero")){value.instrument.multiplier=0;}
    if(!strcmp(name,"bad-multiplier-nan")){value.instrument.multiplier=NAN;}
    if(!strcmp(name,"bad-bid-count")){value.bid_count=UMI_TRADING_MAX_DEPTH+1U;}
    if(!strcmp(name,"bad-ask-count")){value.ask_count=UMI_TRADING_MAX_DEPTH+1U;}
    if(!strcmp(name,"bad-bid-price-nan")){value.bids[0].price=NAN;}
    if(!strcmp(name,"bad-ask-price-nan")){value.asks[0].price=NAN;}
    if(!strcmp(name,"bad-bid-price-inf")){value.bids[0].price=INFINITY;}
    if(!strcmp(name,"bad-ask-price-inf")){value.asks[0].price=-INFINITY;}
    if(!strcmp(name,"bad-bid-size-nan")){value.bids[0].size=NAN;}
    if(!strcmp(name,"bad-ask-size-nan")){value.asks[0].size=NAN;}
    if(!strcmp(name,"bad-bid-size-inf")){value.bids[0].size=INFINITY;}
    if(!strcmp(name,"bad-ask-size-inf")){value.asks[0].size=INFINITY;}
    if(!strcmp(name,"bad-bid-size-negative")){value.bids[0].size=-1;}
    if(!strcmp(name,"bad-ask-size-negative")){value.asks[0].size=-1;}
    if(!strcmp(name,"meta-symbol")){strcpy(value.instrument.symbol,"other");}
    if(!strcmp(name,"meta-venue")){strcpy(value.instrument.venue,"other");}
    if(!strcmp(name,"meta-currency")){strcpy(value.instrument.currency.code,"EUR");}
    if(!strcmp(name,"meta-multiplier")){value.instrument.multiplier+=1;}
    if(!strcmp(name,"meta-expiry")){value.instrument.expiry_yyyymmdd+=1;}
    if(!strcmp(name,"missing")){strcpy(value.instrument.instrument_id.value,"absent-instrument");}
    const UmiMarketDepth *input=!strcmp(name,"null")?NULL:&value;
    int valid=strncmp(name,"bad-",4U)!=0 && strcmp(name,"null")!=0;
    DEPTH_CHECK(umi_market_depth_valid(input)==valid);
    double expectedBid=valid && value.bid_count?value.bids[0].price:0.0;
    double expectedAsk=valid && value.ask_count?value.asks[0].price:0.0;
    DEPTH_CHECK(umi_market_depth_best_bid(input)==expectedBid);
    DEPTH_CHECK(umi_market_depth_best_ask(input)==expectedAsk);
    UmiStatus expected=!valid || !strncmp(name,"meta-",5U)?UMI_STATUS_INVALID_ARGUMENT:
        !strcmp(name,"missing")?UMI_STATUS_NOT_FOUND:UMI_STATUS_OK;
    DEPTH_CHECK(umi_trading_workspace_update_depth(workspace,input)==expected);
    if(expected!=UMI_STATUS_OK) {
        DEPTH_CHECK(!memcmp(before,workspace,sizeof(*workspace)));
    } else {
        const UmiMarketDepth *stored=&workspace->markets[0].depth;
        DEPTH_CHECK(workspace->revision==before->revision+1U);
        DEPTH_CHECK(workspace->markets[0].revision==before->markets[0].revision+1U);
        DEPTH_CHECK(stored->bid_count==value.bid_count && stored->ask_count==value.ask_count);
        DEPTH_CHECK(stored->event_time_ms==value.event_time_ms);
        for(size_t i=0;i<stored->bid_count;++i)DEPTH_CHECK(stored->bids[i].price==value.bids[i].price && stored->bids[i].size==value.bids[i].size);
        for(size_t i=0;i<stored->ask_count;++i)DEPTH_CHECK(stored->asks[i].price==value.asks[i].price && stored->asks[i].size==value.asks[i].size);
    }
done:
    free(before);if(workspace!=NULL)umi_trading_workspace_destroy(workspace);
    return result;
}
