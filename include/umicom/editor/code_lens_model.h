/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/code_lens_model.h
 *
 * PURPOSE:
 *   Model provider-neutral CodeLens titles, commands, resolution state and
 *   visibility for native analysers, LSP providers, tests and AI services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_CODE_LENS_MODEL_H
#define UMICOM_EDITOR_CODE_LENS_MODEL_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/editor/source_location.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_CODE_LENS_MODEL_API_VERSION 1U
#define UMI_EDITOR_CODE_LENS_ID_CAPACITY 128U
#define UMI_EDITOR_CODE_LENS_TEXT_CAPACITY 512U
#define UMI_EDITOR_CODE_LENS_COMMAND_CAPACITY 128U
#define UMI_EDITOR_CODE_LENS_PROVIDER_CAPACITY 128U

typedef enum UmiEditorCodeLensKind {
    UMI_EDITOR_CODE_LENS_REFERENCE = 1,
    UMI_EDITOR_CODE_LENS_IMPLEMENTATION = 2,
    UMI_EDITOR_CODE_LENS_TEST = 3,
    UMI_EDITOR_CODE_LENS_DEBUG = 4,
    UMI_EDITOR_CODE_LENS_HISTORY = 5,
    UMI_EDITOR_CODE_LENS_CUSTOM = 6
} UmiEditorCodeLensKind;

typedef enum UmiEditorCodeLensResolutionState {
    UMI_EDITOR_CODE_LENS_UNRESOLVED = 0,
    UMI_EDITOR_CODE_LENS_RESOLVED = 1,
    UMI_EDITOR_CODE_LENS_FAILED = 2
} UmiEditorCodeLensResolutionState;

typedef struct UmiEditorCodeLensItem {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_EDITOR_CODE_LENS_ID_CAPACITY];
    char provider_id[UMI_EDITOR_CODE_LENS_PROVIDER_CAPACITY];
    char title[UMI_EDITOR_CODE_LENS_TEXT_CAPACITY];
    char command_id[UMI_EDITOR_CODE_LENS_COMMAND_CAPACITY];
    char argument[UMI_EDITOR_CODE_LENS_TEXT_CAPACITY];
    UmiEditorSourceLocation location;
    UmiEditorCodeLensKind kind;
    UmiEditorCodeLensResolutionState resolution_state;
    int enabled;
    int visible;
} UmiEditorCodeLensItem;

typedef struct UmiEditorCodeLensSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t item_count;
    size_t visible_item_count;
    size_t resolved_item_count;
    size_t unresolved_item_count;
    uint64_t revision;
} UmiEditorCodeLensSnapshot;

typedef struct UmiEditorCodeLensModel UmiEditorCodeLensModel;

UmiStatus umi_editor_code_lens_model_create(UmiEditorCodeLensModel **out_model);
void umi_editor_code_lens_model_destroy(UmiEditorCodeLensModel *model);
UmiStatus umi_editor_code_lens_model_clear(UmiEditorCodeLensModel *model);
UmiStatus umi_editor_code_lens_model_upsert(
    UmiEditorCodeLensModel *model,
    const UmiEditorCodeLensItem *item);
UmiStatus umi_editor_code_lens_model_remove(
    UmiEditorCodeLensModel *model,
    const char *item_id);
UmiStatus umi_editor_code_lens_model_set_kind_visible(
    UmiEditorCodeLensModel *model,
    UmiEditorCodeLensKind kind,
    int visible);
UmiStatus umi_editor_code_lens_model_sort(UmiEditorCodeLensModel *model);
UmiStatus umi_editor_code_lens_model_at(
    const UmiEditorCodeLensModel *model,
    size_t index,
    UmiEditorCodeLensItem *out_item);
UmiStatus umi_editor_code_lens_model_visible_at(
    const UmiEditorCodeLensModel *model,
    size_t visible_index,
    UmiEditorCodeLensItem *out_item);
UmiStatus umi_editor_code_lens_model_snapshot(
    const UmiEditorCodeLensModel *model,
    UmiEditorCodeLensSnapshot *out_snapshot);
size_t umi_editor_code_lens_model_count(const UmiEditorCodeLensModel *model);
uint64_t umi_editor_code_lens_model_revision(const UmiEditorCodeLensModel *model);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_CODE_LENS_MODEL_H */
