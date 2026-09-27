/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT */
#include "strategy.h"
#include "umicom/trading/quote.h"
#include <math.h>
#include <string.h>
UmiStatus PracticeStrategy(const UmiResearchView *view,void *data,UmiResearchDirection *target)
{
    const PracticeThresholds *p=data;
    if(view==NULL||p==NULL||target==NULL||!isfinite(p->enterBelow)||!isfinite(p->leaveAbove)||
        p->enterBelow<=0.0||p->leaveAbove<=p->enterBelow) return UMI_STATUS_INVALID_ARGUMENT;
    *target=UMI_RESEARCH_HOLD;
    /* Preserve a waiting order rather than resetting its latency every tick. */
    if(view->hasPending) return UMI_STATUS_OK;
    double mid=umi_quote_mid(&view->current.quote);
    if(view->position==0&&mid<p->enterBelow) *target=UMI_RESEARCH_LONG;
    if(view->position>0&&mid>p->leaveAbove) *target=UMI_RESEARCH_FLAT;
    return UMI_STATUS_OK;
}
UmiInstrument PracticeInstrument(void)
{
    UmiInstrument i={0};strcpy(i.instrument_id.value,"workshop-instrument");
    strcpy(i.symbol,"WORKSHOP");strcpy(i.venue,"PRACTICE");strcpy(i.currency.code,"GBP");
    i.multiplier=1.0;return i;
}
void PracticeQuotes(UmiResearchObservation out[8])
{
    const double bids[8]={99,100,103,104,98,99,103,102};
    for(size_t n=0;n<8;++n) {
        out[n]=(UmiResearchObservation){0};out[n].sequence=(uint64_t)n+1U;
        out[n].quote.instrument=PracticeInstrument();out[n].quote.event_time_ms=(int64_t)n*100;
        out[n].quote.bid=bids[n];out[n].quote.ask=bids[n]+1;
        out[n].quote.bid_size=10;out[n].quote.ask_size=10;
    }
}
