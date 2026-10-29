/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/recent_history/main.c
 *
 * PURPOSE:
 *   Demonstrate saving a recent-history change before publishing it in memory.
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
#include "umicom/platform/native_arguments.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define REQUIRE(x) do { if (!(x)) { fprintf(stderr,"line %d: %s\n",__LINE__,#x); goto done; } } while (0)
static int Run(int argc, char **argv)
{
    if (argc != 3) {
        fputs("Usage: umicom-recent-history ABSOLUTE_METADATA_FILE remove|stale|storage-refusal|pin|unpin|pin-stale|pin-storage-refusal\n",stderr);
        return 2;
    }
    const char *metadata=argv[1], *mode=argv[2];
    int pinning=strcmp(mode,"pin")==0 || strcmp(mode,"unpin")==0 || strncmp(mode,"pin-",4U)==0;
    int stale=strcmp(mode,"stale")==0 || strcmp(mode,"pin-stale")==0;
    int refusal=strcmp(mode,"storage-refusal")==0 || strcmp(mode,"pin-storage-refusal")==0;
    if (!stale && !refusal && strcmp(mode,"remove")!=0 &&
        strcmp(mode,"pin")!=0 && strcmp(mode,"unpin")!=0) return 2;
    UmiRecentItemRegistry *registry=NULL, *reopened=NULL;
    unsigned char *before=NULL, *after=NULL;
    size_t before_size=0U, after_size=0U;
    int loaded=0, result=1;
    char parent[UMI_PATH_CAPACITY];
    REQUIRE(umi_fs_is_absolute(metadata));
    REQUIRE(umi_path_parent(metadata,parent,sizeof(parent))==UMI_STATUS_OK);
    REQUIRE(umi_fs_is_directory(parent));
    /* Malformed existing metadata is never replaced. This example uses two
     * named teaching entries in an explicitly selected sample history file. */
    REQUIRE(UmiRecentItemsLoadLocal(metadata,&registry,&loaded)==UMI_STATUS_OK);
    UmiRecentItemSnapshot item={0}, selected, kept;
    (void)snprintf(item.id,sizeof(item.id),"%s","learning-history-project");
    (void)snprintf(item.label,sizeof(item.label),"%s","Sample project");
    (void)snprintf(item.uri,sizeof(item.uri),"%s","workspace://learning-history-example");
    (void)snprintf(item.kind,sizeof(item.kind),"%s","workspace");
    item.open_count=1U;item.last_opened=10U;item.pinned=strcmp(mode,"unpin")==0;
    REQUIRE(umi_platform_recent_items_registry_upsert(registry,&item)==UMI_STATUS_OK);
    REQUIRE(umi_platform_recent_items_registry_find(registry,item.id,&selected)==UMI_STATUS_OK);
    item.pinned=0;
    (void)snprintf(item.id,sizeof(item.id),"%s","learning-history-guide");
    (void)snprintf(item.kind,sizeof(item.kind),"%s","lesson");
    (void)snprintf(item.label,sizeof(item.label),"%s","Learning about local history");
    (void)snprintf(item.uri,sizeof(item.uri),"%s","lesson://recent-history");
    REQUIRE(umi_platform_recent_items_registry_upsert(registry,&item)==UMI_STATUS_OK);
    REQUIRE(umi_platform_recent_items_registry_find(registry,item.id,&kept)==UMI_STATUS_OK);
    REQUIRE(UmiRecentItemsSaveLocal(registry,metadata)==UMI_STATUS_OK);
    REQUIRE(UmiInputFileRead(metadata,4U*1024U*1024U,&before,&before_size)==UMI_STATUS_OK);
    if (stale) REQUIRE(umi_platform_recent_items_registry_touch(registry,selected.id,20U)==UMI_STATUS_OK);
    uint64_t revision=umi_platform_recent_items_registry_revision(registry);
    size_t count=umi_platform_recent_items_registry_count(registry);
    /* The copied item revision represents the row the user actually saw.
     * An unrelated lesson update does not invalidate that project selection. */
    /* Preserve the original removal-only example call for comparison. Both
     * commands below use the same expected revision and candidate-save boundary. */
#if 0
    UmiStatus status=UmiRecentItemsRemoveLocal(registry,selected.id,selected.revision,refusal ? parent : metadata);
#endif
    UmiStatus status=pinning ?
        UmiRecentItemsSetPinnedLocal(registry,selected.id,selected.revision,strcmp(mode,"unpin")!=0,refusal?parent:metadata) :
        UmiRecentItemsRemoveLocal(registry,selected.id,selected.revision,refusal?parent:metadata);
    if (stale || refusal) {
        REQUIRE(status==(stale ? UMI_STATUS_INVALID_STATE : UMI_STATUS_INVALID_ARGUMENT));
        REQUIRE(umi_platform_recent_items_registry_revision(registry)==revision);
        REQUIRE(umi_platform_recent_items_registry_count(registry)==count);
        REQUIRE(UmiInputFileRead(metadata,4U*1024U*1024U,&after,&after_size)==UMI_STATUS_OK);
        REQUIRE(before_size==after_size && memcmp(before,after,before_size)==0);
        REQUIRE(umi_platform_recent_items_registry_find(registry,selected.id,&item)==UMI_STATUS_OK);
        puts(stale ? "The stale selection was refused; memory and saved history were kept." :
            "The directory destination was refused; memory and saved history were kept.");
    } else if (pinning) {
        REQUIRE(status==UMI_STATUS_OK);
        REQUIRE(umi_platform_recent_items_registry_count(registry)==count);
        REQUIRE(umi_platform_recent_items_registry_revision(registry)==revision+1U);
        REQUIRE(UmiRecentItemsLoadLocal(metadata,&reopened,&loaded)==UMI_STATUS_OK && loaded);
        REQUIRE(umi_platform_recent_items_registry_find(reopened,selected.id,&item)==UMI_STATUS_OK);
        REQUIRE(item.pinned==(strcmp(mode,"unpin")!=0));
        REQUIRE(strcmp(item.uri,selected.uri)==0 && item.open_count==selected.open_count);
        puts(item.pinned?"Project history pinned; the resource was not opened.":
            "Project history unpinned; the resource was kept.");
    } else {
        REQUIRE(status==UMI_STATUS_OK);
        REQUIRE(umi_platform_recent_items_registry_count(registry)==count-1U);
        REQUIRE(umi_platform_recent_items_registry_revision(registry)==revision+1U);
        REQUIRE(umi_platform_recent_items_registry_find(registry,selected.id,&item)==UMI_STATUS_NOT_FOUND);
        REQUIRE(UmiRecentItemsLoadLocal(metadata,&reopened,&loaded)==UMI_STATUS_OK && loaded);
        REQUIRE(umi_platform_recent_items_registry_find(reopened,selected.id,&item)==UMI_STATUS_NOT_FOUND);
        REQUIRE(umi_platform_recent_items_registry_find(reopened,kept.id,&item)==UMI_STATUS_OK);
        REQUIRE(strcmp(item.uri,kept.uri)==0 && strcmp(item.kind,"lesson")==0);
        puts("The project history entry was removed; the lesson entry was preserved.");
    }
    REQUIRE(umi_platform_recent_items_registry_find(registry,kept.id,&item)==UMI_STATUS_OK);
    REQUIRE(memcmp(&item,&kept,sizeof(item))==0);
    puts("No recorded URI was opened or deleted. Keep the sample metadata for inspection.");
    result=ferror(stdout) || fflush(stdout) ? 1 : 0;
done:
    UmiInputFileFree(before);UmiInputFileFree(after);
    umi_platform_recent_items_registry_destroy(reopened);
    umi_platform_recent_items_registry_destroy(registry);
    return result;
}
int main(int argc,char **argv)
{
    int result=1;
    UmiStatus status=UmiNativeArgumentsDispatch(argc,argv,Run,&result);
    if (status!=UMI_STATUS_OK) { fprintf(stderr,"Cannot capture native arguments: %d\n",(int)status);return 1; }
    return result;
}
