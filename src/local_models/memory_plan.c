/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/local_models/memory_plan.c
 *
 * PURPOSE:
 *   Compute checked declared inference budgets while retaining missing costs and unreliable observations as explicit outcomes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/local_models/memory_plan.h"

static int memory_add(uint64_t left,uint64_t right,uint64_t *sum)
{
    if(right>UINT64_MAX-left) return 0;
    *sum=left+right;return 1;
}
static int memory_multiply(uint64_t left,uint64_t right,uint64_t *product)
{
    if(right!=0U && left>UINT64_MAX/right) return 0;
    *product=left*right;return 1;
}

UmiStatus UmiModelMemoryAssess(const UmiModelMemoryRequirements *requirements,
    const UmiDesktopSystemSnapshot *snapshot,uint64_t age,uint64_t maximum_age,
    uint64_t reserve,UmiModelMemoryAssessment *out)
{
    if(requirements==NULL || out==NULL || maximum_age==0U ||
        (requirements->known_components & ~UMI_MODEL_MEMORY_ALL)!=0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    const uint64_t values[]={requirements->weights_bytes,requirements->cache_bytes,
        requirements->workspace_bytes,requirements->runtime_bytes};
    UmiModelMemoryAssessment result={0};
    result.known_components=requirements->known_components;
    result.requirements_complete=requirements->known_components==UMI_MODEL_MEMORY_ALL;
    result.host_reserve_bytes=reserve;result.observation_age_millis=age;
    /* Unknown never means a free component. Keep its absence explicit instead
     * of folding a backend-specific heuristic into a reusable policy service. */
    for(unsigned int i=0U;i<4U;++i) {
        bool known=(requirements->known_components & (1U<<i))!=0U;
        if((!known && values[i]!=0U) || (known && i==0U && values[i]==0U))
            return UMI_STATUS_INVALID_ARGUMENT;
        if(!memory_add(result.known_payload_bytes,values[i],&result.known_payload_bytes))
            return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if(!memory_add(result.known_payload_bytes,reserve,&result.total_declared_bytes))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    result.memory_known=snapshot!=NULL && snapshot->memoryStatus==UMI_STATUS_OK;
    if(result.memory_known) {
        if(snapshot->memory.totalBytes==0U || snapshot->memory.availableBytes>snapshot->memory.totalBytes)
            return UMI_STATUS_INVALID_ARGUMENT;
        result.observed_total_bytes=snapshot->memory.totalBytes;
        result.observed_available_bytes=snapshot->memory.availableBytes;
        result.observation_current=snapshot->fixture==0 && age<=maximum_age;
    }
    if(!result.requirements_complete) result.verdict=UMI_MODEL_MEMORY_INCOMPLETE;
    else if(!result.memory_known) result.verdict=UMI_MODEL_MEMORY_UNAVAILABLE;
    else if(snapshot->fixture!=0) result.verdict=UMI_MODEL_MEMORY_FIXTURE;
    else if(!result.observation_current) result.verdict=UMI_MODEL_MEMORY_STALE;
    else if(result.total_declared_bytes>result.observed_available_bytes) {
        result.verdict=UMI_MODEL_MEMORY_INSUFFICIENT;
        result.deficit_bytes=result.total_declared_bytes-result.observed_available_bytes;
    } else {
        result.verdict=UMI_MODEL_MEMORY_WITHIN_DECLARED_BUDGET;
        result.remaining_bytes=result.observed_available_bytes-result.total_declared_bytes;
    }
    *out=result;return UMI_STATUS_OK;
}

static int memory_element_width(uint32_t bytes)
{
    return bytes==1U || bytes==2U || bytes==4U || bytes==8U;
}
UmiStatus UmiModelDenseKvCacheBytes(const UmiModelDenseKvShape *shape,
    UmiModelKvFootprint *out)
{
    if(shape==NULL || out==NULL || shape->token_capacity==0U || shape->sequence_capacity==0U ||
        shape->layers==0U || shape->key_heads==0U || shape->value_heads==0U ||
        shape->key_dimensions==0U || shape->value_dimensions==0U ||
        !memory_element_width(shape->key_element_bytes) || !memory_element_width(shape->value_element_bytes))
        return UMI_STATUS_INVALID_ARGUMENT;
    uint64_t slots=0U;
    UmiModelKvFootprint result={0};
    if(!memory_multiply(shape->token_capacity,shape->sequence_capacity,&slots) ||
        !memory_multiply(slots,shape->layers,&slots) ||
        !memory_multiply(slots,shape->key_heads,&result.key_bytes) ||
        !memory_multiply(result.key_bytes,shape->key_dimensions,&result.key_bytes) ||
        !memory_multiply(result.key_bytes,shape->key_element_bytes,&result.key_bytes) ||
        !memory_multiply(slots,shape->value_heads,&result.value_bytes) ||
        !memory_multiply(result.value_bytes,shape->value_dimensions,&result.value_bytes) ||
        !memory_multiply(result.value_bytes,shape->value_element_bytes,&result.value_bytes) ||
        !memory_add(result.key_bytes,result.value_bytes,&result.total_bytes))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    *out=result;return UMI_STATUS_OK;
}
