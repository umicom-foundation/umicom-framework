/*-----------------------------------------------------------------------------
 * Umicom Framework | Native AI regression
 * PURPOSE: Reject invalid and stale AdamW gradient accumulation requests
 * without changing caller outputs, model weights or optimiser-step counters.
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation | MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/gradient_accumulation.h"
#include "umicom/ai/training/optimizer_checkpoint.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"line %d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(void)
{
    UmiAiTrainingDecoderConfig cfg={12U,3U,4U,8U,2U,42U};
    UmiAiTrainingDecoder *a=NULL,*other=NULL;
    UmiAiTrainingAdamW *opt=NULL,*wrong=NULL;
    UmiAiTrainingAdamWConfig options;
    size_t tokens[9]={1U,2U,3U,2U,3U,4U,3U,4U,5U};
    size_t targets[9]={2U,3U,4U,3U,4U,5U,4U,5U,6U};
    size_t invalid[9]={1U,2U,3U,2U,3U,4U,3U,4U,12U};
    float untouched=-123.0F;
    uint64_t steps=999U;size_t bytes=0U;
    size_t sizeBefore=0U, sizeAfter=0U, w1=0U, w2=0U;
    uint8_t *before=NULL,*after=NULL;
    CHECK(UmiAiTrainingAdamWDefaults(&options)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderCreate(&cfg,&a)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderCreate(&cfg,&other)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderAdamWCreate(a,&options,&opt)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderAdamWCreate(other,&options,&wrong)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingAdamWCheckpointSize(a,opt,&sizeBefore)==UMI_STATUS_OK);
    before=malloc(sizeBefore);after=malloc(sizeBefore);
    CHECK(before!=NULL && after!=NULL);
    CHECK(UmiAiTrainingAdamWCheckpointEncode(a,opt,before,sizeBefore,&w1)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderTrainAdamWAccumulated(a,opt,tokens,targets,9U,0U,0.01F,&untouched)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingDecoderTrainAdamWAccumulated(a,opt,invalid,targets,9U,1U,0.01F,&untouched)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingDecoderTrainAdamWAccumulated(a,wrong,tokens,targets,9U,1U,0.01F,&untouched)!=UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderTrainAdamWAccumulated(a,opt,tokens,targets,9U,1U,0.0F,&untouched)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(untouched==-123.0F);
    CHECK(UmiAiTrainingAdamWInspect(opt,&steps,&bytes)==UMI_STATUS_OK && steps==0U);
    CHECK(UmiAiTrainingAdamWCheckpointSize(a,opt,&sizeAfter)==UMI_STATUS_OK && sizeBefore==sizeAfter);
    CHECK(UmiAiTrainingAdamWCheckpointEncode(a,opt,after,sizeAfter,&w2)==UMI_STATUS_OK);
    CHECK(w1==w2 && memcmp(before,after,w1)==0);
    free(before);free(after);UmiAiTrainingAdamWDestroy(wrong);UmiAiTrainingAdamWDestroy(opt);
    UmiAiTrainingDecoderDestroy(other);UmiAiTrainingDecoderDestroy(a);
    puts("AdamW accumulated training rejects invalid inputs without mutation PASS");
    return 0;
}
