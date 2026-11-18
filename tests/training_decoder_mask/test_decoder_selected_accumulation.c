/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/training_decoder_mask/test_decoder_selected_accumulation.c
 *
 * PURPOSE:
 *   Verify selected-row weighted microbatches, checkpoint ownership and bounded graph lifetimes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/training/gradient_accumulation.h"
#include "umicom/ai/training/optimizer_checkpoint.h"
#include "../../src/ai/training/decoder_private.h"
#include "../../src/ai/training/optimizer_internal.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"%d: %s\n",__LINE__,#x);return 1; } } while(0)
#define OK(x) CHECK((x)==UMI_STATUS_OK)
static const UmiAiTrainingDecoderConfig config={8,3,4,8,2,42};
static const size_t ids[15]={1,2,3,4,5,6,1,3,5,2,4,6,0,2,4};
static const size_t targets[15]={2,3,4,5,6,7,2,4,6,3,5,7,1,3,5};
static const unsigned char attended[15]={1,0,1,1,0,1,1,1,1,0,1,1,1,0,1};
static const unsigned char scored[15]={0,0,0,1,0,0,1,1,1,0,1,1,0,0,0};
static const unsigned char all[15]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
typedef struct Pair { UmiAiTrainingDecoder *model; UmiAiTrainingAdamW *optimizer; } Pair;
typedef struct Snapshot { unsigned char bytes[65536];size_t count; } Snapshot;
static int create(Pair *p)
{
    UmiAiTrainingAdamWConfig options;*p=(Pair){0};
    OK(UmiAiTrainingAdamWDefaults(&options));options.weightDecay=0.01F;
    OK(UmiAiTrainingDecoderCreate(&config,&p->model));
    OK(UmiAiTrainingDecoderAdamWCreate(p->model,&options,&p->optimizer));
    return 0;
}
static void destroy(Pair *p)
{
    UmiAiTrainingAdamWDestroy(p->optimizer);UmiAiTrainingDecoderDestroy(p->model);*p=(Pair){0};
}
static int save(const Pair *p,Snapshot *s)
{
    OK(UmiAiTrainingAdamWCheckpointEncode(p->model,p->optimizer,s->bytes,sizeof s->bytes,&s->count));
    return 0;
}
static int same(const Pair *p,const Snapshot *s)
{
    Snapshot now;CHECK(save(p,&now)==0);
    CHECK(now.count==s->count && !memcmp(now.bytes,s->bytes,now.count));return 0;
}
static int stable(const Pair *p,const UmiAiTrainingTapeMemory *before)
{
    UmiAiTrainingTapeMemory after;OK(UmiAiTrainingDecoderMemoryInspect(p->model,&after));
    CHECK(after.currentBytes==before->currentBytes && after.tensorCount==before->tensorCount &&
        after.auxiliaryBytes==before->auxiliaryBytes);return 0;
}
static UmiStatus step(Pair *p,size_t micro,const unsigned char *attention,
    const unsigned char *selected,float *loss)
{
    return UmiAiTrainingDecoderTrainAdamWPaddedAccumulated(p->model,p->optimizer,
        ids,targets,15,micro,attention,15,selected,15,0.02F,loss);
}
static int parity(int whole,int legacy)
{
    Pair a,b;Snapshot state;float la,lb;
    CHECK(create(&a)==0 && create(&b)==0);
    for(size_t i=0;i<8;++i) {
        if(whole) {
            OK(UmiAiTrainingDecoderTrainAdamWPaddedStep(a.model,a.optimizer,
                ids,targets,15,attended,15,scored,15,0.02F,&la));
            OK(step(&b,5,attended,scored,&lb));
        } else if(legacy) {
            OK(UmiAiTrainingDecoderTrainAdamWAccumulated(a.model,a.optimizer,
                ids,targets,15,2,0.02F,&la));
            OK(UmiAiTrainingDecoderTrainAdamWMaskedAccumulated(b.model,b.optimizer,
                ids,targets,15,2,all,15,0.02F,&lb));
        } else {
            OK(UmiAiTrainingDecoderTrainAdamWMaskedAccumulated(a.model,a.optimizer,
                ids,targets,15,2,scored,15,0.02F,&la));
            OK(step(&b,2,all,scored,&lb));
        }
        CHECK(la==lb && save(&a,&state)==0 && same(&b,&state)==0);
    }
    destroy(&b);destroy(&a);return 0;
}
static int weighted(void)
{
    Pair p;float pieces[3],observed;CHECK(create(&p)==0);
    /* Three independent scored sequences contain 1, 3 and 2 objectives.
     * An unweighted average of their means is intentionally the wrong target. */
    for(size_t i=0;i<3;++i) {
        size_t start=(i+1U)*3U;
        OK(UmiAiTrainingDecoderEvaluatePadded(p.model,ids+start,targets+start,
            3,attended+start,3,scored+start,3,&pieces[i]));
    }
    double expected=((double)pieces[0]+3.0*(double)pieces[1]+2.0*(double)pieces[2])/6.0;
    OK(step(&p,1,attended,scored,&observed));
    CHECK(fabs((double)observed-expected)<1e-6);
    CHECK(p.model->completedSteps==1U);
    destroy(&p);return 0;
}
static int largeReference(void)
{
    Pair full,micro;float a,b;CHECK(create(&full)==0 && create(&micro)==0);
    UmiAiTrainingTapeMemory first;OK(UmiAiTrainingDecoderMemoryInspect(micro.model,&first));
    OK(UmiAiTrainingDecoderTrainAdamWPaddedStep(full.model,full.optimizer,ids,targets,
        15,attended,15,scored,15,0.02F,&a));
    OK(step(&micro,2,attended,scored,&b));
    CHECK(fabsf(a-b)<2e-6F);
    /* Check actual parameters and subsequent predictions, not only scalar loss.
     * FP32 operation ordering is allowed to differ across graph partitions. */
    for(size_t p=0;p<UMI_DECODER_PARAMETER_COUNT;++p) {
        size_t rows,cols;float x[128],y[128];
        UmiAiDecoderParameterShape(&config,p,&rows,&cols);
        CHECK(rows*cols<=128U);
        OK(UmiAiTrainingTensorValues(full.model->parameters[p],x,rows*cols));
        OK(UmiAiTrainingTensorValues(micro.model->parameters[p],y,rows*cols));
        for(size_t j=0;j<rows*cols;++j)CHECK(fabsf(x[j]-y[j])<2e-4F);
    }
    float x[120],y[120];
    OK(UmiAiTrainingDecoderLogits(full.model,ids,15,x,120));
    OK(UmiAiTrainingDecoderLogits(micro.model,ids,15,y,120));
    for(size_t i=0;i<120;++i)CHECK(fabsf(x[i]-y[i])<5e-4F);
    CHECK(stable(&micro,&first)==0);
    destroy(&micro);destroy(&full);return 0;
}
static int invariance(void)
{
    Pair a,b;Snapshot state;float la,lb;size_t input[15],target[15];
    memcpy(input,ids,sizeof input);memcpy(target,targets,sizeof target);
    for(size_t i=0;i<15;++i) {
        if(!attended[i])input[i]=(input[i]+1U)%8U;
        if(!scored[i])target[i]=(target[i]+1U)%8U;
    }
    /* Whole unscored sequence groups are independent and can be changed too. */
    for(size_t i=0;i<3;++i)input[i]=(input[i]+2U)%8U;
    for(size_t i=12;i<15;++i)input[i]=(input[i]+3U)%8U;
    CHECK(create(&a)==0 && create(&b)==0);
    for(size_t i=0;i<10;++i) {
        OK(step(&a,1,attended,scored,&la));
        OK(UmiAiTrainingDecoderTrainAdamWPaddedAccumulated(b.model,b.optimizer,
            input,target,15,1,attended,15,scored,15,0.02F,&lb));
        CHECK(la==lb && save(&a,&state)==0 && same(&b,&state)==0);
    }
    destroy(&b);destroy(&a);return 0;
}
static int rejections(void)
{
    Pair p;Snapshot state;float loss=-77;unsigned char invalid[15],empty[15]={0};
    size_t bad[15];memcpy(bad,ids,sizeof bad);bad[14]=8U;
    memcpy(invalid,scored,sizeof invalid);invalid[14]=2U;
    CHECK(create(&p)==0 && save(&p,&state)==0);
    UmiAiTrainingTapeMemory before;OK(UmiAiTrainingDecoderMemoryInspect(p.model,&before));
    CHECK(step(&p,0,attended,scored,&loss)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(step(&p,SIZE_MAX,attended,scored,&loss)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(step(&p,1,attended,empty,&loss)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(step(&p,1,attended,invalid,&loss)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(step(&p,1,invalid,scored,&loss)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(step(&p,1,attended,all,&loss)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(step(&p,1,NULL,scored,&loss)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(step(&p,1,attended,NULL,&loss)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(step(&p,1,attended,scored,NULL)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingDecoderTrainAdamWPaddedAccumulated(p.model,p.optimizer,
        bad,targets,15,1,attended,15,scored,15,0.02F,&loss)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingDecoderTrainAdamWPaddedAccumulated(p.model,p.optimizer,
        ids,bad,15,1,attended,15,scored,15,0.02F,&loss)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingDecoderTrainAdamWMaskedAccumulated(p.model,p.optimizer,
        ids,targets,15,1,scored,14,0.02F,&loss)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingDecoderTrainAdamWMaskedAccumulated(p.model,p.optimizer,
        ids,targets,14,1,scored,14,0.02F,&loss)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingDecoderTrainAdamWMaskedAccumulated(p.model,p.optimizer,
        ids,targets,15,1,scored,15,NAN,&loss)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingDecoderTrainAdamWMaskedAccumulated(p.model,p.optimizer,
        ids,targets,15,1,NULL,0,0.02F,&loss)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(loss==-77 && same(&p,&state)==0 && stable(&p,&before)==0);
    destroy(&p);return 0;
}
static int memory(void)
{
    Pair full,micro;float loss;CHECK(create(&full)==0 && create(&micro)==0);
    OK(UmiAiTrainingDecoderTrainAdamWPaddedStep(full.model,full.optimizer,
        ids,targets,15,attended,15,scored,15,0.02F,&loss));
    OK(step(&micro,1,attended,scored,&loss));
    UmiAiTrainingTapeMemory a,b;OK(UmiAiTrainingDecoderMemoryInspect(full.model,&a));
    OK(UmiAiTrainingDecoderMemoryInspect(micro.model,&b));
    CHECK(b.peakBytes<a.peakBytes && b.currentBytes==a.currentBytes);
    for(size_t i=0;i<50;++i) {OK(step(&micro,1,attended,scored,&loss));CHECK(stable(&micro,&b)==0);}
    CHECK(micro.model->completedSteps==51U);
    printf("full_graph_peak=%zu micro_graph_peak=%zu retained=%zu\n",a.peakBytes,b.peakBytes,b.currentBytes);
    destroy(&micro);destroy(&full);return 0;
}
static int budget(void)
{
    Pair p;Snapshot state;float loss=-77;CHECK(create(&p)==0 && save(&p,&state)==0);
    UmiAiTrainingTapeMemory before;OK(UmiAiTrainingDecoderMemoryInspect(p.model,&before));
/* The earlier test misspelled the public budget operation. Use the declared API; retain the rejected call for review. The former implementation is retained for owner review. */
#if 0
    OK(UmiAiTrainingDecoderSetMemoryLimit(p.model,before.currentBytes));
#endif
    OK(UmiAiTrainingDecoderSetMemoryBudget(p.model,before.currentBytes));
    CHECK(step(&p,1,attended,scored,&loss)==UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(loss==-77 && same(&p,&state)==0 && stable(&p,&before)==0);
    destroy(&p);return 0;
}
static int resume(void)
{
    Pair p,q={0};Snapshot state;float a,b;CHECK(create(&p)==0);
    for(size_t i=0;i<4;++i)OK(step(&p,2,attended,scored,&a));
    CHECK(save(&p,&state)==0);
    OK(UmiAiTrainingAdamWCheckpointDecode(state.bytes,state.count,&q.model,&q.optimizer));
    for(size_t i=0;i<8;++i) {
        OK(step(&p,2,attended,scored,&a));OK(step(&q,2,attended,scored,&b));
        CHECK(a==b && save(&p,&state)==0 && same(&q,&state)==0);
    }
    destroy(&q);destroy(&p);return 0;
}
static int binding(void)
{
    Pair a,b;Snapshot sa,sb;float loss=-77;CHECK(create(&a)==0 && create(&b)==0);
    CHECK(save(&a,&sa)==0 && save(&b,&sb)==0);
    CHECK(UmiAiTrainingDecoderTrainAdamWMaskedAccumulated(a.model,b.optimizer,
        ids,targets,15,2,scored,15,0.02F,&loss)==UMI_STATUS_INVALID_STATE);
    CHECK(loss==-77 && same(&a,&sa)==0 && same(&b,&sb)==0);
    destroy(&b);destroy(&a);return 0;
}

static int updateFailure(void)
{
    Pair p;Snapshot state;float loss=-77;UmiAiTrainingTapeMemory before;
    UmiAiTrainingAdamWConfig options;OK(UmiAiTrainingAdamWDefaults(&options));
    options.weightDecay=FLT_MAX;p=(Pair){0};
    OK(UmiAiTrainingDecoderCreate(&config,&p.model));
    OK(UmiAiTrainingDecoderAdamWCreate(p.model,&options,&p.optimizer));
    CHECK(save(&p,&state)==0);
    OK(UmiAiTrainingDecoderMemoryInspect(p.model,&before));
    CHECK(UmiAiTrainingDecoderTrainAdamWPaddedAccumulated(p.model,p.optimizer,
        ids,targets,15,1,attended,15,scored,15,FLT_MAX,&loss)!=UMI_STATUS_OK);
    CHECK(loss==-77 && same(&p,&state)==0 && stable(&p,&before)==0);
    destroy(&p);return 0;
}
static int overflow(void)
{
    Pair p;Snapshot state;float loss=-77;UmiAiTrainingAdamWConfig options;
    uint64_t steps;size_t parameterCount,elements;const size_t *counts;
    const double *first,*second;
    CHECK(create(&p)==0);
    OK(UmiAiTrainingAdamWInternalInspect(p.optimizer,&options,&steps,
        &parameterCount,&elements,&counts,&first,&second));
    OK(UmiAiTrainingAdamWInternalRestore(p.optimizer,first,second,elements,UINT64_MAX));
    p.model->completedSteps=UINT64_MAX;CHECK(save(&p,&state)==0);
    CHECK(step(&p,1,attended,scored,&loss)==UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(loss==-77 && same(&p,&state)==0);destroy(&p);return 0;
}

int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    if(!strcmp(argv[1],"whole-parity"))return parity(1,0);
    if(!strcmp(argv[1],"legacy-parity"))return parity(0,1);
    if(!strcmp(argv[1],"selection-parity"))return parity(0,0);
    if(!strcmp(argv[1],"weighted-loss"))return weighted();
    if(!strcmp(argv[1],"large-reference"))return largeReference();
    if(!strcmp(argv[1],"invariance"))return invariance();
    if(!strcmp(argv[1],"rejections"))return rejections();
    if(!strcmp(argv[1],"memory"))return memory();
    if(!strcmp(argv[1],"budget"))return budget();
    if(!strcmp(argv[1],"resume"))return resume();
    if(!strcmp(argv[1],"binding"))return binding();
    if(!strcmp(argv[1],"update-failure"))return updateFailure();
    if(!strcmp(argv[1],"step-overflow"))return overflow();
    return 2;
}
