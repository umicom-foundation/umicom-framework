/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform/test_recent_items_revision.c
 *
 * PURPOSE:
 *   Verify revision exhaustion preserves history and never partly trims a list.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../../src/platform/recent_items_internal.h"
#include "umicom/platform/filesystem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(x) do { if (!(x)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#x); result=1; goto done; } } while (0)

static int Same(const UmiRecentItemRegistry *a, const UmiRecentItemRegistry *b)
{
    if(a->count!=b->count || a->revision!=b->revision) return 0;
    for(size_t i=0U;i<a->count;++i) {
        const UmiRecentItemSnapshot *x=&a->items[i], *y=&b->items[i];
        if(strcmp(x->id,y->id)!=0 || strcmp(x->uri,y->uri)!=0 ||
            strcmp(x->label,y->label)!=0 || strcmp(x->kind,y->kind)!=0 ||
            x->struct_size!=y->struct_size || x->api_version!=y->api_version ||
            x->last_opened!=y->last_opened || x->open_count!=y->open_count ||
            x->pinned!=y->pinned || x->revision!=y->revision) return 0;
    }
    return 1;
}
int main(int argc,char **argv)
{
    if(argc!=3) return 2;
    const char *mode=argv[1];
    int result=0;
    UmiRecentItemRegistry *registry=NULL, *before=NULL;
    CHECK(umi_platform_recent_items_registry_create(&registry)==UMI_STATUS_OK);
    before=malloc(sizeof(*before)); CHECK(before!=NULL);
    for(size_t i=0U;i<3U;++i) {
        UmiRecentItemSnapshot item={0};
        (void)snprintf(item.id,sizeof(item.id),"item-%zu",i);
        (void)snprintf(item.uri,sizeof(item.uri),"example://item-%zu",i);
        (void)snprintf(item.label,sizeof(item.label),"Item %zu",i);
        (void)snprintf(item.kind,sizeof(item.kind),"%s",i<2U?"workspace":"lesson");
        item.last_opened=(uint64_t)i; item.open_count=2U; item.pinned=i==2U;
        CHECK(umi_platform_recent_items_registry_upsert(registry,&item)==UMI_STATUS_OK);
    }
    /* Only the private fixture changes the boundary value. The real exported
     * mutation functions below are linked from the Framework library. */
    registry->revision=UINT64_MAX;
    if(strcmp(mode,"trim-budget")==0 || strcmp(mode,"trim-kind-budget")==0 ||
        strcmp(mode,"trim-one")==0 || strcmp(mode,"trim-kind-one")==0 ||
        strcmp(mode,"touch-final")==0) registry->revision=UINT64_MAX-1U;
    if(strcmp(mode,"trim-fit")==0) registry->revision=UINT64_MAX-2U;
    if(strcmp(mode,"touch-count-saturation")==0) {
        registry->revision=20U; registry->items[0].open_count=UINT64_MAX;
    }
    if(strcmp(mode,"pinned-only")==0)
        for(size_t i=0U;i<registry->count;++i) registry->items[i].pinned=1;
    *before=*registry;
    UmiStatus expected=UMI_STATUS_CAPACITY_EXCEEDED, actual=UMI_STATUS_INTERNAL_ERROR;
    size_t removed=0U; int touched=0, noChange=0;
    if(strcmp(mode,"upsert-new")==0 || strcmp(mode,"upsert-existing")==0) {
        UmiRecentItemSnapshot item=registry->items[0];
        if(strcmp(mode,"upsert-new")==0) (void)snprintf(item.id,sizeof(item.id),"%s","new-item");
        (void)snprintf(item.label,sizeof(item.label),"%s","Replacement label");
        actual=umi_platform_recent_items_registry_upsert(registry,&item);
    } else if(strcmp(mode,"remove-first")==0)
        actual=umi_platform_recent_items_registry_remove(registry,"item-0");
    else if(strcmp(mode,"remove-middle")==0)
        actual=umi_platform_recent_items_registry_remove(registry,"item-1");
    else if(strcmp(mode,"remove-last")==0)
        actual=umi_platform_recent_items_registry_remove(registry,"item-2");
    else if(strcmp(mode,"touch")==0 || strcmp(mode,"touch-final")==0 || strcmp(mode,"touch-count-saturation")==0) {
        if(strcmp(mode,"touch")!=0){expected=UMI_STATUS_OK;touched=1;}
        actual=umi_platform_recent_items_registry_touch(registry,"item-0",88U);
    } else if(strcmp(mode,"pin")==0)
        actual=umi_platform_recent_items_registry_set_pinned(registry,"item-0",1);
    else if(strcmp(mode,"trim")==0 || strcmp(mode,"trim-budget")==0)
        actual=umi_platform_recent_items_registry_trim(registry,0U);
    else if(strcmp(mode,"trim-kind")==0 || strcmp(mode,"trim-kind-budget")==0)
        actual=UmiRecentItemsTrimKind(registry,"workspace",0U);
    else if(strcmp(mode,"trim-one")==0) {
        expected=UMI_STATUS_OK;removed=1U;
        actual=umi_platform_recent_items_registry_trim(registry,2U);
    } else if(strcmp(mode,"trim-kind-one")==0) {
        expected=UMI_STATUS_OK;removed=1U;
        actual=UmiRecentItemsTrimKind(registry,"workspace",1U);
    } else if(strcmp(mode,"trim-fit")==0) {
        expected=UMI_STATUS_OK;removed=2U;
        actual=umi_platform_recent_items_registry_trim(registry,1U);
    } else if(strcmp(mode,"no-op")==0) {
        expected=UMI_STATUS_OK;noChange=1;
        actual=umi_platform_recent_items_registry_trim(registry,3U);
    } else if(strcmp(mode,"pinned-only")==0) {
        expected=UMI_STATUS_OK;noChange=1;
        actual=umi_platform_recent_items_registry_trim(registry,0U);
    } else if(strcmp(mode,"kind-empty")==0) {
        expected=UMI_STATUS_OK;noChange=1;
        actual=UmiRecentItemsTrimKind(registry,"other",0U);
    } else if(strcmp(mode,"pin-local")==0 || strcmp(mode,"remove-local")==0) {
        char metadata[UMI_PATH_CAPACITY];
        CHECK(umi_fs_is_absolute(argv[2]));
        CHECK(umi_fs_join(metadata,sizeof(metadata),argv[2],"not-written-history")==UMI_STATUS_OK);
        actual=strcmp(mode,"pin-local")==0 ?
            UmiRecentItemsSetPinnedLocal(registry,"item-0",registry->items[0].revision,1,metadata) :
            UmiRecentItemsRemoveLocal(registry,"item-0",registry->items[0].revision,metadata);
    } else CHECK(0 && "Unknown boundary case");
    CHECK(actual==expected);
    if(expected!=UMI_STATUS_OK || noChange) CHECK(Same(registry,before));
    else if(removed!=0U) {
        CHECK(registry->revision==UINT64_MAX && registry->count==3U-removed);
        CHECK(strcmp(registry->items[registry->count-1U].id,"item-2")==0);
        CHECK(registry->items[registry->count-1U].pinned==1);
        CHECK(strcmp(registry->items[0].id,removed==1U?"item-1":"item-2")==0);
    } else if(touched) {
        CHECK(registry->revision==before->revision+1U);
        CHECK(registry->items[0].last_opened==88U);
        CHECK(registry->items[0].open_count==(strcmp(mode,"touch-count-saturation")==0?UINT64_MAX:3U));
        CHECK(registry->items[0].revision==registry->revision);
        CHECK(registry->items[1].revision==before->items[1].revision);
        if(strcmp(mode,"touch-final")==0) {
            *before=*registry;
            CHECK(umi_platform_recent_items_registry_touch(registry,"item-0",99U)==UMI_STATUS_CAPACITY_EXCEEDED);
            CHECK(Same(registry,before));
        }
    }
done:
    free(before);
    umi_platform_recent_items_registry_destroy(registry);
    return result;
}
