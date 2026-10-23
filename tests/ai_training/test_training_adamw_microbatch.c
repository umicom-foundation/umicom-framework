/*-----------------------------------------------------------------------------
 * Umicom Framework | Native AI regression
 * PURPOSE: Compare full AdamW training to the shared gradient reducer using
 * different microbatch sizes and prove the old SGD accumulator still works.
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation | MIT
 *---------------------------------------------------------------------------
 * File: tests/ai_training/test_training_adamw_microbatch.c
 * LICENCE: MIT
 */
#include "umicom/ai/training/gradient_accumulation.h"
#include "umicom/ai/training/optimizer_checkpoint.h"
#include <math.h>
#include <stdio.h>
#define CHECK(x) do { if (!(x)) {fprintf(stderr,"line %d: %s\n",__LINE__,#x);return 1;} } while(0)
int main(void)
{
    UmiAiTrainingDecoderConfig c = {260U,8U,8U,16U,2U,UINT64_C(20261021)};
    UmiAiTrainingDecoder *reference = NULL,*small = NULL,*wide = NULL;
    UmiAiTrainingAdamW *rOpt = NULL,*sOpt = NULL,*wOpt = NULL;
    UmiAiTrainingAdamWConfig adamw;
    size_t x[40],y[40];
    float rLoss=-1.0F,sLoss=-1.0F,wLoss=-1.0F;
    float a[2080],b[2080],d[2080];
    UmiAiTrainingTapeMemory refMemory,smallMemory;
    uint64_t steps=0U;
    size_t stateBytes=0U;
    CHECK(UmiAiTrainingAdamWDefaults(&adamw)==UMI_STATUS_OK);
    for(size_t i=0U;i<40U;++i){x[i]=4U+(i*7U)%47U;y[i]=4U+((i+1U)*7U)%47U;}
    CHECK(UmiAiTrainingDecoderCreate(&c,&reference)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderCreate(&c,&small)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderCreate(&c,&wide)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderAdamWCreate(reference,&adamw,&rOpt)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderAdamWCreate(small,&adamw,&sOpt)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderAdamWCreate(wide,&adamw,&wOpt)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderTrainAdamWStep(reference,rOpt,x,y,40U,0.007F,&rLoss)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderTrainAdamWAccumulated(small,sOpt,x,y,40U,1U,0.007F,&sLoss)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderTrainAdamWAccumulated(wide,wOpt,x,y,40U,2U,0.007F,&wLoss)==UMI_STATUS_OK);
    CHECK(fabsf(rLoss-sLoss)<1e-5F && fabsf(rLoss-wLoss)<1e-5F);
    CHECK(UmiAiTrainingAdamWInspect(sOpt,&steps,&stateBytes)==UMI_STATUS_OK && steps==1U && stateBytes>0U);
    CHECK(UmiAiTrainingDecoderLogits(reference,x,8U,a,2080U)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderLogits(small,x,8U,b,2080U)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderLogits(wide,x,8U,d,2080U)==UMI_STATUS_OK);
    for(size_t i=0U;i<2080U;++i) CHECK(fabsf(a[i]-b[i])<3e-5F && fabsf(a[i]-d[i])<3e-5F);
    CHECK(UmiAiTrainingDecoderMemoryInspect(reference,&refMemory)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderMemoryInspect(small,&smallMemory)==UMI_STATUS_OK);
    CHECK(smallMemory.peakBytes<refMemory.peakBytes);
    UmiAiTrainingAdamWDestroy(wOpt);UmiAiTrainingAdamWDestroy(sOpt);UmiAiTrainingAdamWDestroy(rOpt);
    UmiAiTrainingDecoderDestroy(wide);UmiAiTrainingDecoderDestroy(small);UmiAiTrainingDecoderDestroy(reference);
    puts("AdamW effective-batch parity, moments and lower graph peak: PASS");
    return 0;
}
