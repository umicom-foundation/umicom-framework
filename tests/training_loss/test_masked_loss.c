/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/training_loss/test_masked_loss.c
 *
 * PURPOSE:
 *   Check loss-mask semantics, numeric offset invariance, gradients and transactional tape ownership.
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
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"%d: %s\n",__LINE__,#x); return 1; } } while (0)
static int Near(double actual,double expected,double tolerance) { return fabs(actual-expected)<=tolerance; }
static int Evaluate(const float *values,const unsigned char *selected,float *result,float *gradients)
{
    UmiAiTrainingTape *tape=NULL;UmiAiTrainingTensor *logits=NULL,*loss=NULL;
    const size_t targets[3]={1U,0U,2U};
    CHECK(UmiAiTrainingTapeCreate(&tape)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorCreate(tape,3U,3U,values,9U,0,&logits)==UMI_STATUS_OK);
    UmiStatus status=selected?UmiAiTrainingMaskedCrossEntropy(tape,logits,targets,3U,selected,3U,&loss):
        UmiAiTrainingCrossEntropy(tape,logits,targets,3U,&loss);
    CHECK(status==UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorValues(loss,result,1U)==UMI_STATUS_OK);
    if(gradients) {
        CHECK(UmiAiTrainingBackward(tape,loss)==UMI_STATUS_OK);
        CHECK(UmiAiTrainingTensorGradients(logits,gradients,9U)==UMI_STATUS_OK);
    }
    UmiAiTrainingTapeDestroy(tape);return 0;
}
int main(int argc,char **argv)
{
    CHECK(argc==2);const char *mode=argv[1];
    float values[9]={1.0F,2.0F,-1.0F,3.0F,-2.0F,0.5F,-1.0F,0.0F,2.0F};
    unsigned char selected[3]={1U,0U,1U};
    float result,gradients[9];
    if(!strcmp(mode,"large-offset")) {
        const float offsets[5]={0.0F,1.0e20F,-1.0e20F,FLT_MAX,-FLT_MAX};
        for(size_t k=0U;k<5U;++k) {
            for(size_t i=0U;i<9U;++i)values[i]=offsets[k];
            CHECK(!Evaluate(values,NULL,&result,gradients));CHECK(Near(result,log(3.0),1e-6));
            CHECK(!Evaluate(values,selected,&result,gradients));CHECK(Near(result,log(3.0),1e-6));
        }
    } else if(!strcmp(mode,"reference")) {
        CHECK(!Evaluate(values,selected,&result,gradients));
        double expected=(log(exp(1.0)+exp(2.0)+exp(-1.0))-2.0+
            log(exp(-1.0)+1.0+exp(2.0))-2.0)/2.0;
        CHECK(Near(result,expected,1e-6));
        for(size_t i=3U;i<6U;++i)CHECK(gradients[i]==0.0F);
    } else if(!strcmp(mode,"uniform-gradient")) {
        memset(values,0,sizeof values);CHECK(!Evaluate(values,selected,&result,gradients));
        const float expected[9]={1.0F/6.0F,-1.0F/3.0F,1.0F/6.0F,0,0,0,1.0F/6.0F,1.0F/6.0F,-1.0F/3.0F};
        for(size_t i=0U;i<9U;++i)CHECK(Near(gradients[i],expected[i],1e-6));
    } else if(!strcmp(mode,"finite-difference")) {
        CHECK(!Evaluate(values,selected,&result,gradients));
        for(size_t i=0U;i<9U;++i) {
            const float original=values[i];float plus,minus;
            values[i]=original+0.001F;CHECK(!Evaluate(values,selected,&plus,NULL));
            values[i]=original-0.001F;CHECK(!Evaluate(values,selected,&minus,NULL));values[i]=original;
            CHECK(Near(gradients[i],((double)plus-(double)minus)/0.002,2e-4));
        }
    } else if(!strcmp(mode,"all-row-parity")) {
        float allResult,allGradients[9];memset(selected,1,sizeof selected);
        CHECK(!Evaluate(values,NULL,&result,gradients));CHECK(!Evaluate(values,selected,&allResult,allGradients));
        CHECK(result==allResult&&!memcmp(gradients,allGradients,sizeof gradients));
    } else {
        UmiAiTrainingTape *tape=NULL,*other=NULL;UmiAiTrainingTensor *logits=NULL,*loss=NULL,*foreign=NULL;
        size_t targets[3]={1U,0U,2U};UmiAiTrainingTapeMemory before,after;
        CHECK(UmiAiTrainingTapeCreate(&tape)==UMI_STATUS_OK);
        CHECK(UmiAiTrainingTensorCreate(tape,3U,3U,values,9U,1,&logits)==UMI_STATUS_OK);
        CHECK(UmiAiTrainingTapeMemoryInspect(tape,&before)==UMI_STATUS_OK);loss=logits;
        if(!strcmp(mode,"owned-selection")) {
            CHECK(UmiAiTrainingMaskedCrossEntropy(tape,logits,targets,3U,selected,3U,&loss)==UMI_STATUS_OK);
            memset(selected,0,sizeof selected);memset(targets,0,sizeof targets);
            CHECK(UmiAiTrainingBackward(tape,loss)==UMI_STATUS_OK);
            CHECK(UmiAiTrainingTensorGradients(logits,gradients,9U)==UMI_STATUS_OK);
            CHECK(gradients[1]<0.0F&&gradients[8]<0.0F);
            for(size_t i=3U;i<6U;++i)CHECK(gradients[i]==0.0F);
            CHECK(UmiAiTrainingTapeMemoryInspect(tape,&after)==UMI_STATUS_OK);
            CHECK(after.auxiliaryBytes-before.auxiliaryBytes==3U*sizeof(size_t)&&after.tensorCount==2U);
            CHECK(UmiAiTrainingTapeRewind(tape,1U)==UMI_STATUS_OK);
            CHECK(UmiAiTrainingTapeMemoryInspect(tape,&after)==UMI_STATUS_OK);
            CHECK(after.currentBytes==before.currentBytes&&after.auxiliaryBytes==before.auxiliaryBytes);
        } else if(!strcmp(mode,"budget")) {
            CHECK(UmiAiTrainingTapeSetMemoryBudget(tape,before.currentBytes)==UMI_STATUS_OK);
            CHECK(UmiAiTrainingTapeMemoryInspect(tape,&before)==UMI_STATUS_OK);
            CHECK(UmiAiTrainingMaskedCrossEntropy(tape,logits,targets,3U,selected,3U,&loss)==UMI_STATUS_CAPACITY_EXCEEDED);
            CHECK(loss==logits);
            CHECK(UmiAiTrainingTapeMemoryInspect(tape,&after)==UMI_STATUS_OK);
            CHECK(!memcmp(&before,&after,sizeof before));
        } else if(!strcmp(mode,"rejections")) {
            unsigned char none[3]={0,0,0},bad[3]={1,2,0};
            CHECK(UmiAiTrainingMaskedCrossEntropy(tape,logits,targets,3U,none,3U,&loss)==UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiAiTrainingMaskedCrossEntropy(tape,logits,targets,3U,bad,3U,&loss)==UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiAiTrainingMaskedCrossEntropy(tape,logits,targets,3U,selected,2U,&loss)==UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiAiTrainingMaskedCrossEntropy(tape,logits,targets,2U,selected,2U,&loss)==UMI_STATUS_INVALID_ARGUMENT);
            targets[1]=SIZE_MAX;
            CHECK(UmiAiTrainingMaskedCrossEntropy(tape,logits,targets,3U,selected,3U,&loss)==UMI_STATUS_INVALID_ARGUMENT);
            CHECK(loss==logits);
            CHECK(UmiAiTrainingTapeMemoryInspect(tape,&after)==UMI_STATUS_OK);CHECK(!memcmp(&before,&after,sizeof before));
        } else if(!strcmp(mode,"arguments")) {
            CHECK(UmiAiTrainingMaskedCrossEntropy(NULL,logits,targets,3U,selected,3U,&loss)==UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiAiTrainingMaskedCrossEntropy(tape,NULL,targets,3U,selected,3U,&loss)==UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiAiTrainingMaskedCrossEntropy(tape,logits,NULL,3U,selected,3U,&loss)==UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiAiTrainingMaskedCrossEntropy(tape,logits,targets,3U,NULL,3U,&loss)==UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiAiTrainingMaskedCrossEntropy(tape,logits,targets,3U,selected,3U,NULL)==UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiAiTrainingTapeCreate(&other)==UMI_STATUS_OK);
            CHECK(UmiAiTrainingTensorCreate(other,3U,3U,values,9U,0,&foreign)==UMI_STATUS_OK);
            CHECK(UmiAiTrainingMaskedCrossEntropy(tape,foreign,targets,3U,selected,3U,&loss)==UMI_STATUS_INVALID_ARGUMENT);
            CHECK(loss==logits);
            CHECK(UmiAiTrainingTapeMemoryInspect(tape,&after)==UMI_STATUS_OK);CHECK(!memcmp(&before,&after,sizeof before));
        } else if(!strcmp(mode,"shared-gradient")) {
            UmiAiTrainingTensor *first=NULL,*second=NULL,*combined=NULL;
            unsigned char firstMask[3]={1,0,0},secondMask[3]={0,1,0};
            CHECK(UmiAiTrainingMaskedCrossEntropy(tape,logits,targets,3U,firstMask,3U,&first)==UMI_STATUS_OK);
            CHECK(UmiAiTrainingMaskedCrossEntropy(tape,logits,targets,3U,secondMask,3U,&second)==UMI_STATUS_OK);
            CHECK(UmiAiTrainingAdd(tape,first,second,&combined)==UMI_STATUS_OK);
            CHECK(UmiAiTrainingBackward(tape,combined)==UMI_STATUS_OK);
            CHECK(UmiAiTrainingTensorGradients(logits,gradients,9U)==UMI_STATUS_OK);
            CHECK(gradients[1]<0.0F&&gradients[3]<0.0F);
            for(size_t i=6U;i<9U;++i)CHECK(gradients[i]==0.0F);
        } else { UmiAiTrainingTapeDestroy(tape);return 2; }
        UmiAiTrainingTapeDestroy(other);UmiAiTrainingTapeDestroy(tape);
    }
    return 0;
}
