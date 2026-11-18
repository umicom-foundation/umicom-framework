/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/denoising/test_normal_noise.c
 *
 * PURPOSE:
 *   Check normal-sample statistics, reproducibility limits and atomic error paths.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/training/initialisation.h"
#include "umicom/ai/training/tape.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"%d: %s\n",__LINE__,#x); return 1; } } while (0)
int main(int argc,char **argv)
{
    CHECK(argc==2);const char *mode=argv[1];
    UmiAiTrainingRandomState state={42U},original=state;
    float values[64],before[64];memset(values,0x5a,sizeof(values));memcpy(before,values,sizeof(values));
    if(strcmp(mode,"repeat")==0) {
        float other[64];UmiAiTrainingRandomState repeat=state;
        CHECK(UmiAiTrainingRandomFillNormal(&state,0.0F,1.0F,values,64U)==UMI_STATUS_OK);
        CHECK(UmiAiTrainingRandomFillNormal(&repeat,0.0F,1.0F,other,64U)==UMI_STATUS_OK);
        CHECK(memcmp(values,other,sizeof(values))==0&&state.state==repeat.state&&state.state!=original.state);
        for(size_t i=0U;i<64U;++i)CHECK(isfinite(values[i]));
    } else if(strcmp(mode,"moments")==0) {
        const size_t count=16384U;float *samples=malloc(count*sizeof(*samples));CHECK(samples!=NULL);
        CHECK(UmiAiTrainingRandomFillNormal(&state,2.0F,3.0F,samples,count)==UMI_STATUS_OK);
        double sum=0.0,squares=0.0;
        for(size_t i=0U;i<count;++i){CHECK(isfinite(samples[i]));sum+=samples[i];squares+=(double)samples[i]*samples[i];}
        double mean=sum/(double)count,variance=squares/(double)count-mean*mean;
        printf("seed42 mean %.6f variance %.6f\n",mean,variance);
        CHECK(fabs(mean-2.0)<0.12&&fabs(variance-9.0)<0.7);free(samples);
    } else if(strcmp(mode,"zero-deviation")==0) {
        CHECK(UmiAiTrainingRandomFillNormal(&state,7.25F,0.0F,values,5U)==UMI_STATUS_OK);
        CHECK(state.state==original.state);
        for(size_t i=0;i<5U;++i)CHECK(values[i]==7.25F);
        CHECK(memcmp(values+5U,before+5U,sizeof(values)-5U*sizeof(float))==0);
    } else if(strcmp(mode,"odd-pair")==0) {
        UmiAiTrainingRandomState one=state,two=state,three=state,four=state;
        float a[1],b[2],c[3],d[4];
        CHECK(UmiAiTrainingRandomFillNormal(&one,0.0F,1.0F,a,1U)==UMI_STATUS_OK);
        CHECK(UmiAiTrainingRandomFillNormal(&two,0.0F,1.0F,b,2U)==UMI_STATUS_OK);
        CHECK(UmiAiTrainingRandomFillNormal(&three,0.0F,1.0F,c,3U)==UMI_STATUS_OK);
        CHECK(UmiAiTrainingRandomFillNormal(&four,0.0F,1.0F,d,4U)==UMI_STATUS_OK);
        CHECK(a[0]==b[0]&&one.state==two.state&&three.state==four.state);
        CHECK(memcmp(c,d,sizeof(c))==0&&two.state!=four.state);
    } else if(strcmp(mode,"overflow")==0) {
        CHECK(UmiAiTrainingRandomFillNormal(&state,FLT_MAX,FLT_MAX,values,64U)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(state.state==original.state&&memcmp(values,before,sizeof(values))==0);
    } else if(strcmp(mode,"nonfinite")==0) {
        const float bad[]={NAN,INFINITY,-INFINITY};
        for(size_t i=0;i<3U;++i){
            CHECK(UmiAiTrainingRandomFillNormal(&state,bad[i],1.0F,values,64U)==UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiAiTrainingRandomFillNormal(&state,0.0F,bad[i],values,64U)==UMI_STATUS_INVALID_ARGUMENT);
        }
        CHECK(UmiAiTrainingRandomFillNormal(&state,0.0F,-1.0F,values,64U)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(state.state==original.state&&memcmp(values,before,sizeof(values))==0);
    } else if(strcmp(mode,"empty")==0) {
        CHECK(UmiAiTrainingRandomFillNormal(&state,0.0F,1.0F,NULL,0U)==UMI_STATUS_OK);
        CHECK(state.state==original.state);
    } else if(strcmp(mode,"arguments")==0) {
        CHECK(UmiAiTrainingRandomFillNormal(NULL,0.0F,1.0F,values,64U)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingRandomFillNormal(&state,0.0F,1.0F,NULL,1U)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingRandomFillNormal(&state,0.0F,1.0F,values,(size_t)UMI_AI_TRAINING_MAX_ELEMENTS+1U)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(state.state==original.state&&memcmp(values,before,sizeof(values))==0);
    } else return 2;
    return 0;
}
