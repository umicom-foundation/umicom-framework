/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/training_metrics/test_prediction_summary.c
 *
 * PURPOSE:
 *   Verify checked loss aggregation, no-data reporting and refusal without partial publication.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/training/prediction_metrics.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#x); return 1; } } while (0)
static int Near(double a, double b) { return fabs(a-b) <= 1e-12 * fmax(1.0, fabs(b)); }

static int Merge(void)
{
    const UmiAiTrainingPredictionLossSummary a={4,3,6}, b={3,1,4};
    UmiAiTrainingPredictionLossSummary result={0}; double mean=-1;
    CHECK(UmiAiTrainingPredictionLossMerge(&a,&b,&result)==UMI_STATUS_OK);
    CHECK(result.rows==7 && result.selectedRows==4 && result.negativeLogLikelihoodSum==10);
    CHECK(UmiAiTrainingPredictionLossMean(&result,&mean)==UMI_STATUS_OK && mean==2.5);
    CHECK(mean!=(6.0/3.0+4.0/1.0)/2.0);
    return 0;
}
static int Identity(void)
{
    const UmiAiTrainingPredictionLossSummary zero={0}, a={3,2,4}, excluded={7,0,0};
    UmiAiTrainingPredictionLossSummary result;
    CHECK(UmiAiTrainingPredictionLossMerge(&zero,&a,&result)==UMI_STATUS_OK);
    CHECK(result.rows==3 && result.selectedRows==2 && result.negativeLogLikelihoodSum==4);
    CHECK(UmiAiTrainingPredictionLossMerge(&result,&excluded,&result)==UMI_STATUS_OK);
    CHECK(result.rows==10 && result.selectedRows==2 && result.negativeLogLikelihoodSum==4);
    return 0;
}
static int Aliases(void)
{
    UmiAiTrainingPredictionLossSummary a={3,2,4}, b={2,1,3};
    CHECK(UmiAiTrainingPredictionLossMerge(&a,&b,&a)==UMI_STATUS_OK);
    CHECK(a.rows==5 && a.selectedRows==3 && a.negativeLogLikelihoodSum==7);
    CHECK(UmiAiTrainingPredictionLossMerge(&a,&b,&b)==UMI_STATUS_OK);
    CHECK(b.rows==7 && b.selectedRows==4 && b.negativeLogLikelihoodSum==10);
    CHECK(UmiAiTrainingPredictionLossMerge(&b,&b,&b)==UMI_STATUS_OK);
    CHECK(b.rows==14 && b.selectedRows==8 && b.negativeLogLikelihoodSum==20);
    return 0;
}
static int Streamed(void)
{
    const float logits[]={0,0,1,0,0,3,4,1}; const size_t targets[]={0,0,1,0};
    const uint8_t selected[]={1,0,1,1};
    UmiAiTrainingPredictionLossSummary total={0}, all, one;
    CHECK(UmiAiTrainingPredictionLoss(logits,4,2,targets,selected,4,&all)==UMI_STATUS_OK);
    for(size_t i=0;i<4;++i){
        CHECK(UmiAiTrainingPredictionLoss(logits+2*i,1,2,targets+i,selected+i,1,&one)==UMI_STATUS_OK);
        CHECK(UmiAiTrainingPredictionLossMerge(&total,&one,&total)==UMI_STATUS_OK);
    }
    CHECK(total.rows==all.rows && total.selectedRows==all.selectedRows);
    CHECK(Near(total.negativeLogLikelihoodSum,all.negativeLogLikelihoodSum));
    return 0;
}
static int Empty(void)
{
    const UmiAiTrainingPredictionLossSummary empty={9,0,0}; double value=77;
    CHECK(UmiAiTrainingPredictionLossMean(&empty,&value)==UMI_STATUS_UNAVAILABLE && value==77);
    CHECK(UmiAiTrainingPredictionLossPerplexity(&empty,&value)==UMI_STATUS_UNAVAILABLE && value==77);
    return 0;
}
static int Report(int perplexity)
{
    const UmiAiTrainingPredictionLossSummary uniform={6,3,3.0*log(4.0)}, perfect={2,2,0};
    double value=-1;
    if(perplexity){
        CHECK(UmiAiTrainingPredictionLossPerplexity(&uniform,&value)==UMI_STATUS_OK && Near(value,4));
        CHECK(UmiAiTrainingPredictionLossPerplexity(&perfect,&value)==UMI_STATUS_OK && value==1);
    }else{
        CHECK(UmiAiTrainingPredictionLossMean(&uniform,&value)==UMI_STATUS_OK && Near(value,log(4.0)));
        CHECK(UmiAiTrainingPredictionLossMean(&perfect,&value)==UMI_STATUS_OK && value==0);
    }
    return 0;
}
static int Invalid(const char *name)
{
    const UmiAiTrainingPredictionLossSummary valid={2,1,3};
    UmiAiTrainingPredictionLossSummary invalid=valid, output, before;
    if(strcmp(name,"malformed-count")==0) invalid.selectedRows=3;
    else if(strcmp(name,"malformed-empty")==0) invalid.selectedRows=0;
    else if(strcmp(name,"negative")==0) invalid.negativeLogLikelihoodSum=-1;
    else if(strcmp(name,"nan")==0) invalid.negativeLogLikelihoodSum=NAN;
    else invalid.negativeLogLikelihoodSum=INFINITY;
    memset(&output,0x5a,sizeof(output)); memcpy(&before,&output,sizeof(before));
    CHECK(UmiAiTrainingPredictionLossMerge(&valid,&invalid,&output)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&output,&before,sizeof(output))==0);
    CHECK(UmiAiTrainingPredictionLossMerge(&invalid,&valid,&output)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&output,&before,sizeof(output))==0);
    double value=77;
    CHECK(UmiAiTrainingPredictionLossMean(&invalid,&value)==UMI_STATUS_INVALID_ARGUMENT && value==77);
    CHECK(UmiAiTrainingPredictionLossPerplexity(&invalid,&value)==UMI_STATUS_INVALID_ARGUMENT && value==77);
    return 0;
}
static int Overflow(const char *name)
{
    UmiAiTrainingPredictionLossSummary a={1,1,DBL_MAX}, b=a, output, before;
    if(strcmp(name,"overflow-count")==0){ a.rows=SIZE_MAX; a.selectedRows=SIZE_MAX; a.negativeLogLikelihoodSum=1; b.negativeLogLikelihoodSum=1; }
    if(strcmp(name,"overflow-exp")==0){
        a.negativeLogLikelihoodSum=1000; double mean=-1, value=77;
        CHECK(UmiAiTrainingPredictionLossMean(&a,&mean)==UMI_STATUS_OK && mean==1000);
        CHECK(UmiAiTrainingPredictionLossPerplexity(&a,&value)==UMI_STATUS_CAPACITY_EXCEEDED && value==77);
        return 0;
    }
    memcpy(&output,&a,sizeof(output)); memcpy(&before,&output,sizeof(before));
    CHECK(UmiAiTrainingPredictionLossMerge(&output,&b,&output)==UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(&output,&before,sizeof(output))==0);
    return 0;
}
static int Nulls(void)
{
    const UmiAiTrainingPredictionLossSummary valid={1,1,0};
    UmiAiTrainingPredictionLossSummary output, before;
    memset(&output,0x5a,sizeof(output)); memcpy(&before,&output,sizeof(before));
    CHECK(UmiAiTrainingPredictionLossMerge(NULL,&valid,&output)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingPredictionLossMerge(&valid,NULL,&output)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&output,&before,sizeof(output))==0);
    CHECK(UmiAiTrainingPredictionLossMerge(&valid,&valid,NULL)==UMI_STATUS_INVALID_ARGUMENT);
    double value=77;
    CHECK(UmiAiTrainingPredictionLossMean(NULL,&value)==UMI_STATUS_INVALID_ARGUMENT && value==77);
    CHECK(UmiAiTrainingPredictionLossMean(&valid,NULL)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingPredictionLossPerplexity(NULL,&value)==UMI_STATUS_INVALID_ARGUMENT && value==77);
    CHECK(UmiAiTrainingPredictionLossPerplexity(&valid,NULL)==UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    const char *name=argv[1];
    if(strcmp(name,"merge")==0)return Merge();
    if(strcmp(name,"identity")==0)return Identity();
    if(strcmp(name,"aliases")==0)return Aliases();
    if(strcmp(name,"streamed")==0)return Streamed();
    if(strcmp(name,"empty")==0)return Empty();
    if(strcmp(name,"mean")==0)return Report(0);
    if(strcmp(name,"perplexity")==0)return Report(1);
    if(strcmp(name,"malformed-count")==0 || strcmp(name,"malformed-empty")==0 ||
       strcmp(name,"negative")==0 || strcmp(name,"nan")==0 || strcmp(name,"infinity")==0)return Invalid(name);
    if(strncmp(name,"overflow-",9)==0)return Overflow(name);
    if(strcmp(name,"nulls")==0)return Nulls();
    return 2;
}
