/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/base/budget_allocator.c
 *
 * PURPOSE:
 *   Enforce bounded C allocation ownership without changing the canonical system allocator.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/base/budget_allocator.h"
#include <string.h>

typedef struct BudgetBlock { void *memory; size_t bytes; } BudgetBlock;
struct UmiBudgetAllocator {
    UmiAllocator parent;
    UmiBudgetAllocatorSnapshot state;
    BudgetBlock blocks[];
};

static void increment(uint64_t *counter)
{
    if(*counter!=UINT64_MAX) ++*counter;
}
static UmiStatus invalid(UmiBudgetAllocator *owner)
{
    if(owner!=NULL) increment(&owner->state.invalid_requests);
    return UMI_STATUS_INVALID_ARGUMENT;
}
static size_t locate(const UmiBudgetAllocator *owner,const void *memory)
{
    /* Compare exact starts before touching payload: no hidden header is read
     * through a foreign or interior pointer. The table has a fixed upper bound. */
    for(size_t i=0U;i<owner->state.block_limit;++i)
        if(owner->blocks[i].memory==memory) return i;
    return SIZE_MAX;
}
static UmiStatus admit(UmiBudgetAllocator *owner,size_t bytes)
{
    if(bytes>owner->state.byte_limit-owner->state.live_bytes) {
        increment(&owner->state.capacity_refusals);
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}
static void record_peak(UmiBudgetAllocator *owner,size_t bytes)
{
    if(bytes>owner->state.peak_bytes) owner->state.peak_bytes=bytes;
}
UmiStatus umi_budget_allocator_create(const UmiAllocator *parent,size_t byte_limit,
    size_t block_limit,UmiBudgetAllocator **out_owner)
{
    if(out_owner==NULL || byte_limit==0U || block_limit==0U ||
       block_limit>UMI_BUDGET_ALLOCATOR_MAX_BLOCKS ||
       (parent!=NULL && (parent->allocate==NULL || parent->release==NULL)))
        return UMI_STATUS_INVALID_ARGUMENT;
    if(*out_owner!=NULL) return UMI_STATUS_ALREADY_EXISTS;
    if(block_limit>(SIZE_MAX-sizeof(UmiBudgetAllocator))/sizeof(BudgetBlock))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    size_t metadata=sizeof(UmiBudgetAllocator)+block_limit*sizeof(BudgetBlock);
    UmiAllocator selected=parent==NULL?umi_allocator_system():*parent;
    UmiBudgetAllocator *owner=umi_alloc(&selected,metadata);
    if(owner==NULL) return UMI_STATUS_OUT_OF_MEMORY;
    memset(owner,0,metadata);
    owner->parent=selected;
    owner->state.byte_limit=byte_limit;
    owner->state.block_limit=block_limit;
    owner->state.metadata_bytes=metadata;
    *out_owner=owner;
    return UMI_STATUS_OK;
}
UmiStatus umi_budget_allocator_allocate(UmiBudgetAllocator *owner,size_t bytes,
    void **out_memory)
{
    if(owner==NULL || bytes==0U || out_memory==NULL) return invalid(owner);
    if(*out_memory!=NULL) return invalid(owner);
    if(owner->state.live_blocks==owner->state.block_limit) {
        increment(&owner->state.capacity_refusals);
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    UmiStatus status=admit(owner,bytes);
    if(status!=UMI_STATUS_OK) return status;
    size_t slot=locate(owner,NULL);
    if(slot==SIZE_MAX) return UMI_STATUS_INVALID_STATE;
    void *memory=umi_alloc(&owner->parent,bytes);
    if(memory==NULL) {
        increment(&owner->state.allocation_failures);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    /* Publication follows successful parent allocation. No failure path can
     * leave an untracked buffer or consume the caller's output slot. */
    owner->blocks[slot]=(BudgetBlock){memory,bytes};
    owner->state.live_bytes+=bytes;
    ++owner->state.live_blocks;
    record_peak(owner,owner->state.live_bytes);
    *out_memory=memory;
    return UMI_STATUS_OK;
}
UmiStatus umi_budget_allocator_resize(UmiBudgetAllocator *owner,void *memory,
    size_t bytes,void **out_memory)
{
    if(owner==NULL || bytes==0U || out_memory==NULL) return invalid(owner);
    if(memory==NULL) return umi_budget_allocator_allocate(owner,bytes,out_memory);
    if(*out_memory!=NULL && *out_memory!=memory) return invalid(owner);
    size_t slot=locate(owner,memory);
    if(slot==SIZE_MAX) return invalid(owner);
    size_t before=owner->blocks[slot].bytes;
    if(before==bytes) { *out_memory=memory; return UMI_STATUS_OK; }
    /* Copy-resize deliberately admits both buffers before allocating. A limit
     * that only counted the final size could allow an unseen transient peak. */
    UmiStatus status=admit(owner,bytes);
    if(status!=UMI_STATUS_OK) return status;
    void *replacement=umi_alloc(&owner->parent,bytes);
    if(replacement==NULL) {
        increment(&owner->state.allocation_failures);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    memcpy(replacement,memory,before<bytes?before:bytes);
    record_peak(owner,owner->state.live_bytes+bytes);
    umi_free(&owner->parent,memory);
    owner->blocks[slot]=(BudgetBlock){replacement,bytes};
    owner->state.live_bytes=owner->state.live_bytes-before+bytes;
    *out_memory=replacement;
    return UMI_STATUS_OK;
}
UmiStatus umi_budget_allocator_release(UmiBudgetAllocator *owner,void *memory)
{
    if(owner==NULL) return invalid(NULL);
    if(memory==NULL) return UMI_STATUS_OK;
    size_t slot=locate(owner,memory);
    if(slot==SIZE_MAX) return invalid(owner);
    size_t bytes=owner->blocks[slot].bytes;
    umi_free(&owner->parent,memory);
    owner->blocks[slot]=(BudgetBlock){NULL,0U};
    owner->state.live_bytes-=bytes;
    --owner->state.live_blocks;
    return UMI_STATUS_OK;
}
UmiStatus umi_budget_allocator_snapshot(const UmiBudgetAllocator *owner,
    UmiBudgetAllocatorSnapshot *out_snapshot)
{
    if(owner==NULL || out_snapshot==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_snapshot=owner->state;
    return UMI_STATUS_OK;
}
static void *adapter_allocate(size_t bytes,void *context)
{
    void *memory=NULL;
    (void)umi_budget_allocator_allocate(context,bytes,&memory);
    return memory;
}
static void *adapter_resize(void *memory,size_t bytes,void *context)
{
    void *replacement=NULL;
    if(umi_budget_allocator_resize(context,memory,bytes,&replacement)!=UMI_STATUS_OK)
        return NULL;
    return replacement;
}
static void adapter_release(void *memory,void *context)
{
    (void)umi_budget_allocator_release(context,memory);
}
UmiStatus umi_budget_allocator_adapter(UmiBudgetAllocator *owner,UmiAllocator *out_adapter)
{
    if(owner==NULL || out_adapter==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_adapter=(UmiAllocator){adapter_allocate,adapter_resize,adapter_release,owner};
    return UMI_STATUS_OK;
}
UmiStatus umi_budget_allocator_destroy(UmiBudgetAllocator **owner)
{
    if(owner==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if(*owner==NULL) return UMI_STATUS_OK;
    UmiBudgetAllocator *value=*owner;
    if(value->state.live_blocks!=0U) return UMI_STATUS_BUSY;
    UmiAllocator parent=value->parent;
    umi_free(&parent,value);
    *owner=NULL;
    return UMI_STATUS_OK;
}
