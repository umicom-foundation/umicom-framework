/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/local_models/test_host_kv_plan.c
 *
 * PURPOSE:
 *   Verify host-aware dense-cache planning, uncertain observations, exact integer budgets and transactional refusal.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/local_models/memory_plan.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"line %d: %s\n",__LINE__,#x); return 1; } } while (0)

static UmiModelMemoryRequirements costs(void)
{
    return (UmiModelMemoryRequirements){400U,0U,100U,50U,
        UMI_MODEL_MEMORY_WEIGHTS|UMI_MODEL_MEMORY_WORKSPACE|UMI_MODEL_MEMORY_RUNTIME};
}
static UmiModelDenseKvShape shape(void)
{
    /* Independent per-token cost: 2 sequences * 3 layers *
     * (2 key heads * 4 dimensions * 2 bytes + matching value arrays) = 192. */
    return (UmiModelDenseKvShape){10U,2U,3U,2U,2U,4U,4U,2U,2U};
}
static int run(const char *mode,UmiDesktopSystemSnapshot *s)
{
    UmiModelMemoryRequirements fixed=costs(); UmiModelDenseKvShape kv=shape();
    UmiModelHostKvPlan out;
    s->memoryStatus=UMI_STATUS_OK; s->memory.totalBytes=4000U;
    s->memory.availableBytes=2000U;
    if(!strcmp(mode,"fit")) {
        CHECK(UmiModelDenseKvPlanHostMemory(&fixed,&kv,s,10U,100U,100U,&out)==UMI_STATUS_OK);
        CHECK(out.fixed_memory.total_declared_bytes==650U && out.fixed_memory.remaining_bytes==1350U);
        CHECK(out.cache_plan_available && out.cache.affordable_tokens==7U && !out.cache.request_fits);
        CHECK(out.cache.cache_bytes==1344U && out.cache.unused_budget_bytes==6U);
        CHECK(out.selected_memory.total_declared_bytes==1994U && out.selected_memory.remaining_bytes==6U);
        CHECK(out.selected_memory.verdict==UMI_MODEL_MEMORY_WITHIN_DECLARED_BUDGET);
    } else if(!strcmp(mode,"boundaries")) {
        const uint64_t available[]={649U,650U,841U,842U,2569U,2570U,4000U};
        const uint64_t tokens[]={0U,0U,0U,1U,9U,10U,10U};
        for(size_t i=0;i<sizeof available/sizeof available[0];++i) {
            s->memory.availableBytes=available[i];
            CHECK(UmiModelDenseKvPlanHostMemory(&fixed,&kv,s,0,100,100,&out)==UMI_STATUS_OK);
            CHECK(out.cache_plan_available==(i!=0U));
            if(i==0U)CHECK(out.fixed_memory.verdict==UMI_MODEL_MEMORY_INSUFFICIENT && out.fixed_memory.deficit_bytes==1U);
            else {
                CHECK(out.cache.affordable_tokens==tokens[i]);
                CHECK(out.cache.request_fits==(tokens[i]==10U));
                CHECK(out.selected_memory.total_declared_bytes==650U+tokens[i]*192U);
                CHECK(out.selected_memory.remaining_bytes==available[i]-650U-tokens[i]*192U);
            }
        }
    } else if(!strcmp(mode,"uncertain")) {
        for(unsigned i=0;i<5U;++i) {
            s->memoryStatus=UMI_STATUS_OK;s->fixture=0;
            const UmiDesktopSystemSnapshot *selected=s;uint64_t age=10U;
            UmiModelMemoryVerdict verdict=UMI_MODEL_MEMORY_UNAVAILABLE;
            if(i==0U)selected=NULL;
            if(i==1U)s->memoryStatus=UMI_STATUS_IO_ERROR;
            if(i==2U){s->fixture=-1;verdict=UMI_MODEL_MEMORY_FIXTURE;}
            if(i==3U){age=101U;verdict=UMI_MODEL_MEMORY_STALE;}
            if(i==4U){s->memory.availableBytes=649U;verdict=UMI_MODEL_MEMORY_INSUFFICIENT;}
            CHECK(UmiModelDenseKvPlanHostMemory(&fixed,&kv,selected,age,100,100,&out)==UMI_STATUS_OK);
            CHECK(!out.cache_plan_available && out.fixed_memory.verdict==verdict);
            CHECK(out.cache.requested_tokens==0U && out.cache.affordable_tokens==0U);
            CHECK(out.selected_memory.total_declared_bytes==0U);
        }
    } else if(!strcmp(mode,"incomplete")) {
        for(unsigned mask=0U;mask<16U;++mask) {
            if((mask&UMI_MODEL_MEMORY_CACHE)!=0U || mask==13U)continue;
            fixed=(UmiModelMemoryRequirements){0};fixed.known_components=mask;
            if(mask&UMI_MODEL_MEMORY_WEIGHTS)fixed.weights_bytes=400U;
            if(mask&UMI_MODEL_MEMORY_WORKSPACE)fixed.workspace_bytes=100U;
            if(mask&UMI_MODEL_MEMORY_RUNTIME)fixed.runtime_bytes=50U;
            CHECK(UmiModelDenseKvPlanHostMemory(&fixed,&kv,s,0,100,100,&out)==UMI_STATUS_OK);
            CHECK(out.fixed_memory.verdict==UMI_MODEL_MEMORY_INCOMPLETE && !out.cache_plan_available);
            CHECK(out.fixed_memory.known_components==(mask|UMI_MODEL_MEMORY_CACHE));
        }
    } else if(!strcmp(mode,"refusal")) {
        for(unsigned i=0U;i<13U;++i) {
            fixed=costs();kv=shape();s->memory.totalBytes=4000;s->memory.availableBytes=2000;
            const UmiModelMemoryRequirements *fp=&fixed;const UmiModelDenseKvShape *kp=&kv;
            uint64_t maximum_age=100;
            if(i==0U)fixed.cache_bytes=1U;
            if(i==1U)fixed.known_components|=UMI_MODEL_MEMORY_CACHE;
            if(i==2U)fixed.known_components|=16U;
            if(i==3U)fixed.weights_bytes=0U;
            if(i==4U)fixed.known_components&=~UMI_MODEL_MEMORY_WORKSPACE;
            if(i==5U)kv.token_capacity=0U;
            if(i==6U)kv.sequence_capacity=0U;
            if(i==7U)kv.key_element_bytes=3U;
            if(i==8U)s->memory.totalBytes=0U;
            if(i==9U)s->memory.availableBytes=4001U;
            if(i==10U)fp=NULL;
            if(i==11U)kp=NULL;
            if(i==12U)maximum_age=0U;
            unsigned char before[sizeof out];memset(&out,0x5a,sizeof out);memcpy(before,&out,sizeof out);
            CHECK(UmiModelDenseKvPlanHostMemory(fp,kp,s,0,maximum_age,100,&out)==UMI_STATUS_INVALID_ARGUMENT);
            CHECK(!memcmp(before,&out,sizeof out));
        }
        fixed=costs();kv=shape();
        CHECK(UmiModelDenseKvPlanHostMemory(&fixed,&kv,s,0,100,100,NULL)==UMI_STATUS_INVALID_ARGUMENT);
    } else if(!strcmp(mode,"overflow")) {
        for(unsigned i=0;i<3U;++i) {
            fixed=costs();kv=shape();uint64_t reserve=100U;
            if(i==0U)kv.layers=UINT64_MAX;
            if(i==1U)fixed.weights_bytes=UINT64_MAX;
            if(i==2U)reserve=UINT64_MAX;
            unsigned char before[sizeof out];memset(&out,0x55,sizeof out);memcpy(before,&out,sizeof out);
            CHECK(UmiModelDenseKvPlanHostMemory(&fixed,&kv,s,0,100,reserve,&out)==UMI_STATUS_CAPACITY_EXCEEDED);
            CHECK(!memcmp(before,&out,sizeof out));
        }
        kv=shape();kv.layers=UINT64_MAX;fixed=costs();
        CHECK(UmiModelDenseKvPlanHostMemory(&fixed,&kv,NULL,0,100,100,&out)==UMI_STATUS_CAPACITY_EXCEEDED);
    } else if(!strcmp(mode,"maximum")) {
        fixed=(UmiModelMemoryRequirements){1U,0U,0U,0U,13U};
        kv=(UmiModelDenseKvShape){UINT64_MAX,1U,1U,1U,1U,1U,1U,1U,1U};
        s->memory.totalBytes=s->memory.availableBytes=UINT64_MAX;
        CHECK(UmiModelDenseKvPlanHostMemory(&fixed,&kv,s,0,100,0,&out)==UMI_STATUS_OK);
        CHECK(out.cache_plan_available && out.cache.affordable_tokens==(UINT64_MAX-1U)/2U);
        CHECK(out.cache.cache_bytes==UINT64_MAX-1U && !out.cache.request_fits);
        CHECK(out.selected_memory.total_declared_bytes==UINT64_MAX && out.selected_memory.remaining_bytes==0U);
    } else if(!strcmp(mode,"grid")) {
        s->memory.totalBytes=8000U;
        for(uint64_t reserve=0;reserve<=300U;reserve+=100U)
            for(uint64_t available=0;available<8000U;available+=7U) {
                s->memory.availableBytes=available;
                CHECK(UmiModelDenseKvPlanHostMemory(&fixed,&kv,s,100,100,reserve,&out)==UMI_STATUS_OK);
                if(available<550U+reserve)CHECK(!out.cache_plan_available);
                else {
                    uint64_t tokens=(available-550U-reserve)/192U;if(tokens>10U)tokens=10U;
                    CHECK(out.cache_plan_available && out.cache.affordable_tokens==tokens);
                    CHECK(out.selected_memory.remaining_bytes==available-550U-reserve-tokens*192U);
                }
            }
    } else if(!strcmp(mode,"inputs")) {
        UmiDesktopSystemSnapshot *copy=malloc(sizeof *copy);CHECK(copy!=NULL);
        memcpy(copy,s,sizeof *copy);UmiModelMemoryRequirements fb=fixed;UmiModelDenseKvShape kb=kv;
        UmiStatus status=UmiModelDenseKvPlanHostMemory(&fixed,&kv,s,0,100,100,&out);
        int same=!memcmp(copy,s,sizeof *copy);free(copy);
        CHECK(status==UMI_STATUS_OK && same && !memcmp(&fb,&fixed,sizeof fixed) && !memcmp(&kb,&kv,sizeof kv));
    } else if(!strcmp(mode,"freshness")) {
        s->cpuStatus=UMI_STATUS_UNAVAILABLE;
        CHECK(UmiModelDenseKvPlanHostMemory(&fixed,&kv,s,100,100,100,&out)==UMI_STATUS_OK && out.cache_plan_available);
        CHECK(UmiModelDenseKvPlanHostMemory(&fixed,&kv,s,101,100,100,&out)==UMI_STATUS_OK && !out.cache_plan_available);
        CHECK(UmiModelDenseKvPlanHostMemory(&fixed,&kv,s,UINT64_MAX,UINT64_MAX,100,&out)==UMI_STATUS_OK && out.cache_plan_available);
    } else return 2;
    return 0;
}
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    UmiDesktopSystemSnapshot *snapshot=calloc(1U,sizeof *snapshot);
    if(snapshot==NULL)return 2;
    int result=run(argv[1],snapshot);free(snapshot);return result;
}
