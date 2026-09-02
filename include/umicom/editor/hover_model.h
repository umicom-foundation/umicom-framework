/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/hover_model.h
 *
 * PURPOSE:
 *   Model rich, provider-neutral hover sections, selection and pinning for
 *   native analysers, language servers, AI services and future frontends.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_HOVER_MODEL_H
#define UMICOM_EDITOR_HOVER_MODEL_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/editor/source_location.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_HOVER_MODEL_API_VERSION 1U
#define UMI_EDITOR_HOVER_ID_CAPACITY 128U
#define UMI_EDITOR_HOVER_PROVIDER_CAPACITY 128U
#define UMI_EDITOR_HOVER_TITLE_CAPACITY 256U
#define UMI_EDITOR_HOVER_LANGUAGE_CAPACITY 64U
#define UMI_EDITOR_HOVER_CONTENT_CAPACITY 4096U

/**
 * List the named editor hover content kind values accepted by this public contract.
 */
typedef enum UmiEditorHoverContentKind {
    UMI_EDITOR_HOVER_PLAIN_TEXT = 1,
    UMI_EDITOR_HOVER_MARKDOWN = 2,
    UMI_EDITOR_HOVER_SOURCE_CODE = 3
} UmiEditorHoverContentKind;

/**
 * List the named editor hover resolution state values accepted by this public contract.
 */
typedef enum UmiEditorHoverResolutionState {
    UMI_EDITOR_HOVER_PENDING = 0,
    UMI_EDITOR_HOVER_READY = 1,
    UMI_EDITOR_HOVER_FAILED = 2
} UmiEditorHoverResolutionState;

/**
 * Represent the editor hover section data shared with callers of this public contract.
 */
typedef struct UmiEditorHoverSection {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_EDITOR_HOVER_ID_CAPACITY];
    char provider_id[UMI_EDITOR_HOVER_PROVIDER_CAPACITY];
    char title[UMI_EDITOR_HOVER_TITLE_CAPACITY];
    char language_id[UMI_EDITOR_HOVER_LANGUAGE_CAPACITY];
    char content[UMI_EDITOR_HOVER_CONTENT_CAPACITY];
    UmiEditorSourceLocation location;
    UmiEditorHoverContentKind content_kind;
    UmiEditorHoverResolutionState resolution_state;
    int32_t priority;
    int trusted;
    int visible;
} UmiEditorHoverSection;

/**
 * Represent the editor hover snapshot data shared with callers of this public contract.
 */
typedef struct UmiEditorHoverSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t section_count;
    size_t visible_section_count;
    size_t ready_section_count;
    size_t pending_section_count;
    uint64_t revision;
    char active_section_id[UMI_EDITOR_HOVER_ID_CAPACITY];
    int pinned;
} UmiEditorHoverSnapshot;

/**
 * Represent the editor hover model data shared with callers of this public contract.
 */
typedef struct UmiEditorHoverModel UmiEditorHoverModel;

/**
 * Initialise editor hover model from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_editor_hover_model_create(UmiEditorHoverModel **out_model);
/**
 * Release or reset state held by editor hover model so the same storage can be reused
 * safely.
 */
void umi_editor_hover_model_destroy(UmiEditorHoverModel *model);
/**
 * Release or reset state held by editor hover model so the same storage can be reused
 * safely.
 */
UmiStatus umi_editor_hover_model_clear(UmiEditorHoverModel *model);
/**
 * Provide the editor hover model upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_hover_model_upsert(
    UmiEditorHoverModel *model,
    const UmiEditorHoverSection *section);
/**
 * Remove editor hover model while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_editor_hover_model_remove(
    UmiEditorHoverModel *model,
    const char *section_id);
/**
 * Provide the editor hover model sort operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_hover_model_sort(UmiEditorHoverModel *model);
/**
 * Provide the editor hover model set active operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_hover_model_set_active(
    UmiEditorHoverModel *model,
    const char *section_id);
/**
 * Provide the editor hover model set pinned operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_hover_model_set_pinned(
    UmiEditorHoverModel *model,
    int pinned);
/**
 * Find editor hover model while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_editor_hover_model_at(
    const UmiEditorHoverModel *model,
    size_t index,
    UmiEditorHoverSection *out_section);
/**
 * Find editor hover model visible while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_editor_hover_model_visible_at(
    const UmiEditorHoverModel *model,
    size_t visible_index,
    UmiEditorHoverSection *out_section);
/**
 * Provide the editor hover model active operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_hover_model_active(
    const UmiEditorHoverModel *model,
    UmiEditorHoverSection *out_section);
/**
 * Provide the editor hover model snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_hover_model_snapshot(
    const UmiEditorHoverModel *model,
    UmiEditorHoverSnapshot *out_snapshot);
/**
 * Return the number of records represented by editor hover model without changing their
 * state.
 */
size_t umi_editor_hover_model_count(const UmiEditorHoverModel *model);
/**
 * Provide the editor hover model revision operation used by this module and its client
 * applications.
 */
uint64_t umi_editor_hover_model_revision(const UmiEditorHoverModel *model);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_HOVER_MODEL_H */
