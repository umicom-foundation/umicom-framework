/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform/test_recent_items_remove.c
 *
 * PURPOSE:
 *   Check durable recent-item removal, stale views and failure preservation.
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

#define CHECK(x) do { if (!(x)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#x); result=1; goto done; } } while (0)

static UmiRecentItemSnapshot Item(const char *id, const char *uri, const char *kind, int pinned)
{
    UmiRecentItemSnapshot item={0};
    item.struct_size=(uint32_t)sizeof(item); item.api_version=1U;
    (void)snprintf(item.id,sizeof(item.id),"%s",id);
    (void)snprintf(item.uri,sizeof(item.uri),"%s",uri);
    (void)snprintf(item.label,sizeof(item.label),"Reviewed %s",id);
    (void)snprintf(item.kind,sizeof(item.kind),"%s",kind);
    item.last_opened=10U; item.open_count=3U; item.pinned=pinned;
    return item;
}
int main(int argc,char **argv)
{
    if(argc!=3)return 2;
    int result=0,loaded=0;
    UmiRecentItemRegistry *registry=NULL,*disk=NULL;
    unsigned char *before=NULL,*after=NULL;
    size_t before_size=0U,after_size=0U;
    const char *mode=argv[1];
    char metadata[UMI_PATH_CAPACITY],resource[UMI_PATH_CAPACITY],bad_path[UMI_PATH_CAPACITY];
    CHECK(umi_fs_is_absolute(argv[2]) && umi_fs_is_directory(argv[2]));
    CHECK(umi_fs_join(metadata,sizeof(metadata),argv[2],
        strcmp(mode,"unicode")==0 ? "history-\xcf\x80.txt" : "history.txt")==UMI_STATUS_OK);
    CHECK(umi_fs_join(resource,sizeof(resource),argv[2],"project-source.txt")==UMI_STATUS_OK);
    CHECK(umi_fs_write_text(resource,"A learner's unsaved example remains on disk.\n")==UMI_STATUS_OK);
    CHECK(umi_platform_recent_items_registry_create(&registry)==UMI_STATUS_OK);
    UmiRecentItemSnapshot a=Item("first",resource,"workspace",0);
    UmiRecentItemSnapshot b=Item("middle","/unavailable/project","workspace",1);
    UmiRecentItemSnapshot c=Item("last","https://invalid.example/never-opened","lesson",0);
    CHECK(umi_platform_recent_items_registry_upsert(registry,&a)==UMI_STATUS_OK);
    CHECK(umi_platform_recent_items_registry_upsert(registry,&b)==UMI_STATUS_OK);
    CHECK(umi_platform_recent_items_registry_upsert(registry,&c)==UMI_STATUS_OK);
    const char *id=strcmp(mode,"first")==0 ? "first" : strcmp(mode,"last")==0 ? "last" : "middle";
    UmiRecentItemSnapshot selected,unchanged;
    CHECK(umi_platform_recent_items_registry_find(registry,id,&selected)==UMI_STATUS_OK);
    CHECK(umi_platform_recent_items_registry_find(registry,"first",&unchanged)==UMI_STATUS_OK);
    CHECK(UmiRecentItemsSaveLocal(registry,metadata)==UMI_STATUS_OK);
    CHECK(UmiInputFileRead(metadata,4U*1024U*1024U,&before,&before_size)==UMI_STATUS_OK);
    UmiStatus expected=UMI_STATUS_OK;
    uint64_t expected_revision=selected.revision;
    const char *target=metadata;
    UmiRecentItemRegistry *input=registry;
    if(strcmp(mode,"stale")==0) {
        CHECK(umi_platform_recent_items_registry_touch(registry,id,20U)==UMI_STATUS_OK);
        expected=UMI_STATUS_INVALID_STATE;
    } else if(strcmp(mode,"other-changed")==0) {
        CHECK(umi_platform_recent_items_registry_touch(registry,"last",30U)==UMI_STATUS_OK);
    } else if(strcmp(mode,"missing")==0) {id="absent";expected=UMI_STATUS_NOT_FOUND;}
    else if(strcmp(mode,"null-registry")==0){input=NULL;expected=UMI_STATUS_INVALID_ARGUMENT;}
    else if(strcmp(mode,"null-id")==0){id=NULL;expected=UMI_STATUS_INVALID_ARGUMENT;}
    else if(strcmp(mode,"empty-id")==0){id="";expected=UMI_STATUS_INVALID_ARGUMENT;}
    else if(strcmp(mode,"zero-revision")==0){expected_revision=0U;expected=UMI_STATUS_INVALID_ARGUMENT;}
    else if(strcmp(mode,"null-path")==0){target=NULL;expected=UMI_STATUS_INVALID_ARGUMENT;}
    else if(strcmp(mode,"relative")==0){target="relative-history.txt";expected=UMI_STATUS_INVALID_ARGUMENT;}
    else if(strcmp(mode,"directory")==0){target=argv[2];expected=UMI_STATUS_INVALID_ARGUMENT;}
    else if(strcmp(mode,"missing-parent")==0) {
        CHECK(umi_fs_join(bad_path,sizeof(bad_path),argv[2],"absent/history.txt")==UMI_STATUS_OK);
        target=bad_path;expected=UMI_STATUS_NOT_FOUND;
    } else CHECK(strcmp(mode,"first")==0 || strcmp(mode,"middle")==0 || strcmp(mode,"last")==0 ||
        strcmp(mode,"pinned")==0 || strcmp(mode,"unicode")==0 || strcmp(mode,"repeat")==0);
    uint64_t revision=umi_platform_recent_items_registry_revision(registry);
    UmiRecentItemRegistry *identity=registry;
    UmiStatus actual=UmiRecentItemsRemoveLocal(input,id,expected_revision,target);
    /* A missing parent maps native write errors differently across platforms.
     * The contract here is refusal with unchanged memory and existing metadata. */
    if(strcmp(mode,"missing-parent")==0) CHECK(actual!=UMI_STATUS_OK);
    else CHECK(actual==expected);
    CHECK(registry==identity);
    if(expected==UMI_STATUS_OK) {
        CHECK(umi_platform_recent_items_registry_count(registry)==2U);
        CHECK(umi_platform_recent_items_registry_revision(registry)==revision+1U);
        CHECK(umi_platform_recent_items_registry_find(registry,id,&selected)==UMI_STATUS_NOT_FOUND);
        if(strcmp(mode,"repeat")==0) {
            CHECK(UmiRecentItemsRemoveLocal(registry,id,expected_revision,metadata)==UMI_STATUS_NOT_FOUND);
            CHECK(umi_platform_recent_items_registry_revision(registry)==revision+1U);
        }
        CHECK(UmiRecentItemsLoadLocal(metadata,&disk,&loaded)==UMI_STATUS_OK && loaded);
        CHECK(umi_platform_recent_items_registry_count(disk)==2U);
        CHECK(umi_platform_recent_items_registry_find(disk,id,&selected)==UMI_STATUS_NOT_FOUND);
        if(strcmp(id,"first")!=0) {
            CHECK(umi_platform_recent_items_registry_find(registry,"first",&selected)==UMI_STATUS_OK);
            CHECK(memcmp(&selected,&unchanged,sizeof(selected))==0);
            CHECK(umi_platform_recent_items_registry_find(disk,"first",&selected)==UMI_STATUS_OK);
            CHECK(strcmp(selected.uri,resource)==0 && selected.open_count==3U);
        }
        if(strcmp(mode,"other-changed")==0) {
            CHECK(umi_platform_recent_items_registry_find(disk,"last",&selected)==UMI_STATUS_OK);
            CHECK(selected.last_opened==30U && strcmp(selected.kind,"lesson")==0);
        }
    } else {
        CHECK(umi_platform_recent_items_registry_count(registry)==3U);
        CHECK(umi_platform_recent_items_registry_revision(registry)==revision);
        CHECK(UmiInputFileRead(metadata,4U*1024U*1024U,&after,&after_size)==UMI_STATUS_OK);
        CHECK(after_size==before_size && memcmp(before,after,before_size)==0);
    }
    free(after);after=NULL;
    CHECK(UmiInputFileRead(resource,1024U,&after,&after_size)==UMI_STATUS_OK);
    CHECK(after_size==strlen("A learner's unsaved example remains on disk.\n") &&
        memcmp(after,"A learner's unsaved example remains on disk.\n",after_size)==0);
done:
    free(before);free(after);
    umi_platform_recent_items_registry_destroy(disk);
    umi_platform_recent_items_registry_destroy(registry);
    return result;
}
