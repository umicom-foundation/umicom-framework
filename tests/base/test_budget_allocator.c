/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/base/test_budget_allocator.c
 *
 * PURPOSE:
 *   Exercise real bounded allocation ownership with an independent failure-injecting parent and exact byte expectations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/base/budget_allocator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(condition) do { if(!(condition)) { fprintf(stderr,"line %d: %s\n",__LINE__,#condition); return 1; } } while(0)
#define MATCH(name) (strcmp(mode,(name))==0)
#define PARENT_SLOTS (UMI_BUDGET_ALLOCATOR_MAX_BLOCKS+4U)
typedef struct ParentRecord { void *pointer; size_t bytes; } ParentRecord;
typedef struct Parent {
    size_t attempts, releases, live, payload, peak, fail_at;
    int invalid_free;
    ParentRecord records[PARENT_SLOTS];
} Parent;
static void *parent_allocate(size_t bytes,void *context)
{
    Parent *parent=context;
    ++parent->attempts;
    if(parent->attempts==parent->fail_at) return NULL;
    void *pointer=malloc(bytes);
    if(pointer==NULL) return NULL;
    for(size_t i=0U;i<PARENT_SLOTS;++i) if(parent->records[i].pointer==NULL) {
        parent->records[i]=(ParentRecord){pointer,bytes};
        ++parent->live;parent->payload+=bytes;
        if(parent->payload>parent->peak) parent->peak=parent->payload;
        return pointer;
    }
    free(pointer);
    return NULL;
}
static void parent_release(void *pointer,void *context)
{
    Parent *parent=context;
    for(size_t i=0U;i<PARENT_SLOTS;++i) if(parent->records[i].pointer==pointer) {
        parent->payload-=parent->records[i].bytes;
        parent->records[i]=(ParentRecord){NULL,0U};
        --parent->live;++parent->releases;
        free(pointer);
        return;
    }
    parent->invalid_free=1;
}
static int run(const char *mode,Parent *parent)
{
    UmiAllocator allocator={parent_allocate,NULL,parent_release,parent};
    UmiBudgetAllocator *owner=NULL;
    UmiBudgetAllocatorSnapshot state={0},before={0};
    void *memory=NULL,*other=NULL;
    unsigned char foreign=0x4a;
    size_t limit=MATCH("byte-limit")?8U:MATCH("resize-transient-limit")?15U:64U;
    size_t blocks=MATCH("block-limit")?1U:3U;
    if(MATCH("zero-limit") || MATCH("zero-blocks") || MATCH("too-many-blocks") ||
       MATCH("missing-allocate") || MATCH("missing-release") || MATCH("create-failure")) {
        if(MATCH("missing-allocate")) allocator.allocate=NULL;
        if(MATCH("missing-release")) allocator.release=NULL;
        if(MATCH("create-failure")) parent->fail_at=1U;
        UmiStatus expected=MATCH("create-failure")?UMI_STATUS_OUT_OF_MEMORY:UMI_STATUS_INVALID_ARGUMENT;
        CHECK(umi_budget_allocator_create(&allocator,MATCH("zero-limit")?0U:64U,
            MATCH("zero-blocks")?0U:MATCH("too-many-blocks")?UMI_BUDGET_ALLOCATOR_MAX_BLOCKS+1U:3U,&owner)==expected);
        CHECK(owner==NULL && parent->live==0U);
        CHECK(parent->attempts==(MATCH("create-failure")?1U:0U));
        return 0;
    }
    if(MATCH("maximum-blocks")) {limit=UMI_BUDGET_ALLOCATOR_MAX_BLOCKS;blocks=UMI_BUDGET_ALLOCATOR_MAX_BLOCKS;}
    if(MATCH("overflow-limit")) limit=SIZE_MAX;
    CHECK(umi_budget_allocator_create(&allocator,limit,blocks,&owner)==UMI_STATUS_OK);
    CHECK(parent->live==1U);
    CHECK(umi_budget_allocator_snapshot(owner,&state)==UMI_STATUS_OK);
    CHECK(state.metadata_bytes==parent->payload && state.live_bytes==0U && state.live_blocks==0U);
    if(MATCH("null-owner")) {
        CHECK(umi_budget_allocator_create(&allocator,64U,3U,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_budget_allocator_allocate(NULL,1U,&memory)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_budget_allocator_resize(NULL,NULL,1U,&memory)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_budget_allocator_release(NULL,&foreign)==UMI_STATUS_INVALID_ARGUMENT);
    } else if(MATCH("occupied-output")) {
        UmiBudgetAllocator *saved=owner;
        CHECK(umi_budget_allocator_create(&allocator,64U,3U,&owner)==UMI_STATUS_ALREADY_EXISTS && owner==saved);
        other=&foreign;
        CHECK(umi_budget_allocator_allocate(owner,8U,&other)==UMI_STATUS_INVALID_ARGUMENT && other==&foreign);
        other=NULL;
    } else if(MATCH("null-output")) {
        CHECK(umi_budget_allocator_allocate(owner,8U,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_budget_allocator_resize(owner,NULL,8U,NULL)==UMI_STATUS_INVALID_ARGUMENT);
    } else if(MATCH("zero-bytes")) {
        CHECK(umi_budget_allocator_allocate(owner,0U,&memory)==UMI_STATUS_INVALID_ARGUMENT && memory==NULL);
    } else if(MATCH("allocation-failure")) {
        parent->fail_at=2U;
        CHECK(umi_budget_allocator_allocate(owner,8U,&memory)==UMI_STATUS_OUT_OF_MEMORY && memory==NULL);
        CHECK(umi_budget_allocator_snapshot(owner,&state)==UMI_STATUS_OK);
        CHECK(state.live_bytes==0U && state.live_blocks==0U && state.allocation_failures==1U);
    } else if(MATCH("null-snapshot")) {
        memset(&state,0x5a,sizeof(state));memcpy(&before,&state,sizeof(state));
        CHECK(umi_budget_allocator_snapshot(NULL,&state)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(&state,&before,sizeof(state))==0);
        CHECK(umi_budget_allocator_snapshot(owner,NULL)==UMI_STATUS_INVALID_ARGUMENT);
    } else if(MATCH("null-adapter")) {
        UmiAllocator adapter={0};
        CHECK(umi_budget_allocator_adapter(NULL,&adapter)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(adapter.allocate==NULL && adapter.reallocate==NULL && adapter.release==NULL && adapter.user_data==NULL);
        CHECK(umi_budget_allocator_adapter(owner,NULL)==UMI_STATUS_INVALID_ARGUMENT);
    } else if(MATCH("empty-destroy")) {
        CHECK(umi_budget_allocator_destroy(&owner)==UMI_STATUS_OK && owner==NULL);
        CHECK(umi_budget_allocator_destroy(&owner)==UMI_STATUS_OK);
    } else if(MATCH("null-destroy")) {
        CHECK(umi_budget_allocator_destroy(NULL)==UMI_STATUS_INVALID_ARGUMENT);
    } else if(MATCH("null-release")) {
        CHECK(umi_budget_allocator_release(owner,NULL)==UMI_STATUS_OK && parent->releases==0U);
    } else if(MATCH("release-foreign")) {
        CHECK(umi_budget_allocator_release(owner,&foreign)==UMI_STATUS_INVALID_ARGUMENT && parent->releases==0U);
    } else if(MATCH("resize-foreign")) {
        other=&foreign;
        CHECK(umi_budget_allocator_resize(owner,&foreign,4U,&other)==UMI_STATUS_INVALID_ARGUMENT && other==&foreign);
        other=NULL;
    } else if(MATCH("maximum-blocks")) {
        void **all=calloc(UMI_BUDGET_ALLOCATOR_MAX_BLOCKS,sizeof(*all));CHECK(all!=NULL);
        for(size_t i=0U;i<UMI_BUDGET_ALLOCATOR_MAX_BLOCKS;++i)
            CHECK(umi_budget_allocator_allocate(owner,1U,&all[i])==UMI_STATUS_OK);
        size_t attempts=parent->attempts;
        CHECK(umi_budget_allocator_allocate(owner,1U,&memory)==UMI_STATUS_CAPACITY_EXCEEDED && memory==NULL);
        CHECK(parent->attempts==attempts);
        for(size_t i=0U;i<UMI_BUDGET_ALLOCATOR_MAX_BLOCKS;++i)
            CHECK(umi_budget_allocator_release(owner,all[i])==UMI_STATUS_OK);
        free(all);
    } else if(MATCH("repeated")) {
        for(size_t i=0U;i<200U;++i) {
            CHECK(umi_budget_allocator_allocate(owner,16U,&memory)==UMI_STATUS_OK);
            CHECK(umi_budget_allocator_release(owner,memory)==UMI_STATUS_OK);memory=NULL;
        }
        CHECK(umi_budget_allocator_snapshot(owner,&state)==UMI_STATUS_OK && state.peak_bytes==16U && state.live_bytes==0U);
    } else if(MATCH("adapter") || MATCH("adapter-failure") || MATCH("adapter-zero") || MATCH("adapter-invalid-free")) {
        UmiAllocator adapter;
        CHECK(umi_budget_allocator_adapter(owner,&adapter)==UMI_STATUS_OK);
        memory=umi_alloc(&adapter,8U);CHECK(memory!=NULL);memset(memory,0x3c,8U);
        if(MATCH("adapter-failure")) {
            parent->fail_at=parent->attempts+1U;
            CHECK(umi_realloc(&adapter,memory,16U)==NULL);
            CHECK(((unsigned char *)memory)[0]==0x3c);
        } else if(MATCH("adapter-zero")) {
            CHECK(umi_realloc(&adapter,memory,0U)==NULL);
            CHECK(((unsigned char *)memory)[0]==0x3c);
        } else if(MATCH("adapter-invalid-free")) {
            umi_free(&adapter,&foreign);
            CHECK(umi_budget_allocator_snapshot(owner,&state)==UMI_STATUS_OK && state.invalid_requests==1U);
        } else {
            other=umi_realloc(&adapter,memory,16U);CHECK(other!=NULL);
            memory=other;other=NULL;
            for(size_t i=0U;i<8U;++i) CHECK(((unsigned char *)memory)[i]==0x3c);
        }
        umi_free(&adapter,memory);memory=NULL;
    } else {
        CHECK(umi_budget_allocator_allocate(owner,8U,&memory)==UMI_STATUS_OK);
        memset(memory,0x3c,8U);
        CHECK(umi_budget_allocator_snapshot(owner,&before)==UMI_STATUS_OK);
        size_t attempts=parent->attempts;
        if(MATCH("byte-limit") || MATCH("block-limit") || MATCH("overflow-limit")) {
            CHECK(umi_budget_allocator_allocate(owner,MATCH("overflow-limit")?SIZE_MAX:1U,&other)==UMI_STATUS_CAPACITY_EXCEEDED && other==NULL);
            CHECK(parent->attempts==attempts);
        } else if(MATCH("busy-destroy")) {
            UmiBudgetAllocator *saved=owner;
            CHECK(umi_budget_allocator_destroy(&owner)==UMI_STATUS_BUSY && owner==saved);
            CHECK(parent->live==2U);
        } else if(MATCH("release-interior")) {
            CHECK(umi_budget_allocator_release(owner,(unsigned char *)memory+1U)==UMI_STATUS_INVALID_ARGUMENT);
            CHECK(parent->live==2U);
        } else if(MATCH("resize-zero") || MATCH("resize-failure") || MATCH("resize-transient-limit")) {
            if(MATCH("resize-failure")) parent->fail_at=attempts+1U;
            other=memory;
            UmiStatus expected=MATCH("resize-zero")?UMI_STATUS_INVALID_ARGUMENT:
                MATCH("resize-failure")?UMI_STATUS_OUT_OF_MEMORY:UMI_STATUS_CAPACITY_EXCEEDED;
            CHECK(umi_budget_allocator_resize(owner,memory,MATCH("resize-zero")?0U:MATCH("resize-transient-limit")?12U:16U,&other)==expected && other==memory);
            other=NULL;
            for(size_t i=0U;i<8U;++i) CHECK(((unsigned char *)memory)[i]==0x3c);
            CHECK(umi_budget_allocator_snapshot(owner,&state)==UMI_STATUS_OK);
            CHECK(state.live_bytes==8U && state.live_blocks==1U && state.peak_bytes==8U);
        } else if(MATCH("resize-grow") || MATCH("resize-shrink") || MATCH("resize-same")) {
            size_t bytes=MATCH("resize-grow")?16U:MATCH("resize-shrink")?4U:8U;
            CHECK(umi_budget_allocator_resize(owner,memory,bytes,&other)==UMI_STATUS_OK);
            memory=other;other=NULL;
            for(size_t i=0U;i<(bytes<8U?bytes:8U);++i) CHECK(((unsigned char *)memory)[i]==0x3c);
            CHECK(umi_budget_allocator_snapshot(owner,&state)==UMI_STATUS_OK && state.live_bytes==bytes && state.live_blocks==1U);
            CHECK(state.peak_bytes==(MATCH("resize-same")?8U:8U+bytes));
            CHECK(parent->attempts==attempts+(MATCH("resize-same")?0U:1U));
            CHECK(parent->peak==state.metadata_bytes+state.peak_bytes);
        } else if(MATCH("resize-occupied-output")) {
            other=&foreign;
            CHECK(umi_budget_allocator_resize(owner,memory,12U,&other)==UMI_STATUS_INVALID_ARGUMENT && other==&foreign);
            CHECK(parent->attempts==attempts && ((unsigned char *)memory)[0]==0x3c);
            other=NULL;
        } else if(MATCH("resize-null")) {
            CHECK(umi_budget_allocator_resize(owner,NULL,4U,&other)==UMI_STATUS_OK);
        } else if(MATCH("slot-reuse")) {
            CHECK(umi_budget_allocator_release(owner,memory)==UMI_STATUS_OK);memory=NULL;
            CHECK(umi_budget_allocator_allocate(owner,12U,&memory)==UMI_STATUS_OK);
            CHECK(umi_budget_allocator_snapshot(owner,&state)==UMI_STATUS_OK && state.live_blocks==1U && state.live_bytes==12U);
        } else if(MATCH("copied-snapshot")) {
            state=before;state.live_bytes=0U;state.block_limit=0U;
            CHECK(umi_budget_allocator_snapshot(owner,&state)==UMI_STATUS_OK && state.live_bytes==8U && state.block_limit==blocks);
        } else if(MATCH("two-owners")) {
            UmiBudgetAllocator *second=NULL;
            CHECK(umi_budget_allocator_create(&allocator,64U,2U,&second)==UMI_STATUS_OK);
            CHECK(umi_budget_allocator_release(second,memory)==UMI_STATUS_INVALID_ARGUMENT);
            CHECK(umi_budget_allocator_destroy(&second)==UMI_STATUS_OK);
        } else CHECK(MATCH("basic") || MATCH("parent-no-reallocate"));
    }
    if(memory!=NULL) CHECK(umi_budget_allocator_release(owner,memory)==UMI_STATUS_OK);
    if(other!=NULL) CHECK(umi_budget_allocator_release(owner,other)==UMI_STATUS_OK);
    if(owner!=NULL) {
        CHECK(umi_budget_allocator_snapshot(owner,&state)==UMI_STATUS_OK);
        CHECK(state.live_bytes==0U && state.live_blocks==0U);
    }
    CHECK(umi_budget_allocator_destroy(&owner)==UMI_STATUS_OK && owner==NULL);
    CHECK(parent->live==0U && parent->payload==0U && parent->invalid_free==0);
    return 0;
}
int main(int argc,char **argv)
{
    if(argc!=2) return 2;
    Parent *parent=calloc(1U,sizeof(*parent));
    if(parent==NULL) return 3;
    int result=run(argv[1],parent);
    /* A failing assertion is still a failed test; reclaim only fixture-owned
     * blocks before exiting so diagnostics do not manufacture unrelated leaks. */
    for(size_t i=0U;i<PARENT_SLOTS;++i) free(parent->records[i].pointer);
    free(parent);
    return result;
}
