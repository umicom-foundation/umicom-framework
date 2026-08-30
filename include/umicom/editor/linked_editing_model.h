/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/linked_editing_model.h
 *
 * PURPOSE:
 *   Define synchronized source ranges and construct canonical workspace edits
 *   for linked names, tags, snippet fields and language-provider selections.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_LINKED_EDITING_MODEL_H
#define UMICOM_EDITOR_LINKED_EDITING_MODEL_H

#include "umicom/editor/workspace_edit.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_LINKED_EDITING_API_VERSION 1U
#define UMI_EDITOR_LINKED_RANGE_ID_CAPACITY 128U
#define UMI_EDITOR_LINKED_GROUP_ID_CAPACITY 128U
#define UMI_EDITOR_LINKED_PROVIDER_CAPACITY 128U
#define UMI_EDITOR_LINKED_TEXT_CAPACITY 512U

typedef struct UmiEditorLinkedRange {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_EDITOR_LINKED_RANGE_ID_CAPACITY];
    char group_id[UMI_EDITOR_LINKED_GROUP_ID_CAPACITY];
    char provider_id[UMI_EDITOR_LINKED_PROVIDER_CAPACITY];
    char original_text[UMI_EDITOR_LINKED_TEXT_CAPACITY];
    UmiEditorSourceLocation location;
    int primary;
    int editable;
    int conflict;
} UmiEditorLinkedRange;

typedef struct UmiEditorLinkedEditingSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char active_group_id[UMI_EDITOR_LINKED_GROUP_ID_CAPACITY];
    size_t range_count;
    size_t group_count;
    size_t editable_count;
    size_t conflict_count;
    size_t active_group_range_count;
    uint64_t revision;
    int finalized;
    int has_active_group;
} UmiEditorLinkedEditingSnapshot;

typedef struct UmiEditorLinkedEditingModel UmiEditorLinkedEditingModel;

UmiStatus umi_editor_linked_editing_model_create(
    UmiEditorLinkedEditingModel **out_model);
void umi_editor_linked_editing_model_destroy(UmiEditorLinkedEditingModel *model);
UmiStatus umi_editor_linked_editing_model_clear(
    UmiEditorLinkedEditingModel *model);
UmiStatus umi_editor_linked_editing_model_upsert(
    UmiEditorLinkedEditingModel *model,
    const UmiEditorLinkedRange *range);
UmiStatus umi_editor_linked_editing_model_remove(
    UmiEditorLinkedEditingModel *model,
    const char *range_id);
UmiStatus umi_editor_linked_editing_model_remove_group(
    UmiEditorLinkedEditingModel *model,
    const char *group_id);
UmiStatus umi_editor_linked_editing_model_finalize(
    UmiEditorLinkedEditingModel *model);
UmiStatus umi_editor_linked_editing_model_set_active_group(
    UmiEditorLinkedEditingModel *model,
    const char *group_id);
UmiStatus umi_editor_linked_editing_model_clear_active_group(
    UmiEditorLinkedEditingModel *model);
UmiStatus umi_editor_linked_editing_model_build_edit_set(
    const UmiEditorLinkedEditingModel *model,
    const char *replacement_text,
    UmiEditorWorkspaceEditSet *edit_set);
UmiStatus umi_editor_linked_editing_model_at(
    const UmiEditorLinkedEditingModel *model,
    size_t index,
    UmiEditorLinkedRange *out_range);
UmiStatus umi_editor_linked_editing_model_active_group_at(
    const UmiEditorLinkedEditingModel *model,
    size_t index,
    UmiEditorLinkedRange *out_range);
UmiStatus umi_editor_linked_editing_model_snapshot(
    const UmiEditorLinkedEditingModel *model,
    UmiEditorLinkedEditingSnapshot *out_snapshot);
size_t umi_editor_linked_editing_model_count(
    const UmiEditorLinkedEditingModel *model);
uint64_t umi_editor_linked_editing_model_revision(
    const UmiEditorLinkedEditingModel *model);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_LINKED_EDITING_MODEL_H */
