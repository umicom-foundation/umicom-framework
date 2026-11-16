/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/local_models/test_memory_plan.c
 *
 * PURPOSE:
 *   Verify declared memory budgets and dense-cache arithmetic using synthetic observations without querying hardware or loading models.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "../support/assertions.h"
#include "umicom/local_models/memory_plan.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

static void kv_refused(const UmiModelDenseKvShape *shape,UmiStatus expected)
{
    UmiModelKvFootprint output;unsigned char before[sizeof(output)];
    memset(&output,0xa5,sizeof(output));memcpy(before,&output,sizeof(output));
    assert(UmiModelDenseKvCacheBytes(shape,&output)==expected);
    assert(memcmp(before,&output,sizeof(output))==0);
}
static void cache_case(const char *mode)
{
    UmiModelDenseKvShape shape={2048U,2U,32U,8U,8U,128U,128U,2U,2U};
    UmiModelKvFootprint output;
    if(strcmp(mode,"kv-golden")==0) {
        assert(UmiModelDenseKvCacheBytes(&shape,&output)==UMI_STATUS_OK);
        assert(output.key_bytes==UINT64_C(268435456) && output.value_bytes==UINT64_C(268435456));
        assert(output.total_bytes==UINT64_C(536870912));
    } else if(strcmp(mode,"kv-asymmetric")==0) {
        shape=(UmiModelDenseKvShape){16U,3U,4U,2U,3U,7U,5U,2U,4U};
        assert(UmiModelDenseKvCacheBytes(&shape,&output)==UMI_STATUS_OK);
        assert(output.key_bytes==5376U && output.value_bytes==11520U && output.total_bytes==16896U);
    } else if(strcmp(mode,"kv-widths")==0) {
        const uint32_t widths[]={1U,2U,4U,8U};
        for(size_t key=0U;key<4U;++key) for(size_t value=0U;value<4U;++value) {
            shape=(UmiModelDenseKvShape){1U,1U,1U,1U,1U,1U,1U,widths[key],widths[value]};
            assert(UmiModelDenseKvCacheBytes(&shape,&output)==UMI_STATUS_OK);
            assert(output.key_bytes==widths[key] && output.value_bytes==widths[value]);
            assert(output.total_bytes==(uint64_t)widths[key]+widths[value]);
        }
    } else if(strcmp(mode,"kv-zero-geometry")==0) {
        for(unsigned i=0U;i<7U;++i) {
            UmiModelDenseKvShape bad=shape;
            uint64_t *fields[]={&bad.token_capacity,&bad.sequence_capacity,&bad.layers,&bad.key_heads,
                &bad.value_heads,&bad.key_dimensions,&bad.value_dimensions};
            *fields[i]=0U;kv_refused(&bad,UMI_STATUS_INVALID_ARGUMENT);
        }
    } else if(strcmp(mode,"kv-invalid-width")==0) {
        const uint32_t invalid[]={0U,3U,16U,UINT32_MAX};
        for(size_t i=0U;i<4U;++i) for(unsigned field=0U;field<2U;++field) {
            UmiModelDenseKvShape bad=shape;
            if(field==0U) bad.key_element_bytes=invalid[i];else bad.value_element_bytes=invalid[i];
            kv_refused(&bad,UMI_STATUS_INVALID_ARGUMENT);
        }
    } else if(strcmp(mode,"kv-product-overflow")==0) {
        for(unsigned i=0U;i<8U;++i) {
            UmiModelDenseKvShape bad={UINT64_MAX,1U,1U,1U,1U,1U,1U,1U,1U};
            if(i==0U) bad.sequence_capacity=2U;
            if(i==1U) bad.layers=2U;
            if(i==2U) bad.key_heads=2U;
            if(i==3U) bad.key_dimensions=2U;
            if(i==4U) bad.key_element_bytes=2U;
            if(i==5U) bad.value_heads=2U;
            if(i==6U) bad.value_dimensions=2U;
            if(i==7U) bad.value_element_bytes=2U;
            kv_refused(&bad,UMI_STATUS_CAPACITY_EXCEEDED);
        }
    } else if(strcmp(mode,"kv-sum-overflow")==0) {
        shape=(UmiModelDenseKvShape){UINT64_MAX/2U+1U,1U,1U,1U,1U,1U,1U,1U,1U};
        kv_refused(&shape,UMI_STATUS_CAPACITY_EXCEEDED);
    } else if(strcmp(mode,"kv-maximum")==0) {
        shape=(UmiModelDenseKvShape){UINT64_MAX/2U,1U,1U,1U,1U,1U,1U,1U,1U};
        assert(UmiModelDenseKvCacheBytes(&shape,&output)==UMI_STATUS_OK);
        assert(output.total_bytes==UINT64_MAX-1U);
    } else {
        assert(strcmp(mode,"kv-null")==0);
        kv_refused(NULL,UMI_STATUS_INVALID_ARGUMENT);
        assert(UmiModelDenseKvCacheBytes(&shape,NULL)==UMI_STATUS_INVALID_ARGUMENT);
    }
}
int main(int argc,char **argv)
{
    assert(argc==2);const char *mode=argv[1];
    if(strncmp(mode,"kv-",3U)==0) { cache_case(mode);return 0; }
    UmiDesktopSystemSnapshot *snapshot=calloc(1U,sizeof(*snapshot));
    UmiDesktopSystemSnapshot *snapshot_before=malloc(sizeof(*snapshot_before));
    assert(snapshot!=NULL && snapshot_before!=NULL);
    /* These are supplied numbers for a pure calculation, not a real capture. */
    snapshot->memoryStatus=UMI_STATUS_OK;snapshot->memory.totalBytes=2000U;
    snapshot->memory.availableBytes=1000U;
    UmiModelMemoryRequirements requirements={400U,200U,100U,50U,UMI_MODEL_MEMORY_ALL};
    UmiModelMemoryAssessment output;unsigned char before[sizeof(output)];
    memset(&output,0xa5,sizeof(output));memcpy(before,&output,sizeof(output));
    uint64_t age=10U,maximum_age=100U,reserve=100U;
    UmiModelMemoryVerdict verdict=UMI_MODEL_MEMORY_WITHIN_DECLARED_BUDGET;
    UmiStatus expected=UMI_STATUS_OK;
    const UmiDesktopSystemSnapshot *selected=snapshot;
    const UmiModelMemoryRequirements *selected_requirements=&requirements;
    int null_output=0;
    if(strcmp(mode,"fit")==0) { }
    else if(strcmp(mode,"boundary")==0) snapshot->memory.availableBytes=850U;
    else if(strcmp(mode,"insufficient")==0) { snapshot->memory.availableBytes=849U;verdict=UMI_MODEL_MEMORY_INSUFFICIENT; }
    else if(strcmp(mode,"reserve")==0) { reserve=251U;verdict=UMI_MODEL_MEMORY_INSUFFICIENT; }
    else if(strcmp(mode,"unknown-components")==0) {
        for(unsigned mask=0U;mask<UMI_MODEL_MEMORY_ALL;++mask) {
            UmiModelMemoryRequirements partial={0};partial.known_components=mask;
            if(mask&UMI_MODEL_MEMORY_WEIGHTS) partial.weights_bytes=1U;
            assert(UmiModelMemoryAssess(&partial,snapshot,age,maximum_age,reserve,&output)==UMI_STATUS_OK);
            assert(output.verdict==UMI_MODEL_MEMORY_INCOMPLETE && !output.requirements_complete);
            assert(output.memory_known && output.observation_current);
            assert(output.remaining_bytes==0U && output.deficit_bytes==0U);
        }
        memset(&output,0xa5,sizeof(output));memcpy(before,&output,sizeof(output));
        requirements=(UmiModelMemoryRequirements){0};selected=NULL;verdict=UMI_MODEL_MEMORY_INCOMPLETE;
    } else if(strcmp(mode,"unknown-cost")==0) { requirements.known_components=0U;expected=UMI_STATUS_INVALID_ARGUMENT; }
    else if(strcmp(mode,"unknown-mask")==0) { requirements.known_components|=16U;expected=UMI_STATUS_INVALID_ARGUMENT; }
    else if(strcmp(mode,"zero-weights")==0) { requirements.weights_bytes=0U;expected=UMI_STATUS_INVALID_ARGUMENT; }
    else if(strcmp(mode,"null-snapshot")==0) { selected=NULL;verdict=UMI_MODEL_MEMORY_UNAVAILABLE; }
    else if(strcmp(mode,"memory-unavailable")==0) {
        snapshot->memoryStatus=UMI_STATUS_IO_ERROR;snapshot->memory.availableBytes=UINT64_MAX;
        verdict=UMI_MODEL_MEMORY_UNAVAILABLE;
    } else if(strcmp(mode,"partial-capture")==0) snapshot->cpuStatus=UMI_STATUS_UNAVAILABLE;
    else if(strcmp(mode,"fixture")==0) { snapshot->fixture=-1;verdict=UMI_MODEL_MEMORY_FIXTURE; }
    else if(strcmp(mode,"stale")==0) { age=101U;verdict=UMI_MODEL_MEMORY_STALE; }
    else if(strcmp(mode,"age-boundary")==0) age=maximum_age;
    else if(strcmp(mode,"age-maximum")==0) age=maximum_age=UINT64_MAX;
    else if(strcmp(mode,"zero-age-policy")==0) { maximum_age=0U;expected=UMI_STATUS_INVALID_ARGUMENT; }
    else if(strcmp(mode,"bad-total")==0) { snapshot->memory.totalBytes=0U;expected=UMI_STATUS_INVALID_ARGUMENT; }
    else if(strcmp(mode,"bad-available")==0) { snapshot->memory.availableBytes=2001U;expected=UMI_STATUS_INVALID_ARGUMENT; }
    else if(strcmp(mode,"sum-overflow")==0) { requirements.weights_bytes=UINT64_MAX;expected=UMI_STATUS_CAPACITY_EXCEEDED; }
    else if(strcmp(mode,"reserve-overflow")==0) { reserve=UINT64_MAX;expected=UMI_STATUS_CAPACITY_EXCEEDED; }
    else if(strcmp(mode,"maximum")==0) {
        requirements=(UmiModelMemoryRequirements){UINT64_MAX,0U,0U,0U,UMI_MODEL_MEMORY_ALL};
        reserve=0U;snapshot->memory.totalBytes=UINT64_MAX;snapshot->memory.availableBytes=UINT64_MAX;
    } else if(strcmp(mode,"null-requirements")==0) { selected_requirements=NULL;expected=UMI_STATUS_INVALID_ARGUMENT; }
    else if(strcmp(mode,"null-assessment")==0) { null_output=1;expected=UMI_STATUS_INVALID_ARGUMENT; }
    else assert(!"unknown memory case");
    memcpy(snapshot_before,snapshot,sizeof(*snapshot));
    unsigned char requirements_before[sizeof(requirements)];memcpy(requirements_before,&requirements,sizeof(requirements));
    assert(UmiModelMemoryAssess(selected_requirements,selected,age,maximum_age,reserve,null_output?NULL:&output)==expected);
    if(expected!=UMI_STATUS_OK) assert(memcmp(before,&output,sizeof(output))==0);
    else {
        assert(output.verdict==verdict && output.host_reserve_bytes==reserve && output.observation_age_millis==age);
        assert(output.known_components==requirements.known_components);
        assert(output.requirements_complete==(requirements.known_components==UMI_MODEL_MEMORY_ALL));
        if(verdict==UMI_MODEL_MEMORY_WITHIN_DECLARED_BUDGET || verdict==UMI_MODEL_MEMORY_INSUFFICIENT) {
            assert(output.memory_known && output.observation_current);
            if(strcmp(mode,"maximum")==0) assert(output.known_payload_bytes==UINT64_MAX && output.total_declared_bytes==UINT64_MAX);
            else assert(output.known_payload_bytes==750U && output.total_declared_bytes==750U+reserve);
        }
        if(verdict==UMI_MODEL_MEMORY_WITHIN_DECLARED_BUDGET) {
            assert(output.remaining_bytes==snapshot->memory.availableBytes-output.total_declared_bytes);
            assert(output.deficit_bytes==0U);
        } else if(verdict==UMI_MODEL_MEMORY_INSUFFICIENT) assert(output.deficit_bytes==1U && output.remaining_bytes==0U);
        else assert(output.remaining_bytes==0U && output.deficit_bytes==0U);
        if(verdict==UMI_MODEL_MEMORY_UNAVAILABLE) assert(!output.memory_known && !output.observation_current && output.observed_available_bytes==0U);
        if(verdict==UMI_MODEL_MEMORY_FIXTURE || verdict==UMI_MODEL_MEMORY_STALE) assert(output.memory_known && !output.observation_current);
    }
    assert(memcmp(snapshot_before,snapshot,sizeof(*snapshot))==0);
    assert(memcmp(requirements_before,&requirements,sizeof(requirements))==0);
    free(snapshot_before);free(snapshot);return 0;
}
