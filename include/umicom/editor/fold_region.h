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
/**
 * Represent the editor fold region snapshot data shared with callers of this public
 * contract.
 */
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
/**
 * Represent the editor fold region registry data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorFoldRegionRegistry UmiEditorFoldRegionRegistry;
/**
 * Initialise editor fold region registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_fold_region_registry_create(UmiEditorFoldRegionRegistry **out_registry);
/**
 * Release or reset state held by editor fold region registry so the same storage can be
 * reused safely.
 */
void umi_editor_fold_region_registry_destroy(UmiEditorFoldRegionRegistry *registry);
/**
 * Provide the editor fold region registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_fold_region_registry_upsert(UmiEditorFoldRegionRegistry *registry,const UmiEditorFoldRegionSnapshot *item);
/**
 * Remove editor fold region registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_editor_fold_region_registry_remove(UmiEditorFoldRegionRegistry *registry,const char *id);
/**
 * Find editor fold region registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_editor_fold_region_registry_find(const UmiEditorFoldRegionRegistry *registry,const char *id,UmiEditorFoldRegionSnapshot *out_item);
/**
 * Find editor fold region registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_editor_fold_region_registry_at(const UmiEditorFoldRegionRegistry *registry,size_t index,UmiEditorFoldRegionSnapshot *out_item);
/**
 * Return the number of records represented by editor fold region registry without changing
 * their state.
 */
size_t umi_editor_fold_region_registry_count(const UmiEditorFoldRegionRegistry *registry);
/**
 * Provide the editor fold region registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_editor_fold_region_registry_revision(const UmiEditorFoldRegionRegistry *registry);
#ifdef __cplusplus
}
#endif
#endif
