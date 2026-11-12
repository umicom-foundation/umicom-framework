/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_execution/test_market_event_integrity.c
 *
 * PURPOSE:
 *   Verify market contract matching, finite public ticks and known state transitions without partial owner mutation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/



#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../trading/test_trading_common.h"
/* Include the real owners to compare retained storage after refusal. No private
 * mutation entry point is added to the installed SDK. */
#include "../../src/trading/trade_tape.c"
#include "../../src/trading/workspace.c"

#define EVENT_CHECK(x) do { if (!(x)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#x); result=1; goto done; } } while (0)

int main(int argc, char **argv)
{
    static const char *cases[] = {"bar:valid", "trade:valid", "bar:symbol", "trade:symbol", "bar:venue", "trade:venue", "bar:currency", "trade:currency", "bar:multiplier", "trade:multiplier", "bar:expiry", "trade:expiry", "bar:bad-id", "trade:bad-id", "bar:bad-symbol", "trade:bad-symbol", "bar:bad-venue", "trade:bad-venue", "bar:bad-currency", "trade:bad-currency", "bar:bad-multiplier", "trade:bad-multiplier", "bar:missing", "trade:missing", "bar:null", "trade:null", "bar:metadata-at-capacity", "trade:metadata-at-capacity", "tick:ordinary", "tick:zero-size", "tick:zero-time", "tick:maximum-finite", "tick:price-nan", "tick:price-infinity", "tick:price-zero", "tick:price-negative", "tick:size-nan", "tick:size-infinity", "tick:size-negative", "tick:negative-time", "tick:null", "state:negative:negative", "state:negative:closed", "state:negative:preopen", "state:negative:open", "state:negative:halted", "state:negative:above", "state:negative:maximum", "state:closed:negative", "state:closed:closed", "state:closed:preopen", "state:closed:open", "state:closed:halted", "state:closed:above", "state:closed:maximum", "state:preopen:negative", "state:preopen:closed", "state:preopen:preopen", "state:preopen:open", "state:preopen:halted", "state:preopen:above", "state:preopen:maximum", "state:open:negative", "state:open:closed", "state:open:preopen", "state:open:open", "state:open:halted", "state:open:above", "state:open:maximum", "state:halted:negative", "state:halted:closed", "state:halted:preopen", "state:halted:open", "state:halted:halted", "state:halted:above", "state:halted:maximum", "state:above:negative", "state:above:closed", "state:above:preopen", "state:above:open", "state:above:halted", "state:above:above", "state:above:maximum", "state:maximum:negative", "state:maximum:closed", "state:maximum:preopen", "state:maximum:open", "state:maximum:halted", "state:maximum:above", "state:maximum:maximum", "bar:replace", "bar:older", "bar:negative-price", "state:corrupt-owner"};
    static const char *state_names[] = {"negative","closed","preopen","open","halted","above","maximum"};
    static const int state_values[] = {-1,0,1,2,3,4,INT_MAX};
    /* The contract table is stated independently of the implementation's switch:
     * closed -> preopen; preopen -> open/closed; open -> halted/closed;
     * halted -> open/closed. Declared-state repeats remain allowed. */
    static const int transitions[4][4] = {
        {1,1,0,0}, {1,1,1,0}, {1,0,1,1}, {1,0,1,1}
    };
    int result=0, recognized=0;
    if (argc!=2) return 2;
    const char *name=argv[1];
    for(size_t i=0;i<sizeof(cases)/sizeof(cases[0]);++i)
        if(!strcmp(name,cases[i])) recognized=1;
    if(!recognized) return 2;
    UmiTradingWorkspace *workspace=NULL,*before=NULL;
    UmiTradingTradeTape *tape_before=NULL,*direct_tape=NULL;
    UmiInstrument instrument=test_instrument();
    UmiTradeTick tick={0}; tick.instrument=instrument;tick.price=100;tick.size=2;tick.event_time_ms=1000;
    UmiTradingTradeTapeRecord record={0};
    UmiBar bar={0};bar.instrument=instrument;bar.open=100;bar.high=102;bar.low=99;bar.close=101;
    bar.volume=10;bar.start_time_ms=1000;bar.end_time_ms=2000;

    if(!strncmp(name,"state:",6U) && strcmp(name,"state:corrupt-owner")){
        for(size_t from=0;from<7U;++from)for(size_t to=0;to<7U;++to){
            char candidate[80];
            (void)snprintf(candidate,sizeof(candidate),"state:%s:%s",state_names[from],state_names[to]);
            if(strcmp(candidate,name)) continue;
            int expected=from>=1U && from<=4U && to>=1U && to<=4U
                ?transitions[from-1U][to-1U]:0;
            EVENT_CHECK(umi_market_state_transition_allowed(
                (UmiMarketState)state_values[from],(UmiMarketState)state_values[to])==expected);
        }
        goto done;
    }

    if(!strncmp(name,"tick:",5U)){
        const char *which=name+5;
        if(!strcmp(which,"zero-size"))tick.size=0;
        if(!strcmp(which,"zero-time"))tick.event_time_ms=0;
        if(!strcmp(which,"maximum-finite")){tick.price=DBL_MAX;tick.size=DBL_MAX;}
        if(!strcmp(which,"price-nan"))tick.price=NAN;
        if(!strcmp(which,"price-infinity"))tick.price=INFINITY;
        if(!strcmp(which,"price-zero"))tick.price=0;
        if(!strcmp(which,"price-negative"))tick.price=-1;
        if(!strcmp(which,"size-nan"))tick.size=NAN;
        if(!strcmp(which,"size-infinity"))tick.size=INFINITY;
        if(!strcmp(which,"size-negative"))tick.size=-1;
        if(!strcmp(which,"negative-time"))tick.event_time_ms=-1;
        int valid=!strcmp(which,"ordinary") || !strcmp(which,"zero-size") ||
            !strcmp(which,"zero-time") || !strcmp(which,"maximum-finite");
        const UmiTradeTick *input=!strcmp(which,"null")?NULL:&tick;
        EVENT_CHECK(umi_trade_tick_valid(input)==valid);
        UmiTradingTradeTapeRecord sentinel;
        memset(&record,0xa5,sizeof(record));memcpy(&sentinel,&record,sizeof(record));
        EVENT_CHECK(umi_trading_trade_tape_record_init(&record,1,input,
            UMI_TRADING_TRADE_DIRECTION_UNKNOWN,"")== (valid?UMI_STATUS_OK:UMI_STATUS_INVALID_ARGUMENT));
        if(!valid)EVENT_CHECK(!memcmp(&record,&sentinel,sizeof(record)));
        /* A provider may fill a public record directly. Append must enforce the
         * same finite rule instead of relying on callers to use record_init. */
        EVENT_CHECK(umi_trading_trade_tape_create(&direct_tape)==UMI_STATUS_OK);
        tape_before=malloc(sizeof(*tape_before));EVENT_CHECK(tape_before!=NULL);
        memcpy(tape_before,direct_tape,sizeof(*tape_before));
        memset(&record,0,sizeof(record));
        record.structure_size=(uint32_t)sizeof(record);
        record.api_version=UMI_TRADING_TRADE_TAPE_API_VERSION;
        record.sequence=1;record.trade=tick;record.direction=UMI_TRADING_TRADE_DIRECTION_UNKNOWN;
        if(strcmp(which,"null")){
            EVENT_CHECK(umi_trading_trade_tape_append(direct_tape,&record)==
                (valid?UMI_STATUS_OK:UMI_STATUS_INVALID_ARGUMENT));
            if(!valid)EVENT_CHECK(!memcmp(tape_before,direct_tape,sizeof(*direct_tape)));
            else EVENT_CHECK(direct_tape->count==1 && direct_tape->records[0].trade.price==tick.price);
        }
        goto done;
    }

    EVENT_CHECK(umi_trading_workspace_create(NULL,&workspace)==UMI_STATUS_OK);
    EVENT_CHECK(umi_trading_workspace_add_instrument(workspace,&instrument)==UMI_STATUS_OK);
    EVENT_CHECK(umi_trading_workspace_update_bar(workspace,&bar,99)==UMI_STATUS_OK);
    EVENT_CHECK(umi_trading_trade_tape_record_init(&record,1,&tick,
        UMI_TRADING_TRADE_DIRECTION_UNKNOWN,"")==UMI_STATUS_OK);
    EVENT_CHECK(umi_trading_workspace_update_trade(workspace,&record)==UMI_STATUS_OK);
    int is_bar=!strncmp(name,"bar:",4U);
    const char *which=strchr(name,':')+1;
    if(!strcmp(name,"state:corrupt-owner"))workspace->markets[0].market_state=(UmiMarketState)99;
    if(!strcmp(which,"metadata-at-capacity"))workspace->revision=UINT64_MAX;
    before=malloc(sizeof(*before));tape_before=malloc(sizeof(*tape_before));
    EVENT_CHECK(before!=NULL && tape_before!=NULL);
    memcpy(before,workspace,sizeof(*before));
    memcpy(tape_before,workspace->trade_tape,sizeof(*tape_before));
    if(!strcmp(name,"state:corrupt-owner")){
        EVENT_CHECK(umi_trading_workspace_set_market_state(workspace,instrument.instrument_id.value,
            (UmiMarketState)99)==UMI_STATUS_INVALID_STATE);
        EVENT_CHECK(!memcmp(before,workspace,sizeof(*workspace)));
        goto done;
    }
    UmiInstrument changed=instrument;
    if(!strcmp(which,"symbol"))strcpy(changed.symbol,"OTHER");
    if(!strcmp(which,"venue"))strcpy(changed.venue,"OTHER");
    if(!strcmp(which,"currency"))strcpy(changed.currency.code,"EUR");
    if(!strcmp(which,"multiplier") || !strcmp(which,"metadata-at-capacity"))changed.multiplier+=1;
    if(!strcmp(which,"expiry"))changed.expiry_yyyymmdd+=1;
    if(!strcmp(which,"bad-id"))memset(changed.instrument_id.value,'x',sizeof(changed.instrument_id.value));
    if(!strcmp(which,"bad-symbol"))memset(changed.symbol,'x',sizeof(changed.symbol));
    if(!strcmp(which,"bad-venue"))memset(changed.venue,'x',sizeof(changed.venue));
    if(!strcmp(which,"bad-currency"))memset(changed.currency.code,'x',sizeof(changed.currency.code));
    if(!strcmp(which,"bad-multiplier"))changed.multiplier=INFINITY;
    if(!strcmp(which,"missing"))strcpy(changed.instrument_id.value,"ABSENT");
    bar.instrument=changed;record.trade.instrument=changed;record.sequence=2;
    bar.start_time_ms=3000;bar.end_time_ms=4000;bar.close=102;record.trade.price=101;
    if(!strcmp(which,"replace")){bar.start_time_ms=1000;bar.end_time_ms=2000;}
    if(!strcmp(which,"older")){bar.start_time_ms=0;bar.end_time_ms=1000;}
    if(!strcmp(which,"negative-price")){bar.open=-2;bar.high=-1;bar.low=-3;bar.close=-2;}
    int success=!strcmp(which,"valid") || !strcmp(which,"replace") || !strcmp(which,"negative-price");
    UmiStatus expected=success?UMI_STATUS_OK:!strcmp(which,"missing")?UMI_STATUS_NOT_FOUND:
        !strcmp(which,"older")?UMI_STATUS_INVALID_STATE:UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus observed=is_bar?umi_trading_workspace_update_bar(workspace,
        !strcmp(which,"null")?NULL:&bar,99):umi_trading_workspace_update_trade(workspace,
        !strcmp(which,"null")?NULL:&record);
    EVENT_CHECK(observed==expected);
    if(!success){
        EVENT_CHECK(!memcmp(before,workspace,sizeof(*workspace)));
        EVENT_CHECK(!memcmp(tape_before,workspace->trade_tape,sizeof(*tape_before)));
    } else {
        EVENT_CHECK(workspace->revision==before->revision+1U);
        EVENT_CHECK(workspace->markets[0].revision==before->markets[0].revision+1U);
        if(is_bar){
            EVENT_CHECK(workspace->bar_histories[0].count==(!strcmp(which,"replace")?1U:2U));
            EVENT_CHECK(workspace->markets[0].bar.close==bar.close);
            EVENT_CHECK(!memcmp(tape_before,workspace->trade_tape,sizeof(*tape_before)));
        }else{
            EVENT_CHECK(workspace->trade_tape->count==2U);
            EVENT_CHECK(workspace->trade_tape->last_sequence==2U);
            EVENT_CHECK(workspace->markets[0].trade.price==101);
            EVENT_CHECK(!memcmp(&before->bar_histories[0],&workspace->bar_histories[0],
                sizeof(workspace->bar_histories[0])));
        }
    }
done:
    free(before);free(tape_before);
    if(direct_tape!=NULL)umi_trading_trade_tape_destroy(direct_tape);
    if(workspace!=NULL)umi_trading_workspace_destroy(workspace);
    return result;
}
