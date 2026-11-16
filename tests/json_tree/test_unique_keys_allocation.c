/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/json_tree/test_unique_keys_allocation.c
 *
 * PURPOSE:
 *   Exercise actual JSON uniqueness allocation ownership and recovery without replacing the tree parser.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static size_t allocation_calls, fail_at, live_allocations;
static bool allocation_refused(void)
{
    ++allocation_calls;
    return fail_at!=0U && allocation_calls==fail_at;
}
static void *unique_malloc(size_t size)
{
    if(allocation_refused()) return NULL;
    void *memory=malloc(size);if(memory!=NULL) ++live_allocations;return memory;
}
static void *unique_calloc(size_t count,size_t size)
{
    if(allocation_refused()) return NULL;
    void *memory=calloc(count,size);if(memory!=NULL) ++live_allocations;return memory;
}
static void *unique_realloc(void *old,size_t size)
{
    if(allocation_refused()) return NULL;
    bool was_empty=old==NULL;
    void *memory=realloc(old,size);
    if(memory!=NULL && was_empty) ++live_allocations;
    return memory;
}
static void unique_free(void *memory)
{
    if(memory!=NULL) --live_allocations;
    free(memory);
}
/* Compile the real owner under private test names. Its own public header then
 * declares those names, keeping the production tree layout and parser intact. */
#define UmiJsonTreeDefaultLimits UmiTestJsonTreeDefaultLimits
#define UmiJsonTreeCreate UmiTestJsonTreeCreate
#define UmiJsonTreeDestroy UmiTestJsonTreeDestroy
#define UmiJsonTreeKind UmiTestJsonTreeKind
#define UmiJsonTreeFirst UmiTestJsonTreeFirst
#define UmiJsonTreeNext UmiTestJsonTreeNext
#define UmiJsonTreeCount UmiTestJsonTreeCount
#define UmiJsonTreeMember UmiTestJsonTreeMember
#define UmiJsonTreeText UmiTestJsonTreeText
#define UmiJsonTreeInteger UmiTestJsonTreeInteger
#define UmiJsonTreeBoolean UmiTestJsonTreeBoolean
#define UmiJsonTreeIsNull UmiTestJsonTreeIsNull
#define UmiJsonTreeSourceSpan UmiTestJsonTreeSourceSpan
#define UmiJsonTreeValuesEqual UmiTestJsonTreeValuesEqual
#define UmiJsonTreeValidateUniqueKeys UmiTestJsonTreeValidateUniqueKeys
#define malloc unique_malloc
#define calloc unique_calloc
#define realloc unique_realloc
#define free unique_free
#include "../../src/language_runtime/json_tree.c"
#undef free
#undef realloc
#undef calloc
#undef malloc
#undef UmiJsonTreeValidateUniqueKeys
#undef UmiJsonTreeValuesEqual
#undef UmiJsonTreeSourceSpan
#undef UmiJsonTreeIsNull
#undef UmiJsonTreeBoolean
#undef UmiJsonTreeInteger
#undef UmiJsonTreeText
#undef UmiJsonTreeMember
#undef UmiJsonTreeCount
#undef UmiJsonTreeNext
#undef UmiJsonTreeFirst
#undef UmiJsonTreeKind
#undef UmiJsonTreeDestroy
#undef UmiJsonTreeCreate
#undef UmiJsonTreeDefaultLimits
#define CHECK(condition) do { if (!(condition)) { fprintf(stderr,"line %d: %s\n",__LINE__,#condition); return 1; } } while (0)

static int exercise(const char *mode)
{
    const char *json=strcmp(mode,"duplicate-cleanup")==0?
        "{\"a\":{\"first\":1},\"z\":{\"duplicate\":1,\"duplicate\":2}}":
        "{\"a\":{\"first\":1},\"z\":[{\"second\":2,\"third\":3}]}";
    UmiJsonTree *tree=NULL;
    CHECK(UmiTestJsonTreeCreate(json,strlen(json),NULL,NULL,&tree)==UMI_STATUS_OK);
    size_t baseline=live_allocations;
    allocation_calls=0U;
    if(strcmp(mode,"allocation")==0) {
        CHECK(UmiTestJsonTreeValidateUniqueKeys(tree,0,NULL)==UMI_STATUS_OK);
        size_t calls=allocation_calls;
        CHECK(calls>1U && live_allocations==baseline);
        for(size_t failure=1U;failure<=calls;++failure) {
            allocation_calls=0U;fail_at=failure;
            CHECK(UmiTestJsonTreeValidateUniqueKeys(tree,0,NULL)==UMI_STATUS_OUT_OF_MEMORY);
            CHECK(live_allocations==baseline);
            const char *retained=NULL;size_t length=0U;
            CHECK(UmiTestJsonTreeSourceSpan(tree,0,&retained,&length)==UMI_STATUS_OK);
            CHECK(length==strlen(json) && memcmp(retained,json,length)==0);
            fail_at=0U;
            CHECK(UmiTestJsonTreeValidateUniqueKeys(tree,0,NULL)==UMI_STATUS_OK);
            CHECK(live_allocations==baseline);
        }
    } else if(strcmp(mode,"duplicate-cleanup")==0) {
        CHECK(UmiTestJsonTreeValidateUniqueKeys(tree,0,NULL)==UMI_STATUS_ALREADY_EXISTS);
        CHECK(allocation_calls>1U && live_allocations==baseline);
    } else if(strcmp(mode,"cancel")==0) {
        UmiCancellationToken *cancel=NULL;
        CHECK(umi_cancellation_token_create(&cancel)==UMI_STATUS_OK);
        umi_cancellation_token_request(cancel);
        CHECK(UmiTestJsonTreeValidateUniqueKeys(tree,0,cancel)==UMI_STATUS_CANCELLED);
        CHECK(allocation_calls==0U && live_allocations==baseline);
        umi_cancellation_token_destroy(cancel);
    } else if(strcmp(mode,"invalid")==0) {
        CHECK(UmiTestJsonTreeValidateUniqueKeys(NULL,0,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(allocation_calls==0U && live_allocations==baseline);
    } else { UmiTestJsonTreeDestroy(tree);return 2; }
    UmiTestJsonTreeDestroy(tree);
    CHECK(live_allocations==0U);
    return 0;
}
int main(int argc,char **argv) { return argc==2?exercise(argv[1]):2; }
