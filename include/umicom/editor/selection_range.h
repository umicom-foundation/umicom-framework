/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/selection_range.h
 *
 * PURPOSE:
 *   Define editor selection ranges for normal and rectangular selections.
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
#ifndef UMICOM_EDITOR_SELECTION_RANGE_H
#define UMICOM_EDITOR_SELECTION_RANGE_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_EDITOR_SELECTION_RANGE_CAPACITY 4096U
/**
 * Represent the editor selection range snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorSelectionRangeSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char document_id[128];
    uint64_t anchor_line;
    uint64_t anchor_column;
    uint64_t active_line;
    uint64_t active_column;
    int rectangular;
    uint64_t revision;
} UmiEditorSelectionRangeSnapshot;
/**
 * Represent the editor selection range registry data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorSelectionRangeRegistry UmiEditorSelectionRangeRegistry;
/**
 * Initialise editor selection range registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_selection_range_registry_create(UmiEditorSelectionRangeRegistry **out_registry);
/**
 * Release or reset state held by editor selection range registry so the same storage can
 * be reused safely.
 */
void umi_editor_selection_range_registry_destroy(UmiEditorSelectionRangeRegistry *registry);
/**
 * Provide the editor selection range registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_selection_range_registry_upsert(UmiEditorSelectionRangeRegistry *registry,const UmiEditorSelectionRangeSnapshot *item);
/**
 * Remove editor selection range registry while keeping the remaining records in a valid
 * and discoverable state.
 */
UmiStatus umi_editor_selection_range_registry_remove(UmiEditorSelectionRangeRegistry *registry,const char *id);
/**
 * Find editor selection range registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_editor_selection_range_registry_find(const UmiEditorSelectionRangeRegistry *registry,const char *id,UmiEditorSelectionRangeSnapshot *out_item);
/**
 * Find editor selection range registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_editor_selection_range_registry_at(const UmiEditorSelectionRangeRegistry *registry,size_t index,UmiEditorSelectionRangeSnapshot *out_item);
/**
 * Return the number of records represented by editor selection range registry without
 * changing their state.
 */
size_t umi_editor_selection_range_registry_count(const UmiEditorSelectionRangeRegistry *registry);
/**
 * Provide the editor selection range registry revision operation used by this module and
 * its client applications.
 */
uint64_t umi_editor_selection_range_registry_revision(const UmiEditorSelectionRangeRegistry *registry);
#ifdef __cplusplus
}
#endif
#endif
