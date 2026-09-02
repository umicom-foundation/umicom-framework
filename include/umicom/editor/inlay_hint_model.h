/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/inlay_hint_model.h
 *
 * PURPOSE:
 *   Model resolved and unresolved inline parameter, type and return hints for
 *   every editor frontend without exposing graphical toolkit objects.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * List the named editor inlay hint kind values accepted by this public contract.
 */
typedef enum UmiEditorInlayHintKind {
    UMI_EDITOR_INLAY_HINT_PARAMETER = 1,
    UMI_EDITOR_INLAY_HINT_TYPE = 2,
    UMI_EDITOR_INLAY_HINT_RETURN_TYPE = 3,
    UMI_EDITOR_INLAY_HINT_CHAINED_CALL = 4,
    UMI_EDITOR_INLAY_HINT_CUSTOM = 5
} UmiEditorInlayHintKind;

/**
 * Represent the editor inlay hint item data shared with callers of this public contract.
 */
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

/**
 * Represent the editor inlay hint snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorInlayHintSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t item_count;
    size_t visible_item_count;
    size_t resolved_item_count;
    uint64_t revision;
} UmiEditorInlayHintSnapshot;

/**
 * Represent the editor inlay hint model data shared with callers of this public contract.
 */
typedef struct UmiEditorInlayHintModel UmiEditorInlayHintModel;

/**
 * Initialise editor inlay hint model from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_inlay_hint_model_create(UmiEditorInlayHintModel **out_model);
/**
 * Release or reset state held by editor inlay hint model so the same storage can be reused
 * safely.
 */
void umi_editor_inlay_hint_model_destroy(UmiEditorInlayHintModel *model);
/**
 * Release or reset state held by editor inlay hint model so the same storage can be reused
 * safely.
 */
UmiStatus umi_editor_inlay_hint_model_clear(UmiEditorInlayHintModel *model);
/**
 * Provide the editor inlay hint model upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_inlay_hint_model_upsert(
    UmiEditorInlayHintModel *model,
    const UmiEditorInlayHintItem *item);
/**
 * Remove editor inlay hint model while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_editor_inlay_hint_model_remove(
    UmiEditorInlayHintModel *model,
    const char *item_id);
/**
 * Provide the editor inlay hint model set kind visible operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_inlay_hint_model_set_kind_visible(
    UmiEditorInlayHintModel *model,
    UmiEditorInlayHintKind kind,
    int visible);
/**
 * Provide the editor inlay hint model sort operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_inlay_hint_model_sort(UmiEditorInlayHintModel *model);
/**
 * Find editor inlay hint model while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_editor_inlay_hint_model_at(
    const UmiEditorInlayHintModel *model,
    size_t index,
    UmiEditorInlayHintItem *out_item);
/**
 * Find editor inlay hint model visible while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_editor_inlay_hint_model_visible_at(
    const UmiEditorInlayHintModel *model,
    size_t visible_index,
    UmiEditorInlayHintItem *out_item);
/**
 * Provide the editor inlay hint model snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_inlay_hint_model_snapshot(
    const UmiEditorInlayHintModel *model,
    UmiEditorInlayHintSnapshot *out_snapshot);
/**
 * Return the number of records represented by editor inlay hint model without changing
 * their state.
 */
size_t umi_editor_inlay_hint_model_count(const UmiEditorInlayHintModel *model);
/**
 * Provide the editor inlay hint model revision operation used by this module and its
 * client applications.
 */
uint64_t umi_editor_inlay_hint_model_revision(const UmiEditorInlayHintModel *model);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_INLAY_HINT_MODEL_H */
