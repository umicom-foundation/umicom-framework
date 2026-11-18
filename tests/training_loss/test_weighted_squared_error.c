/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/training_loss/test_weighted_squared_error.c
 *
 * PURPOSE:
 *   Verify continuous objective weighting, derivatives, numeric admission and owned storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/training/tape.h"
#include <float.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do {if(!(x)){fprintf(stderr,"%d: %s\n",__LINE__,#x);return 1;}}while(0)
#define OK(x) CHECK((x)==UMI_STATUS_OK)
static int evaluate(const float *p,const float *t,const float *w,float *value,float *pg,float *tg)
{
    UmiAiTrainingTape *tape=NULL;UmiAiTrainingTensor *a=NULL,*b=NULL,*loss=NULL;
    OK(UmiAiTrainingTapeCreate(&tape));OK(UmiAiTrainingTensorCreate(tape,2,2,p,4,1,&a));
    OK(UmiAiTrainingTensorCreate(tape,2,2,t,4,1,&b));
    OK(UmiAiTrainingWeightedMeanSquaredError(tape,a,b,w,4,&loss));
    OK(UmiAiTrainingTensorValues(loss,value,1));OK(UmiAiTrainingBackward(tape,loss));
    OK(UmiAiTrainingTensorGradients(a,pg,4));OK(UmiAiTrainingTensorGradients(b,tg,4));
    UmiAiTrainingTapeDestroy(tape);return 0;
}
static int numerical(void)
{
    float p[4]={-1,2,3,-4},t[4]={0.3F,1.2F,-0.5F,2},w[4]={0.25F,2.5F,0.5F,0};
    float value,pg[4],tg[4],tmp[4],unused[4];CHECK(!evaluate(p,t,w,&value,pg,tg));
    for(size_t operand=0;operand<2;++operand)for(size_t i=0;i<4;++i) {
        float *x=operand==0?p:t;float saved=x[i],plus,minus;
        x[i]=saved+0.002F;CHECK(!evaluate(p,t,w,&plus,tmp,unused));
        x[i]=saved-0.002F;CHECK(!evaluate(p,t,w,&minus,tmp,unused));x[i]=saved;
        CHECK(fabs((double)(operand==0?pg[i]:tg[i])-((double)plus-minus)/0.004)<0.001);
    }
    return 0;
}
static int rescaling(void)
{
    float p[4]={1,2,3,4},t[4]={0},w[4]={1,0,3,0},v,pg[4],tg[4],v2,x[4],y[4];
    CHECK(!evaluate(p,t,w,&v,pg,tg));
    const float scales[3]={FLT_MIN,0.25F,FLT_MAX/4.0F};
    for(size_t k=0;k<3;++k) {
        float scaled[4];for(size_t i=0;i<4;++i)scaled[i]=w[i]*scales[k];
        CHECK(!evaluate(p,t,scaled,&v2,x,y));CHECK(fabsf(v-v2)<1e-5F);
        for(size_t i=0;i<4;++i)CHECK(fabsf(pg[i]-x[i])<1e-5F&&fabsf(tg[i]-y[i])<1e-5F);
    }
    return 0;
}
static int full(void)
{
    size_t n=UMI_AI_TRAINING_MAX_ELEMENTS;
    float *p=calloc(n,sizeof *p),*w=calloc(n,sizeof *w),*g=calloc(n,sizeof *g);CHECK(p&&w&&g);
    UmiAiTrainingTape *tape=NULL;UmiAiTrainingTensor *a=NULL,*b=NULL,*loss=NULL;
    OK(UmiAiTrainingTapeCreate(&tape));OK(UmiAiTrainingTensorCreate(tape,1,n,p,n,1,&b));
    p[n-1]=3;w[n-1]=FLT_MIN;
    OK(UmiAiTrainingTensorCreate(tape,1,n,p,n,1,&a));
    OK(UmiAiTrainingWeightedMeanSquaredError(tape,a,b,w,n,&loss));
    float v;OK(UmiAiTrainingTensorValues(loss,&v,1));CHECK(v==9);
    OK(UmiAiTrainingBackward(tape,loss));OK(UmiAiTrainingTensorGradients(a,g,n));
    for(size_t i=0;i<n-1;++i)CHECK(g[i]==0);
    CHECK(g[n-1]==6);UmiAiTrainingTapeDestroy(tape);free(g);free(w);free(p);return 0;
}
int main(int argc,char **argv)
{
    CHECK(argc==2);const char *mode=argv[1];
    if(!strcmp(mode,"finite-difference"))return numerical();
    if(!strcmp(mode,"rescaling"))return rescaling();
    if(!strcmp(mode,"full-capacity"))return full();
    float p[4]={1,2,3,4},t[4]={0},w[4]={1,0,3,0},v,pg[4],tg[4];
    if(!strcmp(mode,"reference")||!strcmp(mode,"excluded-extremes")) {
        if(!strcmp(mode,"excluded-extremes")){p[1]=FLT_MAX;t[1]=-FLT_MAX;p[3]=-FLT_MAX;t[3]=FLT_MAX;}
        CHECK(!evaluate(p,t,w,&v,pg,tg));CHECK(v==7 && pg[0]==0.5F && pg[2]==4.5F);
        for(size_t i=0;i<4;++i)CHECK(pg[i]==-tg[i]);
        CHECK(pg[1]==0 && pg[3]==0);return 0;
    }
    UmiAiTrainingTape *tape=NULL,*other=NULL;UmiAiTrainingTensor *a=NULL,*b=NULL,*loss=NULL,*extra=NULL;
    UmiAiTrainingTapeMemory before,after;
    OK(UmiAiTrainingTapeCreate(&tape));OK(UmiAiTrainingTensorCreate(tape,2,2,p,4,1,&a));
    OK(UmiAiTrainingTensorCreate(tape,2,2,t,4,1,&b));
    OK(UmiAiTrainingTapeMemoryInspect(tape,&before));loss=a;
    if(!strcmp(mode,"binary-parity")||!strcmp(mode,"uniform-parity")) {
        unsigned char mask[4]={1,0,1,0};
        if(!strcmp(mode,"uniform-parity")) {
            for(size_t i=0;i<4;++i)w[i]=3;
            OK(UmiAiTrainingMeanSquaredError(tape,a,b,&extra));
        } else {
            w[2]=1;OK(UmiAiTrainingMaskedMeanSquaredError(tape,a,b,mask,4,&extra));
        }
        OK(UmiAiTrainingTensorValues(extra,&v,1));OK(UmiAiTrainingBackward(tape,extra));
        OK(UmiAiTrainingTensorGradients(a,pg,4));
        OK(UmiAiTrainingWeightedMeanSquaredError(tape,a,b,w,4,&loss));
        float x;OK(UmiAiTrainingTensorValues(loss,&x,1));OK(UmiAiTrainingBackward(tape,loss));
        OK(UmiAiTrainingTensorGradients(a,tg,4));CHECK(v==x);
        for(size_t i=0;i<4;++i)CHECK(pg[i]==tg[i]);
    } else if(!strcmp(mode,"ownership")) {
        float *owned=malloc(sizeof w);CHECK(owned);memcpy(owned,w,sizeof w);
        OK(UmiAiTrainingWeightedMeanSquaredError(tape,a,b,owned,4,&loss));
        memset(owned,0,sizeof w);free(owned);OK(UmiAiTrainingBackward(tape,loss));
        OK(UmiAiTrainingTensorGradients(a,pg,4));CHECK(pg[0]==0.5F&&pg[2]==4.5F);
        OK(UmiAiTrainingTapeMemoryInspect(tape,&after));CHECK(after.auxiliaryBytes-before.auxiliaryBytes==sizeof w);
        OK(UmiAiTrainingTapeRewind(tape,2));OK(UmiAiTrainingTapeMemoryInspect(tape,&after));
        CHECK(after.currentBytes==before.currentBytes&&after.auxiliaryBytes==before.auxiliaryBytes);
    } else if(!strcmp(mode,"budget")) {
        OK(UmiAiTrainingWeightedMeanSquaredError(tape,a,b,w,4,&loss));
        OK(UmiAiTrainingTapeMemoryInspect(tape,&after));size_t exact=after.currentBytes;
        OK(UmiAiTrainingTapeRewind(tape,2));OK(UmiAiTrainingTapeSetMemoryBudget(tape,exact-1));
        OK(UmiAiTrainingTapeMemoryInspect(tape,&before));loss=a;
        CHECK(UmiAiTrainingWeightedMeanSquaredError(tape,a,b,w,4,&loss)==UMI_STATUS_CAPACITY_EXCEEDED);
        OK(UmiAiTrainingTapeMemoryInspect(tape,&after));CHECK(loss==a&&!memcmp(&before,&after,sizeof before));
        OK(UmiAiTrainingTapeSetMemoryBudget(tape,exact));OK(UmiAiTrainingWeightedMeanSquaredError(tape,a,b,w,4,&loss));
    } else if(!strcmp(mode,"refusal")) {
        const float invalid[4]={-1,NAN,INFINITY,-INFINITY};
        for(size_t i=0;i<4;++i){w[3]=invalid[i];CHECK(UmiAiTrainingWeightedMeanSquaredError(tape,a,b,w,4,&loss)==UMI_STATUS_INVALID_ARGUMENT);}
        memset(w,0,sizeof w);CHECK(UmiAiTrainingWeightedMeanSquaredError(tape,a,b,w,4,&loss)==UMI_STATUS_INVALID_ARGUMENT);
        w[0]=1;const size_t counts[]={0,3,5,SIZE_MAX};
        for(size_t i=0;i<4;++i)CHECK(UmiAiTrainingWeightedMeanSquaredError(tape,a,b,w,counts[i],&loss)==UMI_STATUS_INVALID_ARGUMENT);
        OK(UmiAiTrainingTapeMemoryInspect(tape,&after));CHECK(loss==a&&!memcmp(&before,&after,sizeof before));
    } else if(!strcmp(mode,"arguments")) {
        CHECK(UmiAiTrainingWeightedMeanSquaredError(NULL,a,b,w,4,&loss)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingWeightedMeanSquaredError(tape,NULL,b,w,4,&loss)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingWeightedMeanSquaredError(tape,a,NULL,w,4,&loss)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingWeightedMeanSquaredError(tape,a,b,NULL,4,&loss)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingWeightedMeanSquaredError(tape,a,b,w,4,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        OK(UmiAiTrainingTapeCreate(&other));OK(UmiAiTrainingTensorCreate(other,2,2,t,4,1,&extra));
        CHECK(UmiAiTrainingWeightedMeanSquaredError(tape,a,extra,w,4,&loss)==UMI_STATUS_INVALID_ARGUMENT);
        OK(UmiAiTrainingTapeMemoryInspect(tape,&after));CHECK(loss==a&&!memcmp(&before,&after,sizeof before));
    } else if(!strcmp(mode,"shape")) {
        OK(UmiAiTrainingTensorCreate(tape,1,4,t,4,1,&extra));
        CHECK(UmiAiTrainingWeightedMeanSquaredError(tape,a,extra,w,4,&loss)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(loss==a);
    } else if(!strcmp(mode,"numeric-refusal")) {
        p[0]=FLT_MAX;OK(UmiAiTrainingTensorCreate(tape,2,2,p,4,1,&extra));
        OK(UmiAiTrainingTapeMemoryInspect(tape,&before));
        CHECK(UmiAiTrainingWeightedMeanSquaredError(tape,extra,b,w,4,&loss)==UMI_STATUS_INVALID_ARGUMENT);
        OK(UmiAiTrainingTapeMemoryInspect(tape,&after));CHECK(loss==a&&!memcmp(&before,&after,sizeof before));
    } else if(!strcmp(mode,"same-input")) {
        OK(UmiAiTrainingWeightedMeanSquaredError(tape,a,a,w,4,&loss));
        OK(UmiAiTrainingTensorValues(loss,&v,1));CHECK(v==0);OK(UmiAiTrainingBackward(tape,loss));
        OK(UmiAiTrainingTensorGradients(a,pg,4));for(size_t i=0;i<4;++i)CHECK(pg[i]==0);
    } else if(!strcmp(mode,"shared-gradient")) {
        OK(UmiAiTrainingWeightedMeanSquaredError(tape,a,b,w,4,&loss));
        OK(UmiAiTrainingAdd(tape,loss,loss,&extra));OK(UmiAiTrainingBackward(tape,extra));
        OK(UmiAiTrainingTensorGradients(a,pg,4));CHECK(pg[0]==1&&pg[2]==9);

    } else if(!strcmp(mode,"node-capacity")) {
        for(size_t i=2;i<UMI_AI_TRAINING_MAX_TENSORS;++i)
            OK(UmiAiTrainingAdd(tape,a,b,&extra));
        OK(UmiAiTrainingTapeMemoryInspect(tape,&before));
        CHECK(UmiAiTrainingWeightedMeanSquaredError(tape,a,b,w,4,&loss)==UMI_STATUS_CAPACITY_EXCEEDED);
        OK(UmiAiTrainingTapeMemoryInspect(tape,&after));CHECK(loss==a&&!memcmp(&before,&after,sizeof before));
    } else if(!strcmp(mode,"refused-after-backward")) {
        OK(UmiAiTrainingWeightedMeanSquaredError(tape,a,b,w,4,&extra));
        OK(UmiAiTrainingBackward(tape,extra));w[3]=-1;
        CHECK(UmiAiTrainingWeightedMeanSquaredError(tape,a,b,w,4,&loss)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(loss==a);
        OK(UmiAiTrainingSgdStep(tape,a,0.1F));OK(UmiAiTrainingTensorValues(a,pg,4));
        CHECK(fabsf(pg[0]-0.95F)<1e-6F&&fabsf(pg[2]-2.55F)<1e-6F);
    } else if(!strcmp(mode,"repeat")) {
        for(size_t i=0;i<100;++i) {
            OK(UmiAiTrainingWeightedMeanSquaredError(tape,a,b,w,4,&loss));OK(UmiAiTrainingBackward(tape,loss));
            OK(UmiAiTrainingTapeRewind(tape,2));OK(UmiAiTrainingTapeMemoryInspect(tape,&after));
            CHECK(after.currentBytes==before.currentBytes&&after.auxiliaryBytes==before.auxiliaryBytes);
        }
    } else { UmiAiTrainingTapeDestroy(tape);return 2; }
    UmiAiTrainingTapeDestroy(other);UmiAiTrainingTapeDestroy(tape);return 0;
}
