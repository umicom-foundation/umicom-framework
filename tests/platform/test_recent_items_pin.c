/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform/test_recent_items_pin.c
 *
 * PURPOSE:
 *   Verify durable pin decisions, stale rows, ordering and failed-save ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/platform/recent_items.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/input_file.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); result=1; goto done; } } while (0)

static UmiRecentItemSnapshot Item(const char *id, const char *uri, const char *kind,
    uint64_t opened, int pinned)
{
    UmiRecentItemSnapshot item = {0};
    (void)snprintf(item.id, sizeof(item.id), "%s", id);
    (void)snprintf(item.uri, sizeof(item.uri), "%s", uri);
    (void)snprintf(item.label, sizeof(item.label), "Project %s", id);
    (void)snprintf(item.kind, sizeof(item.kind), "%s", kind);
    item.last_opened=opened; item.open_count=7U; item.pinned=pinned;
    return item;
}
/* Compare the resource metadata separately from the deliberately changed pin
 * and revision. Pinning must not pretend that a project was opened again. */
static int SameResource(const UmiRecentItemSnapshot *a, const UmiRecentItemSnapshot *b)
{
    return strcmp(a->id,b->id)==0 && strcmp(a->uri,b->uri)==0 &&
        strcmp(a->label,b->label)==0 && strcmp(a->kind,b->kind)==0 &&
        a->last_opened==b->last_opened && a->open_count==b->open_count;
}
int main(int argc, char **argv)
{
    if (argc!=3) return 2;
    const char *mode=argv[1];
    int result=0, loaded=0, pin=strcmp(mode,"unpin")==0?0:1;
    UmiRecentItemRegistry *registry=NULL, *disk=NULL;
    unsigned char *before=NULL, *after=NULL;
    size_t beforeSize=0U, afterSize=0U;
    char metadata[UMI_PATH_CAPACITY], resource[UMI_PATH_CAPACITY], bad[UMI_PATH_CAPACITY];
    CHECK(umi_fs_is_absolute(argv[2]) && umi_fs_is_directory(argv[2]));
    CHECK(umi_fs_join(metadata,sizeof(metadata),argv[2],
        strcmp(mode,"unicode")==0?"pins-\xcf\x80.txt":"pins.txt")==UMI_STATUS_OK);
    CHECK(umi_fs_join(resource,sizeof(resource),argv[2],"project.txt")==UMI_STATUS_OK);
    CHECK(umi_fs_write_text(resource,"Project text is not history metadata.\n")==UMI_STATUS_OK);
    CHECK(umi_platform_recent_items_registry_create(&registry)==UMI_STATUS_OK);
    UmiRecentItemSnapshot a=Item("older",resource,"workspace",10U,
        strcmp(mode,"unpin")==0 || strcmp(mode,"already-pinned")==0);
    UmiRecentItemSnapshot b=Item("newer","/missing/example","workspace",30U,0);
    UmiRecentItemSnapshot c=Item("lesson","https://invalid.example/not-opened","lesson",40U,0);
    CHECK(umi_platform_recent_items_registry_upsert(registry,&a)==UMI_STATUS_OK);
    CHECK(umi_platform_recent_items_registry_upsert(registry,&b)==UMI_STATUS_OK);
    CHECK(umi_platform_recent_items_registry_upsert(registry,&c)==UMI_STATUS_OK);
    CHECK(umi_platform_recent_items_registry_find(registry,a.id,&a)==UMI_STATUS_OK);
    CHECK(umi_platform_recent_items_registry_find(registry,b.id,&b)==UMI_STATUS_OK);
    CHECK(UmiRecentItemsSaveLocal(registry,metadata)==UMI_STATUS_OK);
    CHECK(UmiInputFileRead(metadata,4U*1024U*1024U,&before,&beforeSize)==UMI_STATUS_OK);
    UmiRecentItemRegistry *input=registry, *identity=registry;
    const char *id=a.id, *target=metadata;
    uint64_t expectedRevision=a.revision;
    UmiStatus expected=UMI_STATUS_OK;
    if(strcmp(mode,"stale")==0) {
        CHECK(umi_platform_recent_items_registry_touch(registry,a.id,50U)==UMI_STATUS_OK);
        expected=UMI_STATUS_INVALID_STATE;
    } else if(strcmp(mode,"other-changed")==0) {
        CHECK(umi_platform_recent_items_registry_touch(registry,c.id,60U)==UMI_STATUS_OK);
    } else if(strcmp(mode,"missing")==0){id="absent";expected=UMI_STATUS_NOT_FOUND;}
    else if(strcmp(mode,"null-registry")==0){input=NULL;expected=UMI_STATUS_INVALID_ARGUMENT;}
    else if(strcmp(mode,"null-id")==0){id=NULL;expected=UMI_STATUS_INVALID_ARGUMENT;}
    else if(strcmp(mode,"empty-id")==0){id="";expected=UMI_STATUS_INVALID_ARGUMENT;}
    else if(strcmp(mode,"zero-revision")==0){expectedRevision=0U;expected=UMI_STATUS_INVALID_ARGUMENT;}
    else if(strcmp(mode,"null-path")==0){target=NULL;expected=UMI_STATUS_INVALID_ARGUMENT;}
    else if(strcmp(mode,"relative")==0){target="pins.txt";expected=UMI_STATUS_INVALID_ARGUMENT;}
    else if(strcmp(mode,"directory")==0){target=argv[2];expected=UMI_STATUS_INVALID_ARGUMENT;}
    else if(strcmp(mode,"missing-parent")==0) {
        CHECK(umi_fs_join(bad,sizeof(bad),argv[2],"absent/pins.txt")==UMI_STATUS_OK);
        target=bad;expected=UMI_STATUS_NOT_FOUND;
    } else if(strcmp(mode,"negative")==0) pin=-7;
    else CHECK(strcmp(mode,"pin")==0 || strcmp(mode,"unpin")==0 ||
        strcmp(mode,"already-pinned")==0 || strcmp(mode,"unicode")==0 ||
        strcmp(mode,"repeat")==0 || strcmp(mode,"retry")==0);
    UmiRecentItemSnapshot selected, found, ranked[3];
    CHECK(umi_platform_recent_items_registry_find(registry,a.id,&selected)==UMI_STATUS_OK);
    uint64_t revision=umi_platform_recent_items_registry_revision(registry);
    if(strcmp(mode,"retry")==0) {
        CHECK(UmiRecentItemsSetPinnedLocal(registry,id,expectedRevision,pin,argv[2])!=UMI_STATUS_OK);
        CHECK(umi_platform_recent_items_registry_revision(registry)==revision);
        CHECK(umi_platform_recent_items_registry_find(registry,a.id,&found)==UMI_STATUS_OK);
        CHECK(SameResource(&selected,&found) && selected.pinned==found.pinned &&
            selected.revision==found.revision);
    }
    UmiStatus actual=UmiRecentItemsSetPinnedLocal(input,id,expectedRevision,pin,target);
    if(strcmp(mode,"missing-parent")==0) CHECK(actual!=UMI_STATUS_OK);
    else CHECK(actual==expected);
    CHECK(registry==identity && umi_platform_recent_items_registry_count(registry)==3U);
    CHECK(umi_platform_recent_items_registry_find(registry,a.id,&found)==UMI_STATUS_OK);
    CHECK(SameResource(&selected,&found));
    if(expected==UMI_STATUS_OK) {
        CHECK(found.pinned==(pin!=0) && found.revision==revision+1U);
        CHECK(umi_platform_recent_items_registry_revision(registry)==revision+1U);
        CHECK(UmiRecentItemsLoadLocal(metadata,&disk,&loaded)==UMI_STATUS_OK && loaded);
        CHECK(umi_platform_recent_items_registry_count(disk)==3U);
        UmiRecentItemSnapshot restored;
        CHECK(umi_platform_recent_items_registry_find(disk,a.id,&restored)==UMI_STATUS_OK);
        CHECK(SameResource(&found,&restored) && restored.pinned==found.pinned);
        CHECK(umi_platform_recent_items_registry_find(registry,b.id,&restored)==UMI_STATUS_OK);
        CHECK(SameResource(&b,&restored) && b.revision==restored.revision && !restored.pinned);
        UmiRecentItemQuery query={.kind="workspace"};
        size_t count=0U;
        CHECK(umi_platform_recent_items_registry_query(disk,&query,ranked,3U,&count)==UMI_STATUS_OK);
        CHECK(count==2U && strcmp(ranked[0].id,pin!=0?a.id:b.id)==0);
        if(strcmp(mode,"other-changed")==0) {
            CHECK(umi_platform_recent_items_registry_find(disk,c.id,&restored)==UMI_STATUS_OK);
            CHECK(restored.last_opened==60U && restored.open_count==8U);
        }
        if(strcmp(mode,"repeat")==0) {
            CHECK(UmiRecentItemsSetPinnedLocal(registry,id,expectedRevision,0,metadata)==UMI_STATUS_INVALID_STATE);
            CHECK(umi_platform_recent_items_registry_find(registry,a.id,&restored)==UMI_STATUS_OK);
            CHECK(restored.pinned==1 && restored.revision==found.revision);
        }
    } else {
        CHECK(found.pinned==selected.pinned && found.revision==selected.revision);
        CHECK(umi_platform_recent_items_registry_revision(registry)==revision);
        CHECK(UmiInputFileRead(metadata,4U*1024U*1024U,&after,&afterSize)==UMI_STATUS_OK);
        CHECK(afterSize==beforeSize && memcmp(before,after,beforeSize)==0);
    }
    free(after);after=NULL;
    CHECK(UmiInputFileRead(resource,1024U,&after,&afterSize)==UMI_STATUS_OK);
    CHECK(afterSize==strlen("Project text is not history metadata.\n") &&
        memcmp(after,"Project text is not history metadata.\n",afterSize)==0);
done:
    free(before);free(after);
    umi_platform_recent_items_registry_destroy(disk);
    umi_platform_recent_items_registry_destroy(registry);
    return result;
}
