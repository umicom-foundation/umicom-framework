/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/research_replay/test_alloc.c
 * PURPOSE:
 *   Check allocation failure handling in the research replay service.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT */
#include "strategy.h"
#include <stdio.h>
#include <stdlib.h>
static size_t failAt,calls;
void *__real_calloc(size_t,size_t);
void *__wrap_calloc(size_t n,size_t size) {calls++;return failAt&&calls==failAt?NULL:__real_calloc(n,size);}
static UmiStatus Hold(const UmiResearchView *v,void *d,UmiResearchDirection *t)
{(void)v;(void)d;*t=UMI_RESEARCH_HOLD;return UMI_STATUS_OK;}
int main(void)
{
    UmiResearchObservation e[8];PracticeQuotes(e);UmiResearchConfig c=UmiResearchConfigDefault();
    for(size_t n=1;n<=3;n++) {
        calls=0;failAt=n;UmiResearchReplay *r=NULL;
        UmiStatus s=UmiResearchReplayCreate(&c,e,8,Hold,NULL,&r);
        failAt=0;if(s!=UMI_STATUS_OUT_OF_MEMORY||r!=NULL) return 1;
    }
    puts("All three creation allocation failures left no published replay.");return 0;
}
