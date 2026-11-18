/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/denoising/test_normal_allocation.c
 *
 * PURPOSE:
 *   Inject scratch allocation failure and inspect sample clearing before release.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/training/initialisation.h"
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"%d: %s\n",__LINE__,#x); return 1; } } while (0)
static int refuse,live,uncleared;
static size_t allocatedBytes;
static void *SampleAllocate(size_t bytes)
{
    if(refuse)return NULL;
    void *memory=malloc(bytes);
    if(memory!=NULL){allocatedBytes=bytes;++live;}
    return memory;
}
static void SampleRelease(void *memory)
{
    if(memory==NULL)return;
    const unsigned char *bytes=memory;
    for(size_t i=0U;i<allocatedBytes;++i)if(bytes[i]!=0U)uncleared=1;
    --live;free(memory);
}
/* Interpose only this production transform's scratch allocation. Inspect
 * while it is still alive; never read the buffer after the real free. */
#define malloc SampleAllocate
#define free SampleRelease
#include "../../src/ai/training/normal_noise.c"
#undef malloc
#undef free
int main(void)
{
    UmiAiTrainingRandomState state={42U},original=state;
    float output[64],before[64];memset(output,0x5a,sizeof(output));memcpy(before,output,sizeof(output));
    refuse=1;
    CHECK(UmiAiTrainingRandomFillNormal(&state,0.0F,1.0F,output,64U)==UMI_STATUS_OUT_OF_MEMORY);
    CHECK(state.state==original.state&&memcmp(output,before,sizeof(output))==0&&live==0);
    refuse=0;
    CHECK(UmiAiTrainingRandomFillNormal(&state,0.0F,1.0F,output,64U)==UMI_STATUS_OK);
    CHECK(live==0&&!uncleared);
    state=original;
    CHECK(UmiAiTrainingRandomFillNormal(&state,FLT_MAX,FLT_MAX,output,64U)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(live==0&&!uncleared&&state.state==original.state);
    return 0;
}
