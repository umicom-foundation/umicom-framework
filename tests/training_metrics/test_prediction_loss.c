/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/training_metrics/test_prediction_loss.c
 *
 * PURPOSE:
 *   Verify selected log-loss sums against independent probabilities and existing tape behavior.
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
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#x); return 1; } } while(0)
static int near(double a,double b,double tolerance)
{ return isfinite(a) && isfinite(b) && fabs(a-b)<=tolerance*fmax(1.0,fabs(b)); }
static int uniform(void)
{
    float logits[64]={0};size_t targets[8]={0};UmiAiTrainingPredictionLossSummary result;
    for(size_t rows=1;rows<=8;++rows)for(size_t vocab=1;vocab<=8;++vocab){
        CHECK(UmiAiTrainingPredictionLoss(logits,rows,vocab,targets,NULL,0,&result)==UMI_STATUS_OK);
        CHECK(result.rows==rows && result.selectedRows==rows);
        CHECK(near(result.negativeLogLikelihoodSum,(double)rows*log((double)vocab),1e-13));
    }return 0;
}
static int probability(void)
{
    /* Expected losses come from specified probabilities, not the subject's
     * maximum/exp/log arithmetic. Float logits introduce a small input error. */
    const double p[6]={0.25,0.5,0.25,0.1,0.2,0.7};float logits[6];
    for(size_t i=0;i<6;++i)logits[i]=(float)log(p[i]);
    const size_t targets[2]={1,2};UmiAiTrainingPredictionLossSummary result;
    CHECK(UmiAiTrainingPredictionLoss(logits,2,3,targets,NULL,0,&result)==UMI_STATUS_OK);
    CHECK(near(result.negativeLogLikelihoodSum,-log(0.5)-log(0.7),1e-7));return 0;
}
static int large_offset(void)
{
    const float offsets[]={1e20f,-1e20f,FLT_MAX,-FLT_MAX};const size_t targets[2]={0,2};
    for(size_t i=0;i<4;++i){float logits[6];for(size_t j=0;j<6;++j)logits[j]=offsets[i];
        UmiAiTrainingPredictionLossSummary result;
        CHECK(UmiAiTrainingPredictionLoss(logits,2,3,targets,NULL,0,&result)==UMI_STATUS_OK);
        CHECK(near(result.negativeLogLikelihoodSum,2.0*log(3.0),1e-13));
    }return 0;
}
static int extremes(void)
{
    const float logits[4]={FLT_MAX,-FLT_MAX,-FLT_MAX,FLT_MAX};const size_t targets[2]={1,0};
    UmiAiTrainingPredictionLossSummary result;
    CHECK(UmiAiTrainingPredictionLoss(logits,2,2,targets,NULL,0,&result)==UMI_STATUS_OK);
    CHECK(near(result.negativeLogLikelihoodSum,4.0*(double)FLT_MAX,1e-13));
    /* No exp(loss) is attempted: the finite log-loss can imply an unrepresentable perplexity. */
    return 0;
}
static int offset_invariance(void)
{
    float logits[6]={-3,0,4,7,-2,1},shifted[6];const size_t targets[2]={0,1};
    for(size_t i=0;i<6;++i)shifted[i]=logits[i]+1024.0f;
    UmiAiTrainingPredictionLossSummary a,b;
    CHECK(UmiAiTrainingPredictionLoss(logits,2,3,targets,NULL,0,&a)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingPredictionLoss(shifted,2,3,targets,NULL,0,&b)==UMI_STATUS_OK);
    CHECK(near(a.negativeLogLikelihoodSum,b.negativeLogLikelihoodSum,1e-13));return 0;
}
static int selection(void)
{
    const float logits[9]={0,0,0,4,1,-3,0,0,0};const size_t targets[3]={1,2,0};const uint8_t mask[3]={1,0,1};
    UmiAiTrainingPredictionLossSummary result;
    CHECK(UmiAiTrainingPredictionLoss(logits,3,3,targets,mask,3,&result)==UMI_STATUS_OK);
    CHECK(result.rows==3 && result.selectedRows==2 && near(result.negativeLogLikelihoodSum,2.0*log(3.0),1e-13));return 0;
}
static int empty_selection(void)
{
    const float logits[4]={9,-9,8,-8};const size_t targets[2]={0,1};const uint8_t mask[2]={0,0};
    UmiAiTrainingPredictionLossSummary result={99,88,77};
    CHECK(UmiAiTrainingPredictionLoss(logits,2,2,targets,mask,2,&result)==UMI_STATUS_OK);
    CHECK(result.rows==2 && result.selectedRows==0 && result.negativeLogLikelihoodSum==0.0);return 0;
}
static int aggregate(void)
{
    const float logits[8]={0,0,0,0,0,0,0,4};const size_t targets[4]={0,0,0,0};
    UmiAiTrainingPredictionLossSummary all,first,last;
    CHECK(UmiAiTrainingPredictionLoss(logits,4,2,targets,NULL,0,&all)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingPredictionLoss(logits,3,2,targets,NULL,0,&first)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingPredictionLoss(logits+6,1,2,targets+3,NULL,0,&last)==UMI_STATUS_OK);
    CHECK(near(all.negativeLogLikelihoodSum,first.negativeLogLikelihoodSum+last.negativeLogLikelihoodSum,1e-13));
    const double mean=all.negativeLogLikelihoodSum/(double)all.selectedRows;
    const double wrong=(first.negativeLogLikelihoodSum/3.0+last.negativeLogLikelihoodSum)/2.0;
    CHECK(fabs(mean-wrong)>0.5);return 0;
}
static int nonfinite(void)
{
    const float bad[]={NAN,INFINITY,-INFINITY};const size_t targets[2]={0,1};const uint8_t mask[2]={1,0};
    for(size_t k=0;k<3;++k)for(size_t at=0;at<4;++at){float logits[4]={0};logits[at]=bad[k];
        UmiAiTrainingPredictionLossSummary result;memset(&result,0x5b,sizeof result);
        unsigned char before[sizeof result];memcpy(before,&result,sizeof result);
        CHECK(UmiAiTrainingPredictionLoss(logits,2,2,targets,mask,2,&result)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(before,&result,sizeof result)==0);
    }return 0;
}
static int refusals(void)
{
    const float logits[4]={0};size_t targets[2]={0,1};uint8_t mask[2]={1,0};
    UmiAiTrainingPredictionLossSummary result;memset(&result,0x5b,sizeof result);
    unsigned char before[sizeof result];memcpy(before,&result,sizeof result);
#define REFUSE(call,status) do { CHECK((call)==(status));CHECK(memcmp(before,&result,sizeof result)==0); } while(0)
    targets[1]=2;REFUSE(UmiAiTrainingPredictionLoss(logits,2,2,targets,mask,2,&result),UMI_STATUS_INVALID_ARGUMENT);targets[1]=1;
    mask[1]=2;REFUSE(UmiAiTrainingPredictionLoss(logits,2,2,targets,mask,2,&result),UMI_STATUS_INVALID_ARGUMENT);mask[1]=0;
    REFUSE(UmiAiTrainingPredictionLoss(logits,2,2,targets,mask,1,&result),UMI_STATUS_INVALID_ARGUMENT);
    REFUSE(UmiAiTrainingPredictionLoss(logits,2,2,targets,NULL,2,&result),UMI_STATUS_INVALID_ARGUMENT);
    REFUSE(UmiAiTrainingPredictionLoss(NULL,2,2,targets,mask,2,&result),UMI_STATUS_INVALID_ARGUMENT);
    REFUSE(UmiAiTrainingPredictionLoss(logits,2,2,NULL,mask,2,&result),UMI_STATUS_INVALID_ARGUMENT);
    REFUSE(UmiAiTrainingPredictionLoss(logits,0,2,targets,NULL,0,&result),UMI_STATUS_INVALID_ARGUMENT);
    REFUSE(UmiAiTrainingPredictionLoss(logits,2,0,targets,NULL,0,&result),UMI_STATUS_INVALID_ARGUMENT);
    REFUSE(UmiAiTrainingPredictionLoss(logits,SIZE_MAX,2,targets,NULL,0,&result),UMI_STATUS_CAPACITY_EXCEEDED);
    REFUSE(UmiAiTrainingPredictionLoss(logits,2,SIZE_MAX,targets,NULL,0,&result),UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(UmiAiTrainingPredictionLoss(logits,2,2,targets,NULL,0,NULL)==UMI_STATUS_INVALID_ARGUMENT);
#undef REFUSE
    return 0;
}
static int maximum(void)
{
    static float logits[UMI_AI_TRAINING_MAX_ELEMENTS];static size_t targets[4096];static uint8_t mask[4096];
    for(size_t i=0;i<4096;++i){targets[i]=i%4U;mask[i]=(uint8_t)(i%2U);}
    UmiAiTrainingPredictionLossSummary result;
    CHECK(UmiAiTrainingPredictionLoss(logits,4096,4,targets,mask,4096,&result)==UMI_STATUS_OK);
    CHECK(result.selectedRows==2048 && near(result.negativeLogLikelihoodSum,2048.0*log(4.0),1e-12));return 0;
}
static int ownership(void)
{
    float logits[6]={1,2,3,3,2,1},beforeLogits[6];size_t targets[2]={2,0},beforeTargets[2];uint8_t mask[2]={1,1},beforeMask[2];
    memcpy(beforeLogits,logits,sizeof logits);memcpy(beforeTargets,targets,sizeof targets);memcpy(beforeMask,mask,sizeof mask);
    UmiAiTrainingPredictionLossSummary result;
    CHECK(UmiAiTrainingPredictionLoss(logits,2,3,targets,mask,2,&result)==UMI_STATUS_OK);
    CHECK(memcmp(beforeLogits,logits,sizeof logits)==0 && memcmp(beforeTargets,targets,sizeof targets)==0 && memcmp(beforeMask,mask,sizeof mask)==0);
    const double sum=result.negativeLogLikelihoodSum;memset(logits,0,sizeof logits);targets[0]=0;mask[0]=0;
    CHECK(result.negativeLogLikelihoodSum==sum && result.selectedRows==2);return 0;
}
static int tape_parity(void)
{
    const float logits[9]={1,2,3,9,9,9,-1,0,1};const size_t targets[3]={2,1,0};const uint8_t mask[3]={1,0,1};
    UmiAiTrainingPredictionLossSummary summary;
    CHECK(UmiAiTrainingPredictionLoss(logits,3,3,targets,mask,3,&summary)==UMI_STATUS_OK);
    UmiAiTrainingTape *tape=NULL;UmiAiTrainingTensor *tensor=NULL,*loss=NULL;
    CHECK(UmiAiTrainingTapeCreate(&tape)==UMI_STATUS_OK);
/* The public tensor constructor requires an explicit trainable flag. This comparison leaf is not trainable; preserve the initial draft call for review. The former implementation is retained for owner review. */
#if 0
    CHECK(UmiAiTrainingTensorCreate(tape,3,3,logits,9,&tensor)==UMI_STATUS_OK);
#endif
    CHECK(UmiAiTrainingTensorCreate(tape,3,3,logits,9,0,&tensor)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingMaskedCrossEntropy(tape,tensor,targets,3,mask,3,&loss)==UMI_STATUS_OK);
/* The public tensor accessor copies into caller storage instead of returning an internal pointer. Use its actual ownership contract; retain the initial draft for review. The former implementation is retained for owner review. */
#if 0
    CHECK(near(UmiAiTrainingTensorValues(loss)[0],summary.negativeLogLikelihoodSum/2.0,1e-6));
#endif
    float tapeValue = 0.0f;
    CHECK(UmiAiTrainingTensorValues(loss,&tapeValue,1)==UMI_STATUS_OK);
    CHECK(near(tapeValue,summary.negativeLogLikelihoodSum/2.0,1e-6));
    UmiAiTrainingTapeDestroy(tape);return 0;
}
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    if(strcmp(argv[1],"uniform")==0)return uniform();
    if(strcmp(argv[1],"probability")==0)return probability();
    if(strcmp(argv[1],"large-offset")==0)return large_offset();
    if(strcmp(argv[1],"extremes")==0)return extremes();
    if(strcmp(argv[1],"offset-invariance")==0)return offset_invariance();
    if(strcmp(argv[1],"selection")==0)return selection();
    if(strcmp(argv[1],"empty-selection")==0)return empty_selection();
    if(strcmp(argv[1],"aggregate")==0)return aggregate();
    if(strcmp(argv[1],"nonfinite")==0)return nonfinite();
    if(strcmp(argv[1],"refusals")==0)return refusals();
    if(strcmp(argv[1],"maximum")==0)return maximum();
    if(strcmp(argv[1],"ownership")==0)return ownership();
    if(strcmp(argv[1],"tape-parity")==0)return tape_parity();
    return 2;
}
