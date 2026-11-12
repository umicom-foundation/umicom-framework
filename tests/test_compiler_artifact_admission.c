/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_compiler_artifact_admission.c
 *
 * PURPOSE:
 *   Verify complete artifact owners, bounded lookup and preservation at publication limits.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/compiler/artifact.h"
#include <stdio.h>
#include <string.h>
#define REQUIRE(condition) do { if (!(condition)) { fprintf(stderr,"artifact admission line %d\n",__LINE__); return 1; } } while (0)
static UmiCompilerArtifactStore store, before;
static UmiCompilerArtifact item;
static void reset_store(void)
{
    (void)memset(&store,0,sizeof(store));
    (void)memset(&item,0,sizeof(item));
    (void)strcpy(item.artifact_id,"object");
    (void)strcpy(item.path,"build/object.o");
    item.kind = UMI_COMPILER_ARTIFACT_OBJECT;
    item.language = UMI_COMPILER_LANGUAGE_C;
}
int main(void)
{
    reset_store();
    REQUIRE(!umi_compiler_artifact_valid(NULL));
    REQUIRE(umi_compiler_artifact_store_valid(&store));
    REQUIRE(umi_compiler_artifact_store_add(&store,&item) == UMI_STATUS_OK);
    REQUIRE(store.count == 1U && store.revision == 1U);
    REQUIRE(umi_compiler_artifact_store_find(&store,"object") == &store.items[0]);
    REQUIRE(umi_compiler_artifact_store_count_kind(&store,UMI_COMPILER_ARTIFACT_OBJECT) == 1U);
    REQUIRE(umi_compiler_artifact_store_count_kind(&store,(UmiCompilerArtifactKind)-1) == 0U);
    /* Neutral metadata is valid, but arbitrary enum values are not admitted. */
    item.kind = UMI_COMPILER_ARTIFACT_METADATA; item.language = UMI_COMPILER_LANGUAGE_UNKNOWN;
    REQUIRE(umi_compiler_artifact_valid(&item));
    item.kind = (UmiCompilerArtifactKind)0;
    REQUIRE(!umi_compiler_artifact_valid(&item));
    item.kind = UMI_COMPILER_ARTIFACT_OBJECT; item.language = (UmiCompilerLanguage)99;
    REQUIRE(!umi_compiler_artifact_valid(&item));
    reset_store();
    (void)memset(item.artifact_id,'a',sizeof(item.artifact_id)-1U);
    (void)memset(item.path,'p',sizeof(item.path)-1U);
    REQUIRE(umi_compiler_artifact_store_add(&store,&item) == UMI_STATUS_OK);
    REQUIRE(umi_compiler_artifact_store_find(&store,item.artifact_id) == &store.items[0]);
    for (unsigned field = 0U; field < 2U; ++field) {
        reset_store(); before = store;
        if (field == 0U) (void)memset(item.artifact_id,'x',sizeof(item.artifact_id));
        else (void)memset(item.path,'x',sizeof(item.path));
        REQUIRE(umi_compiler_artifact_store_add(&store,&item) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(memcmp(&store,&before,sizeof(store)) == 0);
    }
    /* A malformed late row blocks an earlier match and insertion alike. */
    reset_store();
    REQUIRE(umi_compiler_artifact_store_add(&store,&item) == UMI_STATUS_OK);
    store.items[1] = item; (void)strcpy(store.items[1].artifact_id,"late"); store.count = 2U;
    (void)memset(store.items[1].path,'x',sizeof(store.items[1].path)); before = store;
    REQUIRE(umi_compiler_artifact_store_find(&store,"object") == NULL);
    REQUIRE(umi_compiler_artifact_store_count_kind(&store,UMI_COMPILER_ARTIFACT_OBJECT) == 0U);
    REQUIRE(umi_compiler_artifact_store_add(&store,&item) == UMI_STATUS_INVALID_STATE);
    REQUIRE(memcmp(&store,&before,sizeof(store)) == 0);
    for (unsigned oversized = 0U; oversized < 2U; ++oversized) {
        reset_store(); store.count = oversized ? SIZE_MAX : UMI_COMPILER_MAX_ARTIFACTS + 1U; before = store;
        REQUIRE(!umi_compiler_artifact_store_valid(&store));
        REQUIRE(umi_compiler_artifact_store_add(&store,&item) == UMI_STATUS_INVALID_STATE);
        REQUIRE(umi_compiler_artifact_store_find(&store,"object") == NULL);
        REQUIRE(memcmp(&store,&before,sizeof(store)) == 0);
    }
    reset_store();
    for (size_t index = 0U; index < UMI_COMPILER_MAX_ARTIFACTS; ++index) {
        (void)snprintf(item.artifact_id,sizeof(item.artifact_id),"object-%zu",index);
        REQUIRE(umi_compiler_artifact_store_add(&store,&item) == UMI_STATUS_OK);
    }
    before = store;
    REQUIRE(umi_compiler_artifact_store_add(&store,&item) == UMI_STATUS_ALREADY_EXISTS);
    (void)strcpy(item.artifact_id,"new");
    REQUIRE(umi_compiler_artifact_store_add(&store,&item) == UMI_STATUS_CAPACITY_EXCEEDED);
    REQUIRE(memcmp(&store,&before,sizeof(store)) == 0);
    reset_store(); store.items[0] = item;
    REQUIRE(umi_compiler_artifact_store_add(&store,&store.items[0]) == UMI_STATUS_OK);
    store.revision = UINT64_MAX; before = store;
    REQUIRE(umi_compiler_artifact_store_add(&store,&store.items[0]) == UMI_STATUS_ALREADY_EXISTS);
    REQUIRE(umi_compiler_artifact_store_find(&store,"object") == &store.items[0]);
    (void)strcpy(item.artifact_id,"new");
    REQUIRE(umi_compiler_artifact_store_add(&store,&item) == UMI_STATUS_CAPACITY_EXCEEDED);
    REQUIRE(memcmp(&store,&before,sizeof(store)) == 0);
    char bad_lookup[UMI_COMPILER_ID_CAPACITY]; (void)memset(bad_lookup,'x',sizeof(bad_lookup));
    REQUIRE(umi_compiler_artifact_store_find(&store,bad_lookup) == NULL);
    REQUIRE(umi_compiler_artifact_store_find(&store,"") == NULL);
    REQUIRE(umi_compiler_artifact_store_find(&store,NULL) == NULL);
    return 0;
}
