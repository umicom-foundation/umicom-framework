/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/fold_region.h
 *
 * PURPOSE:
 *   Define folding regions without coupling language analysis to a text widget.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This is a product-neutral C23 model. The registry owns snapshot copies by
 * value; callers own external resources and coordinate cross-thread mutation.
 */
#ifndef UMICOM_EDITOR_FOLD_REGION_H
#define UMICOM_EDITOR_FOLD_REGION_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_EDITOR_FOLD_REGION_CAPACITY 4096U
typedef struct UmiEditorFoldRegionSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char document_id[128];
    char kind[64];
    uint64_t start_line;
    uint64_t end_line;
    int collapsed;
    uint64_t revision;
} UmiEditorFoldRegionSnapshot;
typedef struct UmiEditorFoldRegionRegistry UmiEditorFoldRegionRegistry;
UmiStatus umi_editor_fold_region_registry_create(UmiEditorFoldRegionRegistry **out_registry);
void umi_editor_fold_region_registry_destroy(UmiEditorFoldRegionRegistry *registry);
UmiStatus umi_editor_fold_region_registry_upsert(UmiEditorFoldRegionRegistry *registry,const UmiEditorFoldRegionSnapshot *item);
UmiStatus umi_editor_fold_region_registry_remove(UmiEditorFoldRegionRegistry *registry,const char *id);
UmiStatus umi_editor_fold_region_registry_find(const UmiEditorFoldRegionRegistry *registry,const char *id,UmiEditorFoldRegionSnapshot *out_item);
UmiStatus umi_editor_fold_region_registry_at(const UmiEditorFoldRegionRegistry *registry,size_t index,UmiEditorFoldRegionSnapshot *out_item);
size_t umi_editor_fold_region_registry_count(const UmiEditorFoldRegionRegistry *registry);
uint64_t umi_editor_fold_region_registry_revision(const UmiEditorFoldRegionRegistry *registry);
#ifdef __cplusplus
}
#endif
#endif
