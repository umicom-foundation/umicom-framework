/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/inlay_hint_model.h
 *
 * PURPOSE:
 *   Model resolved and unresolved inline parameter, type and return hints for
 *   every editor frontend without exposing graphical toolkit objects.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_INLAY_HINT_MODEL_H
#define UMICOM_EDITOR_INLAY_HINT_MODEL_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/editor/source_location.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_INLAY_HINT_MODEL_API_VERSION 1U
#define UMI_EDITOR_INLAY_HINT_ID_CAPACITY 128U
#define UMI_EDITOR_INLAY_HINT_TEXT_CAPACITY 512U
#define UMI_EDITOR_INLAY_HINT_PROVIDER_CAPACITY 128U

typedef enum UmiEditorInlayHintKind {
    UMI_EDITOR_INLAY_HINT_PARAMETER = 1,
    UMI_EDITOR_INLAY_HINT_TYPE = 2,
    UMI_EDITOR_INLAY_HINT_RETURN_TYPE = 3,
    UMI_EDITOR_INLAY_HINT_CHAINED_CALL = 4,
    UMI_EDITOR_INLAY_HINT_CUSTOM = 5
} UmiEditorInlayHintKind;

typedef struct UmiEditorInlayHintItem {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_EDITOR_INLAY_HINT_ID_CAPACITY];
    char provider_id[UMI_EDITOR_INLAY_HINT_PROVIDER_CAPACITY];
    char label[UMI_EDITOR_INLAY_HINT_TEXT_CAPACITY];
    char tooltip[UMI_EDITOR_INLAY_HINT_TEXT_CAPACITY];
    UmiEditorSourceLocation location;
    UmiEditorInlayHintKind kind;
    int padding_before;
    int padding_after;
    int resolved;
    int visible;
} UmiEditorInlayHintItem;

typedef struct UmiEditorInlayHintSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t item_count;
    size_t visible_item_count;
    size_t resolved_item_count;
    uint64_t revision;
} UmiEditorInlayHintSnapshot;

typedef struct UmiEditorInlayHintModel UmiEditorInlayHintModel;

UmiStatus umi_editor_inlay_hint_model_create(UmiEditorInlayHintModel **out_model);
void umi_editor_inlay_hint_model_destroy(UmiEditorInlayHintModel *model);
UmiStatus umi_editor_inlay_hint_model_clear(UmiEditorInlayHintModel *model);
UmiStatus umi_editor_inlay_hint_model_upsert(
    UmiEditorInlayHintModel *model,
    const UmiEditorInlayHintItem *item);
UmiStatus umi_editor_inlay_hint_model_remove(
    UmiEditorInlayHintModel *model,
    const char *item_id);
UmiStatus umi_editor_inlay_hint_model_set_kind_visible(
    UmiEditorInlayHintModel *model,
    UmiEditorInlayHintKind kind,
    int visible);
UmiStatus umi_editor_inlay_hint_model_sort(UmiEditorInlayHintModel *model);
UmiStatus umi_editor_inlay_hint_model_at(
    const UmiEditorInlayHintModel *model,
    size_t index,
    UmiEditorInlayHintItem *out_item);
UmiStatus umi_editor_inlay_hint_model_visible_at(
    const UmiEditorInlayHintModel *model,
    size_t visible_index,
    UmiEditorInlayHintItem *out_item);
UmiStatus umi_editor_inlay_hint_model_snapshot(
    const UmiEditorInlayHintModel *model,
    UmiEditorInlayHintSnapshot *out_snapshot);
size_t umi_editor_inlay_hint_model_count(const UmiEditorInlayHintModel *model);
uint64_t umi_editor_inlay_hint_model_revision(const UmiEditorInlayHintModel *model);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_INLAY_HINT_MODEL_H */
