/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/market_tape/test_alloc.c
 * PURPOSE:
 *   Check allocation failure handling in the market tape service.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Umicom Foundation | Sammy Hegab | MIT. Linux linker fault injection only. */
#include "umicom/trading/market_tape.h"
#include <stdio.h>
#include <stdlib.h>
static unsigned failure=0U,calls=0U;
void *__real_calloc(size_t count,size_t size);
void *__wrap_calloc(size_t count,size_t size)
{
    ++calls;
    if(failure!=0U&&calls==failure)return NULL;
    return __real_calloc(count,size);
}
int main(void)
{
    UmiMarketTapeConfig c=UmiMarketTapeConfigDefault();
    for(unsigned n=1U;n<=2U;++n){UmiMarketTape *t=NULL;calls=0U;failure=n;
        UmiStatus status=UmiMarketTapeCreate(&c,&t);failure=0U;
        if(status!=UMI_STATUS_OUT_OF_MEMORY||t!=NULL)return 1;
    }
    UmiMarketTape *t=NULL;failure=0U;
    if(UmiMarketTapeCreate(&c,&t)!=UMI_STATUS_OK)return 1;
    UmiInstrument i={.instrument_id={"A"},.symbol="A",.venue="SIM",.currency={"GBP"},.multiplier=1.0};
    if(UmiMarketTapeRegister(t,&i)!=UMI_STATUS_OK||UmiMarketTapeBegin(t,&i.instrument_id,1U,1U)!=UMI_STATUS_OK)return 1;
    UmiMarketTapePacket p={.kind=UMI_MARKET_TAPE_TRADE,.generation=1U,.sequence=1U,.receivedTimeMs=1000};
    p.value.trade=(UmiTradeTick){i,100.0,1.0,1000};calls=0U;failure=1U;
    UmiStatus status=UmiMarketTapeApply(t,&p,NULL);failure=0U;
    if(status!=UMI_STATUS_OK||calls!=0U)return 1;
    UmiMarketTapeDestroy(t);puts("Allocation failures cleaned; Apply allocated no memory.");return 0;
}
