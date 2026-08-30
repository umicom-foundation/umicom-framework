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

/* BEGINNER NOTE:
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
typedef struct UmiEditorSelectionRangeRegistry UmiEditorSelectionRangeRegistry;
UmiStatus umi_editor_selection_range_registry_create(UmiEditorSelectionRangeRegistry **out_registry);
void umi_editor_selection_range_registry_destroy(UmiEditorSelectionRangeRegistry *registry);
UmiStatus umi_editor_selection_range_registry_upsert(UmiEditorSelectionRangeRegistry *registry,const UmiEditorSelectionRangeSnapshot *item);
UmiStatus umi_editor_selection_range_registry_remove(UmiEditorSelectionRangeRegistry *registry,const char *id);
UmiStatus umi_editor_selection_range_registry_find(const UmiEditorSelectionRangeRegistry *registry,const char *id,UmiEditorSelectionRangeSnapshot *out_item);
UmiStatus umi_editor_selection_range_registry_at(const UmiEditorSelectionRangeRegistry *registry,size_t index,UmiEditorSelectionRangeSnapshot *out_item);
size_t umi_editor_selection_range_registry_count(const UmiEditorSelectionRangeRegistry *registry);
uint64_t umi_editor_selection_range_registry_revision(const UmiEditorSelectionRangeRegistry *registry);
#ifdef __cplusplus
}
#endif
#endif
