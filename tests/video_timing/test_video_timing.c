/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/video_timing/test_video_timing.c
 *
 * PURPOSE:
 *   Check timing semantics against a direct independent reference and transactional refusal fixtures.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/training/video_timing.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if(!(x)) { fprintf(stderr,"%d: %s\n",__LINE__,#x);return 1; } } while(0)

static int Golden(void)
{
    const uint64_t times[]={100U,130U,205U,210U};
    const uint64_t queries[]={100U,129U,130U,200U,205U,209U,210U,210U};
    const size_t expected[]={0U,0U,1U,1U,2U,2U,3U,3U};
    size_t out[10];for(size_t i=0;i<10U;++i)out[i]=999U;
    CHECK(UmiAiTrainingVideoSelectAtOrBefore(times,4U,queries,8U,out,10U)==UMI_STATUS_OK);
    CHECK(memcmp(out,expected,sizeof expected)==0 && out[8]==999U && out[9]==999U);
    return 0;
}
static int Extremes(void)
{
    const uint64_t times[]={0U,1U,UINT64_MAX-1U,UINT64_MAX};
    const uint64_t query[]={0U,1U,2U,UINT64_MAX-2U,UINT64_MAX-1U,UINT64_MAX};
    const size_t expected[]={0U,1U,1U,1U,2U,3U};
    size_t out[6];
    CHECK(UmiAiTrainingVideoSelectAtOrBefore(times,4U,query,6U,out,6U)==UMI_STATUS_OK);
    CHECK(memcmp(out,expected,sizeof expected)==0);
    return 0;
}
static int Singleton(void)
{
    const uint64_t times[]={42U};
    uint64_t queries[UMI_AI_TRAINING_VIDEO_MAX_FRAMES];
    size_t out[UMI_AI_TRAINING_VIDEO_MAX_FRAMES];
    for(size_t i=0U;i<UMI_AI_TRAINING_VIDEO_MAX_FRAMES;++i)queries[i]=42U;
    CHECK(UmiAiTrainingVideoSelectAtOrBefore(times,1U,queries,UMI_AI_TRAINING_VIDEO_MAX_FRAMES,out,UMI_AI_TRAINING_VIDEO_MAX_FRAMES)==UMI_STATUS_OK);
    for(size_t i=0U;i<UMI_AI_TRAINING_VIDEO_MAX_FRAMES;++i)CHECK(out[i]==0U);
    return 0;
}
static int Grid(void)
{
    /* Exhaust all ordered nonempty subsets of eight timestamps. The oracle
     * independently searches every source for each possible query. */
    for(unsigned mask=1U;mask<256U;++mask) {
        uint64_t times[8],queries[8];size_t n=0U,q=0U,out[8];
        for(unsigned i=0U;i<8U;++i)if((mask&(1U<<i))!=0U)times[n++]=(uint64_t)i;
        for(uint64_t t=times[0];t<=times[n-1U];++t)queries[q++]=t;
        CHECK(UmiAiTrainingVideoSelectAtOrBefore(times,n,queries,q,out,8U)==UMI_STATUS_OK);
        for(size_t j=0U;j<q;++j) {
            size_t expected=0U;
            for(size_t k=0U;k<n;++k)if(times[k]<=queries[j])expected=k;
            CHECK(out[j]==expected);
        }
    }
    return 0;
}
static int Maximum(void)
{
    uint64_t times[UMI_AI_TRAINING_VIDEO_MAX_TIMESTAMPS];
    uint64_t queries[UMI_AI_TRAINING_VIDEO_MAX_FRAMES];
    size_t out[UMI_AI_TRAINING_VIDEO_MAX_FRAMES+1U];
    for(size_t i=0U;i<UMI_AI_TRAINING_VIDEO_MAX_TIMESTAMPS;++i)times[i]=(uint64_t)i*3U;
    for(size_t i=0U;i<UMI_AI_TRAINING_VIDEO_MAX_FRAMES;++i)queries[i]=times[i*65U];
    out[UMI_AI_TRAINING_VIDEO_MAX_FRAMES]=77U;
    CHECK(UmiAiTrainingVideoSelectAtOrBefore(times,UMI_AI_TRAINING_VIDEO_MAX_TIMESTAMPS,queries,UMI_AI_TRAINING_VIDEO_MAX_FRAMES,out,UMI_AI_TRAINING_VIDEO_MAX_FRAMES+1U)==UMI_STATUS_OK);
    for(size_t i=0U;i<UMI_AI_TRAINING_VIDEO_MAX_FRAMES;++i)CHECK(out[i]==i*65U);
    CHECK(out[UMI_AI_TRAINING_VIDEO_MAX_FRAMES]==77U);
    return 0;
}
static int Refusal(const char *kind)
{
    uint64_t times[]={10U,20U,30U,40U},queries[]={10U,25U,40U};
    size_t out[4]={91U,92U,93U,94U},before[4];memcpy(before,out,sizeof out);
    const uint64_t *source=times,*query=queries;size_t *dest=out;
    size_t n=4U,q=3U,capacity=4U;UmiStatus wanted=UMI_STATUS_INVALID_ARGUMENT;
    if(strcmp(kind,"source-null")==0)source=NULL;
    else if(strcmp(kind,"query-null")==0)query=NULL;
    else if(strcmp(kind,"output-null")==0)dest=NULL;
    else if(strcmp(kind,"source-empty")==0)n=0U;
    else if(strcmp(kind,"query-empty")==0)q=0U;
    else if(strcmp(kind,"source-limit")==0){n=UMI_AI_TRAINING_VIDEO_MAX_TIMESTAMPS+1U;wanted=UMI_STATUS_CAPACITY_EXCEEDED;}
    else if(strcmp(kind,"query-limit")==0){q=UMI_AI_TRAINING_VIDEO_MAX_FRAMES+1U;wanted=UMI_STATUS_CAPACITY_EXCEEDED;}
    else if(strcmp(kind,"capacity")==0){capacity=2U;wanted=UMI_STATUS_CAPACITY_EXCEEDED;}
    else if(strcmp(kind,"duplicate-source")==0)times[3]=30U;
    else if(strcmp(kind,"descending-source")==0)times[3]=19U;
    else if(strcmp(kind,"unselected-bad-source")==0){times[3]=30U;queries[2]=25U;}
    else if(strcmp(kind,"descending-query")==0)queries[2]=24U;
    else if(strcmp(kind,"before-first")==0)queries[0]=9U;
    else if(strcmp(kind,"after-last")==0)queries[2]=41U;
    else return 2;
    CHECK(UmiAiTrainingVideoSelectAtOrBefore(source,n,query,q,dest,capacity)==wanted);
    CHECK(memcmp(out,before,sizeof out)==0);
    return 0;
}
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    if(strcmp(argv[1],"golden")==0)return Golden();
    if(strcmp(argv[1],"extremes")==0)return Extremes();
    if(strcmp(argv[1],"singleton")==0)return Singleton();
    if(strcmp(argv[1],"grid")==0)return Grid();
    if(strcmp(argv[1],"maximum")==0)return Maximum();
    return Refusal(argv[1]);
}
