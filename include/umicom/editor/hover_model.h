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

typedef enum UmiEditorHoverContentKind {
    UMI_EDITOR_HOVER_PLAIN_TEXT = 1,
    UMI_EDITOR_HOVER_MARKDOWN = 2,
    UMI_EDITOR_HOVER_SOURCE_CODE = 3
} UmiEditorHoverContentKind;

typedef enum UmiEditorHoverResolutionState {
    UMI_EDITOR_HOVER_PENDING = 0,
    UMI_EDITOR_HOVER_READY = 1,
    UMI_EDITOR_HOVER_FAILED = 2
} UmiEditorHoverResolutionState;

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

typedef struct UmiEditorHoverModel UmiEditorHoverModel;

UmiStatus umi_editor_hover_model_create(UmiEditorHoverModel **out_model);
void umi_editor_hover_model_destroy(UmiEditorHoverModel *model);
UmiStatus umi_editor_hover_model_clear(UmiEditorHoverModel *model);
UmiStatus umi_editor_hover_model_upsert(
    UmiEditorHoverModel *model,
    const UmiEditorHoverSection *section);
UmiStatus umi_editor_hover_model_remove(
    UmiEditorHoverModel *model,
    const char *section_id);
UmiStatus umi_editor_hover_model_sort(UmiEditorHoverModel *model);
UmiStatus umi_editor_hover_model_set_active(
    UmiEditorHoverModel *model,
    const char *section_id);
UmiStatus umi_editor_hover_model_set_pinned(
    UmiEditorHoverModel *model,
    int pinned);
UmiStatus umi_editor_hover_model_at(
    const UmiEditorHoverModel *model,
    size_t index,
    UmiEditorHoverSection *out_section);
UmiStatus umi_editor_hover_model_visible_at(
    const UmiEditorHoverModel *model,
    size_t visible_index,
    UmiEditorHoverSection *out_section);
UmiStatus umi_editor_hover_model_active(
    const UmiEditorHoverModel *model,
    UmiEditorHoverSection *out_section);
UmiStatus umi_editor_hover_model_snapshot(
    const UmiEditorHoverModel *model,
    UmiEditorHoverSnapshot *out_snapshot);
size_t umi_editor_hover_model_count(const UmiEditorHoverModel *model);
uint64_t umi_editor_hover_model_revision(const UmiEditorHoverModel *model);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_HOVER_MODEL_H */
