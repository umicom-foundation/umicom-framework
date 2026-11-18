/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain/test_document_copy_allocation.c
 *
 * PURPOSE:
 *   Inject copy-allocation failure and verify atomic budget admission and unchanged outputs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"line %d: %s\n",__LINE__,#x); exit(1); } } while (0)
static unsigned calls;
static int refuse;
static void *CopyAllocate(size_t size) { ++calls; return refuse ? NULL : malloc(size); }
/* Only malloc in the real document store is intercepted. Locks, edits and
 * release use production services; no simulated store substitutes for them. */
#define malloc CopyAllocate
#include "../../src/platform/document_store.c"
#undef malloc
int main(void)
{
    UmiDocumentStore *store=NULL; UmiDocumentId id=0U;
    CHECK(umi_document_store_create(&store)==UMI_STATUS_OK);
    CHECK(umi_document_store_new(store,"budget.uc",&id)==UMI_STATUS_OK);
    CHECK(umi_document_store_replace_text(store,id,"abc",3U)==UMI_STATUS_OK);
    UmiDocumentSnapshot snapshot,before; memset(&snapshot,0x35,sizeof snapshot);
    memcpy(&before,&snapshot,sizeof snapshot);
    char *copy=NULL; calls=0U; refuse=1;
    CHECK(UmiDocumentStoreCopySnapshotBounded(store,id,2U,&snapshot,&copy)==UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(calls==0U && copy==NULL && memcmp(&before,&snapshot,sizeof snapshot)==0);
    CHECK(UmiDocumentStoreCopySnapshotBounded(store,id,3U,&snapshot,&copy)==UMI_STATUS_OUT_OF_MEMORY);
    CHECK(calls==1U && copy==NULL && memcmp(&before,&snapshot,sizeof snapshot)==0);
    refuse=0;
    CHECK(UmiDocumentStoreCopySnapshotBounded(store,id,3U,&snapshot,&copy)==UMI_STATUS_OK);
    CHECK(calls==2U && snapshot.length==3U && strcmp(copy,"abc")==0);
    umi_document_store_free_text(copy);
    umi_document_store_destroy(store);
    return 0;
}
