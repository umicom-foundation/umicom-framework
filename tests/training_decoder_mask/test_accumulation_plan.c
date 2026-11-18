/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/training_decoder_mask/test_accumulation_plan.c
 *
 * PURPOSE:
 *   Check independent reducer payload counts, partition boundaries and refusal atomicity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/training/capacity_plan.h"
#include "umicom/ai/training/gradient_accumulation.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do {if(!(x)){fprintf(stderr,"%d: %s\n",__LINE__,#x);return 1;}}while(0)
static const UmiAiTrainingDecoderConfig small={8,3,4,8,2,42};
static int values(void)
{
    UmiAiTrainingAccumulationPlan p;
    CHECK(UmiAiTrainingAccumulationPlanCompute(&small,5,2,&p)==UMI_STATUS_OK);
    /* Independently counted: embeddings 44; four norm vectors 16;
     * four 4x4 attention matrices 64; MLP 76; logits projection 32. */
    CHECK(p.graph.parameterElements==232U && p.largestParameterElements==32U);
    CHECK(p.accumulatorBytes==232U*sizeof(double));
    CHECK(p.reducedGradientBytes==232U*sizeof(float));
    CHECK(p.scratchBytes==32U*sizeof(float));
    CHECK(p.peakReductionPayloadBytes==232U*(sizeof(double)+sizeof(float))+32U*sizeof(float));
    CHECK(p.effectiveSequences==5 && p.effectiveTokens==15 &&
          p.requestedMicrobatchSequences==2 && p.maximumGraphs==3 && p.lastGraphSequences==1);
    CHECK(p.graph.tokensPerBatch==6 && p.graph.logitsElements==48 &&
          p.graph.attentionProbabilityElements==36);
    return 0;
}
static int partition(void)
{
    for(size_t effective=1;effective<=100;++effective)
        for(size_t micro=1;micro<=120;++micro) {
            UmiAiTrainingAccumulationPlan p;
            CHECK(UmiAiTrainingAccumulationPlanCompute(&small,effective,micro,&p)==UMI_STATUS_OK);
            CHECK(p.maximumGraphs>0 && p.lastGraphSequences>0 && p.lastGraphSequences<=micro);
            CHECK((p.maximumGraphs-1U)*micro+p.lastGraphSequences==effective);
            CHECK(p.graph.tokensPerBatch==(effective<micro?effective:micro)*3U);
            CHECK(p.scratchBytes==32U*sizeof(float));
        }
    return 0;
}
static int limits(void)
{
    UmiAiTrainingDecoderConfig c={260,8,8,16,2,1};
    UmiAiTrainingAccumulationPlan p,old;
    CHECK(UmiAiTrainingAccumulationPlanCompute(&c,8192,7,&p)==UMI_STATUS_OK);
    CHECK(p.effectiveTokens==65536 && p.maximumGraphs==1171 && p.lastGraphSequences==2);
    CHECK(p.graph.parameterElements==4792 && p.largestParameterElements==2080);
    CHECK(p.peakReductionPayloadBytes==4792U*(sizeof(double)+sizeof(float))+2080U*sizeof(float));
    old=p;
    CHECK(UmiAiTrainingAccumulationPlanCompute(&c,8193,7,&p)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(!memcmp(&p,&old,sizeof p));
    CHECK(UmiAiTrainingAccumulationPlanCompute(&c,8,8,&p)==UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(!memcmp(&p,&old,sizeof p));
    CHECK(UmiAiTrainingAccumulationPlanCompute(&c,1,8192,&p)==UMI_STATUS_OK);
    CHECK(p.maximumGraphs==1 && p.lastGraphSequences==1 && p.graph.tokensPerBatch==8);
    old=p;
    CHECK(UmiAiTrainingAccumulationPlanCompute(&c,1,8193,&p)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(!memcmp(&p,&old,sizeof p));
    return 0;
}
static int refusal(void)
{
    UmiAiTrainingAccumulationPlan p,old;memset(&p,0xa5,sizeof p);memcpy(&old,&p,sizeof p);
    UmiAiTrainingDecoderConfig c=small;c.attentionHeads=3;
    CHECK(UmiAiTrainingAccumulationPlanCompute(NULL,5,1,&p)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingAccumulationPlanCompute(&small,5,1,NULL)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingAccumulationPlanCompute(&small,0,1,&p)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingAccumulationPlanCompute(&small,1,0,&p)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingAccumulationPlanCompute(&small,SIZE_MAX,1,&p)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingAccumulationPlanCompute(&small,1,SIZE_MAX,&p)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingAccumulationPlanCompute(&c,5,1,&p)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(!memcmp(&p,&old,sizeof p));
    return 0;
}
static int graphBoundary(void)
{
    UmiAiTrainingCapacityPlan base;
    UmiAiTrainingAccumulationPlan p;
    CHECK(UmiAiTrainingCapacityPlanCompute(&small,1,&base)==UMI_STATUS_OK);
    size_t max=base.maximumBatchSequences;
    CHECK(UmiAiTrainingAccumulationPlanCompute(&small,max+1,max,&p)==UMI_STATUS_OK);
    CHECK(p.maximumGraphs==2 && p.lastGraphSequences==1);
    CHECK(UmiAiTrainingAccumulationPlanCompute(&small,max+1,max+1,&p)==UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(p.maximumGraphs==2 && p.lastGraphSequences==1);
    return 0;
}
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    if(!strcmp(argv[1],"values"))return values();
    if(!strcmp(argv[1],"partition"))return partition();
    if(!strcmp(argv[1],"limits"))return limits();
    if(!strcmp(argv[1],"refusal"))return refusal();
    if(!strcmp(argv[1],"graph-boundary"))return graphBoundary();
    return 2;
}
