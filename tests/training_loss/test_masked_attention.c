/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/training_loss/test_masked_attention.c
 *
 * PURPOSE:
 *   Verify padding exclusion, independent sequences, numerical derivatives and owned attention mask storage.
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if(!(x)){fprintf(stderr,"%d: %s\n",__LINE__,#x);return 1;} } while(0)
static const unsigned char selection[6]={1,0,1,0,1,1};

static void parameters(float p[3][24])
{
    for(size_t n=0;n<3U;++n)for(size_t i=0;i<24U;++i)
        p[n][i]=(float)((int)((n*19U+i*7U)%31U)-15)*0.031F;
}
static UmiStatus evaluate(const float p[3][24],const unsigned char *mask,
    int alias,float *value,float gradient[3][24],float output[24])
{
    UmiAiTrainingTape *tape=NULL;
    UmiAiTrainingTensor *q=NULL,*k=NULL,*v=NULL,*target=NULL,*attention=NULL,*loss=NULL;
    const float zeros[24]={0};
    UmiStatus s=UmiAiTrainingTapeCreate(&tape);
    if(s==UMI_STATUS_OK)s=UmiAiTrainingTensorCreate(tape,6U,4U,p[0],24U,1,&q);
    if(alias){k=q;v=q;}
    else{
        if(s==UMI_STATUS_OK)s=UmiAiTrainingTensorCreate(tape,6U,4U,p[1],24U,1,&k);
        if(s==UMI_STATUS_OK)s=UmiAiTrainingTensorCreate(tape,6U,4U,p[2],24U,1,&v);
    }
    if(s==UMI_STATUS_OK)s=UmiAiTrainingTensorCreate(tape,6U,4U,zeros,24U,0,&target);
    if(s==UMI_STATUS_OK)s=mask?
        UmiAiTrainingMaskedMultiHeadCausalAttention(tape,q,k,v,3U,2U,mask,6U,&attention):
        UmiAiTrainingMultiHeadCausalAttention(tape,q,k,v,3U,2U,&attention);
    if(s==UMI_STATUS_OK&&output)s=UmiAiTrainingTensorValues(attention,output,24U);
    if(s==UMI_STATUS_OK)s=UmiAiTrainingMeanSquaredError(tape,attention,target,&loss);
    if(s==UMI_STATUS_OK&&value)s=UmiAiTrainingTensorValues(loss,value,1U);
    if(s==UMI_STATUS_OK&&gradient){
        s=UmiAiTrainingBackward(tape,loss);
        if(s==UMI_STATUS_OK)s=UmiAiTrainingTensorGradients(q,gradient[0],24U);
        if(!alias&&s==UMI_STATUS_OK)s=UmiAiTrainingTensorGradients(k,gradient[1],24U);
        if(!alias&&s==UMI_STATUS_OK)s=UmiAiTrainingTensorGradients(v,gradient[2],24U);
    }
    UmiAiTrainingTapeDestroy(tape);
    return s;
}
static int reference(void)
{
    float p[3][24]={{0}},out[24],g[3][24];
    for(size_t i=0;i<24U;++i)p[2][i]=(float)i;
    CHECK(evaluate((const float(*)[24])p,selection,0,NULL,g,out)==UMI_STATUS_OK);
    for(size_t c=0;c<4U;++c){
        CHECK(out[c]==(float)c);
        CHECK(out[4U+c]==0);
        CHECK(out[8U+c]==(float)c+4.0F);
        CHECK(out[12U+c]==0);
        CHECK(out[16U+c]==(float)c+16.0F);
        CHECK(out[20U+c]==(float)c+18.0F);
    }
    for(size_t n=0;n<3U;++n)for(size_t r=0;r<6U;++r)if(!selection[r])
        for(size_t c=0;c<4U;++c)CHECK(g[n][r*4U+c]==0);
    return 0;
}
static int numerical(int alias)
{
    float p[3][24],g[3][24]={{0}},loss;parameters(p);
    CHECK(evaluate((const float(*)[24])p,selection,alias,&loss,g,NULL)==UMI_STATUS_OK);
    for(size_t n=0;n<(alias?1U:3U);++n)for(size_t i=0;i<24U;++i){
        const float before=p[n][i],step=0.001F;float plus,minus;
        p[n][i]=before+step;
        CHECK(evaluate((const float(*)[24])p,selection,alias,&plus,NULL,NULL)==UMI_STATUS_OK);
        p[n][i]=before-step;
        CHECK(evaluate((const float(*)[24])p,selection,alias,&minus,NULL,NULL)==UMI_STATUS_OK);
        p[n][i]=before;
        CHECK(fabsf((plus-minus)/(2.0F*step)-g[n][i])<0.0002F);
        if(!selection[i/4U])CHECK(g[n][i]==0);
    }
    return 0;
}
static int parity(void)
{
    const unsigned char all[6]={1,1,1,1,1,1};
    for(size_t trial=0;trial<17U;++trial){
        float p[3][24],a[24],b[24],ga[3][24],gb[3][24],la,lb;parameters(p);
        for(size_t n=0;n<3U;++n)for(size_t i=0;i<24U;++i)p[n][i]+=(float)trial*0.01F;
        CHECK(evaluate((const float(*)[24])p,NULL,0,&la,ga,a)==UMI_STATUS_OK);
        CHECK(evaluate((const float(*)[24])p,all,0,&lb,gb,b)==UMI_STATUS_OK);
        CHECK(la==lb&&!memcmp(a,b,sizeof a)&&!memcmp(ga,gb,sizeof ga));
    }
    return 0;
}
static int empty(void)
{
    float p[3][24],out[24],g[3][24],loss;parameters(p);
    const unsigned char none[6]={0};
    CHECK(evaluate((const float(*)[24])p,none,0,&loss,g,out)==UMI_STATUS_OK);
    CHECK(loss==0);
    for(size_t i=0;i<24U;++i){CHECK(out[i]==0);for(size_t n=0;n<3U;++n)CHECK(g[n][i]==0);}
    const unsigned char sequence[6]={0,0,0,1,1,1};
    CHECK(evaluate((const float(*)[24])p,sequence,0,&loss,g,out)==UMI_STATUS_OK);
    for(size_t i=0;i<12U;++i){CHECK(out[i]==0);for(size_t n=0;n<3U;++n)CHECK(g[n][i]==0);}
    return 0;
}
static int isolation(void)
{
    float p[3][24],out[24],changed[24];parameters(p);
    CHECK(evaluate((const float(*)[24])p,selection,0,NULL,NULL,out)==UMI_STATUS_OK);
    for(size_t n=0;n<3U;++n)for(size_t c=0;c<4U;++c)p[n][20U+c]+=3.0F;
    CHECK(evaluate((const float(*)[24])p,selection,0,NULL,NULL,changed)==UMI_STATUS_OK);
    for(size_t i=0;i<20U;++i)CHECK(out[i]==changed[i]);
    for(size_t n=0;n<3U;++n)for(size_t i=0;i<12U;++i)p[n][i]-=5.0F;
    float second[24];
    CHECK(evaluate((const float(*)[24])p,selection,0,NULL,NULL,second)==UMI_STATUS_OK);
    for(size_t i=12U;i<24U;++i)CHECK(second[i]==changed[i]);
    return 0;
}
static int extremes(void)
{
    float p[3][24],a[24],b[24],ga[3][24],gb[3][24],la,lb;parameters(p);
    CHECK(evaluate((const float(*)[24])p,selection,0,&la,ga,a)==UMI_STATUS_OK);
    for(size_t n=0;n<3U;++n)for(size_t r=0;r<6U;++r)if(!selection[r])
        for(size_t c=0;c<4U;++c)p[n][r*4U+c]=c%2U?FLT_MAX:-FLT_MAX;
    CHECK(evaluate((const float(*)[24])p,selection,0,&lb,gb,b)==UMI_STATUS_OK);
    CHECK(la==lb&&!memcmp(a,b,sizeof a)&&!memcmp(ga,gb,sizeof ga));
    return 0;
}
static int ownership(const char *mode)
{
    UmiAiTrainingTape *tape=NULL,*other=NULL;
    UmiAiTrainingTensor *input=NULL,*target=NULL,*out=NULL,*loss=NULL,*foreign=NULL;
    float data[24],ones[24];for(size_t i=0;i<24U;++i){data[i]=(float)i*0.01F;ones[i]=1;}
    CHECK(UmiAiTrainingTapeCreate(&tape)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorCreate(tape,6U,4U,data,24U,1,&input)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorCreate(tape,6U,4U,ones,24U,0,&target)==UMI_STATUS_OK);
    UmiAiTrainingTapeMemory before,after;
    CHECK(UmiAiTrainingTapeMemoryInspect(tape,&before)==UMI_STATUS_OK);out=input;
    if(!strcmp(mode,"ownership")){
        unsigned char *owned=malloc(6U);CHECK(owned);memcpy(owned,selection,6U);
        CHECK(UmiAiTrainingMaskedMultiHeadCausalAttention(tape,input,input,input,3U,2U,owned,6U,&out)==UMI_STATUS_OK);
        memset(owned,1,6U);free(owned);
        CHECK(UmiAiTrainingTapeMemoryInspect(tape,&after)==UMI_STATUS_OK);
        CHECK(after.auxiliaryBytes-before.auxiliaryBytes==6U+36U*sizeof(float));
        CHECK(UmiAiTrainingMeanSquaredError(tape,out,target,&loss)==UMI_STATUS_OK);
        CHECK(UmiAiTrainingBackward(tape,loss)==UMI_STATUS_OK);
        float gradient[24];CHECK(UmiAiTrainingTensorGradients(input,gradient,24U)==UMI_STATUS_OK);
        for(size_t i=0;i<24U;++i)if(!selection[i/4U])CHECK(gradient[i]==0);
        CHECK(UmiAiTrainingTapeRewind(tape,2U)==UMI_STATUS_OK);
        CHECK(UmiAiTrainingTapeMemoryInspect(tape,&after)==UMI_STATUS_OK);
        CHECK(after.currentBytes==before.currentBytes&&after.auxiliaryBytes==before.auxiliaryBytes);
    }else if(!strcmp(mode,"budget")){
        CHECK(UmiAiTrainingMaskedMultiHeadCausalAttention(tape,input,input,input,3U,2U,selection,6U,&out)==UMI_STATUS_OK);
        CHECK(UmiAiTrainingTapeMemoryInspect(tape,&after)==UMI_STATUS_OK);
        size_t budget=after.currentBytes;
        CHECK(UmiAiTrainingTapeRewind(tape,2U)==UMI_STATUS_OK);
        CHECK(UmiAiTrainingTapeSetMemoryBudget(tape,budget-1U)==UMI_STATUS_OK);
        CHECK(UmiAiTrainingTapeMemoryInspect(tape,&before)==UMI_STATUS_OK);out=input;
        CHECK(UmiAiTrainingMaskedMultiHeadCausalAttention(tape,input,input,input,3U,2U,selection,6U,&out)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(out==input&&UmiAiTrainingTapeMemoryInspect(tape,&after)==UMI_STATUS_OK);
        CHECK(!memcmp(&before,&after,sizeof before));
        CHECK(UmiAiTrainingTapeSetMemoryBudget(tape,budget)==UMI_STATUS_OK);
        CHECK(UmiAiTrainingMaskedMultiHeadCausalAttention(tape,input,input,input,3U,2U,selection,6U,&out)==UMI_STATUS_OK);
    }else if(!strcmp(mode,"rejections")){
        const size_t counts[]={0U,5U,7U,SIZE_MAX};
        for(size_t i=0;i<4U;++i)
            CHECK(UmiAiTrainingMaskedMultiHeadCausalAttention(tape,input,input,input,3U,2U,selection,counts[i],&out)==UMI_STATUS_INVALID_ARGUMENT);
        unsigned char bad[6]={1,0,1,0,1,2};
        CHECK(UmiAiTrainingMaskedMultiHeadCausalAttention(tape,input,input,input,3U,2U,bad,6U,&out)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingMaskedMultiHeadCausalAttention(tape,input,input,input,3U,2U,NULL,6U,&out)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingMaskedMultiHeadCausalAttention(NULL,input,input,input,3U,2U,selection,6U,&out)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingMaskedMultiHeadCausalAttention(tape,input,input,input,3U,2U,selection,6U,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingMaskedMultiHeadCausalAttention(tape,input,input,input,0U,2U,selection,6U,&out)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingMaskedMultiHeadCausalAttention(tape,input,input,input,3U,0U,selection,6U,&out)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingMaskedMultiHeadCausalAttention(tape,input,input,input,3U,3U,selection,6U,&out)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingMaskedMultiHeadCausalAttention(tape,input,input,input,4U,2U,selection,6U,&out)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingTapeCreate(&other)==UMI_STATUS_OK);
        CHECK(UmiAiTrainingTensorCreate(other,6U,4U,data,24U,1,&foreign)==UMI_STATUS_OK);
        CHECK(UmiAiTrainingMaskedMultiHeadCausalAttention(tape,foreign,input,input,3U,2U,selection,6U,&out)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingMaskedMultiHeadCausalAttention(tape,input,foreign,input,3U,2U,selection,6U,&out)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingMaskedMultiHeadCausalAttention(tape,input,input,foreign,3U,2U,selection,6U,&out)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(out==input&&UmiAiTrainingTapeMemoryInspect(tape,&after)==UMI_STATUS_OK);
        CHECK(!memcmp(&before,&after,sizeof before));
    }else if(!strcmp(mode,"shared-gradient")){
        unsigned char none[6]={0};UmiAiTrainingTensor *sum=NULL;
        CHECK(UmiAiTrainingMaskedMultiHeadCausalAttention(tape,input,input,input,3U,2U,none,6U,&out)==UMI_STATUS_OK);
        CHECK(UmiAiTrainingAdd(tape,input,out,&sum)==UMI_STATUS_OK);
        CHECK(UmiAiTrainingMeanSquaredError(tape,sum,target,&loss)==UMI_STATUS_OK);
        CHECK(UmiAiTrainingBackward(tape,loss)==UMI_STATUS_OK);
        float gradient[24];CHECK(UmiAiTrainingTensorGradients(input,gradient,24U)==UMI_STATUS_OK);
        for(size_t i=0;i<24U;++i)CHECK(fabsf(gradient[i]-2.0F*(data[i]-1.0F)/24.0F)<0.000001F);
    }else if(!strcmp(mode,"rollback")){
        for(size_t i=0;i<50U;++i){
            CHECK(UmiAiTrainingMaskedMultiHeadCausalAttention(tape,input,input,input,3U,2U,selection,6U,&out)==UMI_STATUS_OK);
            CHECK(UmiAiTrainingTapeRollback(tape,2U)==UMI_STATUS_OK);
            CHECK(UmiAiTrainingTapeMemoryInspect(tape,&after)==UMI_STATUS_OK);
            CHECK(after.currentBytes==before.currentBytes&&after.auxiliaryBytes==before.auxiliaryBytes);
        }
    }else{UmiAiTrainingTapeDestroy(other);UmiAiTrainingTapeDestroy(tape);return 2;}
    UmiAiTrainingTapeDestroy(other);UmiAiTrainingTapeDestroy(tape);return 0;
}
static int capacity(void)
{
    const size_t sizes[]={128U,129U,UMI_AI_TRAINING_MAX_ELEMENTS};
    for(size_t n=0;n<3U;++n){
        const size_t rows=sizes[n],sequence=n==2U?1U:rows;
        float *data=malloc(rows*sizeof *data),*values=malloc(rows*sizeof *values);
        unsigned char *mask=malloc(rows);CHECK(data&&values&&mask);
        for(size_t i=0;i<rows;++i){data[i]=1;mask[i]=1;}
        UmiAiTrainingTape *tape=NULL;UmiAiTrainingTensor *input=NULL,*out=NULL;
        CHECK(UmiAiTrainingTapeCreate(&tape)==UMI_STATUS_OK);
        CHECK(UmiAiTrainingTensorCreate(tape,rows,1U,data,rows,1,&input)==UMI_STATUS_OK);out=input;
        UmiAiTrainingTapeMemory before,after;
        CHECK(UmiAiTrainingTapeMemoryInspect(tape,&before)==UMI_STATUS_OK);
        UmiStatus s=UmiAiTrainingMaskedMultiHeadCausalAttention(tape,input,input,input,sequence,1U,mask,rows,&out);
        if(n==1U){
            CHECK(s==UMI_STATUS_CAPACITY_EXCEEDED&&out==input);
            CHECK(UmiAiTrainingTapeMemoryInspect(tape,&after)==UMI_STATUS_OK);
            CHECK(!memcmp(&before,&after,sizeof before));
        }else{
            CHECK(s==UMI_STATUS_OK);
            CHECK(UmiAiTrainingTensorValues(out,values,rows)==UMI_STATUS_OK);
            for(size_t i=0;i<rows;++i)CHECK(fabsf(values[i]-1.0F)<0.000001F);
            CHECK(UmiAiTrainingTapeMemoryInspect(tape,&after)==UMI_STATUS_OK);
            CHECK(after.auxiliaryBytes-before.auxiliaryBytes==rows+rows*sequence*sizeof(float));
        }
        UmiAiTrainingTapeDestroy(tape);free(mask);free(values);free(data);
    }
    UmiAiTrainingTape *tape=NULL;UmiAiTrainingTensor *input=NULL,*out=NULL;
    const float value=1;const unsigned char mask=1;
    CHECK(UmiAiTrainingTapeCreate(&tape)==UMI_STATUS_OK);
    for(size_t i=0;i<UMI_AI_TRAINING_MAX_TENSORS;++i)
        CHECK(UmiAiTrainingTensorCreate(tape,1U,1U,&value,1U,1,&input)==UMI_STATUS_OK);
    out=input;
    CHECK(UmiAiTrainingMaskedMultiHeadCausalAttention(tape,input,input,input,1U,1U,&mask,1U,&out)==UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(out==input);UmiAiTrainingTapeDestroy(tape);return 0;
}

int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    if(!strcmp(argv[1],"capacity"))return capacity();
    if(!strcmp(argv[1],"reference"))return reference();
    if(!strcmp(argv[1],"finite-difference"))return numerical(0);
    if(!strcmp(argv[1],"alias-gradient"))return numerical(1);
    if(!strcmp(argv[1],"all-selected"))return parity();
    if(!strcmp(argv[1],"all-excluded"))return empty();
    if(!strcmp(argv[1],"isolation"))return isolation();
    if(!strcmp(argv[1],"excluded-extremes"))return extremes();
    return ownership(argv[1]);
}
