/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_workflow/test_euler_step.c
 *
 * PURPOSE:
 *   Check Euler arithmetic, in-place updates and refusal without partial latent writes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai_workflow/euler_step.h"
#include <math.h>
#include <float.h>
#include <stdio.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"line %d\n",__LINE__); return 1; } } while(0)
int main(void)
{
    float x[]={4,-4,0,FLT_MAX}, d[]={2,0,8,FLT_MAX}, out[]={99,99,99,99,77};
    CHECK(UmiAiEulerDenoisedStep(x,d,4,2,1,out,5)==UMI_STATUS_OK);
    CHECK(out[0]==3 && out[1]==-2 && out[2]==4 && out[3]==FLT_MAX && out[4]==77);
    CHECK(UmiAiEulerDenoisedStep(x,d,4,2,0,x,4)==UMI_STATUS_OK);
    for(size_t i=0;i<4;++i) CHECK(x[i]==d[i]);
    for(size_t i=0;i<4;++i) out[i]=99;
    d[3]=NAN;
    CHECK(UmiAiEulerDenoisedStep(x,d,4,1,0,out,5)==UMI_STATUS_INVALID_ARGUMENT);
    for(size_t i=0;i<4;++i) CHECK(out[i]==99);
    d[3]=0;
    CHECK(UmiAiEulerDenoisedStep(x,d,4,0,0,out,5)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiEulerDenoisedStep(x,d,4,1,1,out,5)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiEulerDenoisedStep(x,d,4,1,-1,out,5)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiEulerDenoisedStep(x,d,4,INFINITY,0,out,5)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiEulerDenoisedStep(x,d,4,1,0,out,3)==UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(out[0]==99 && out[4]==77);
    return 0;
}
