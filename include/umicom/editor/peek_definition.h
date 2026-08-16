/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/peek_definition.h
 *
 * PURPOSE:
 *   Define a reusable, toolkit-neutral peek-definition model with multiple
 *   ranked targets, keyboard traversal, pinning and source-anchor ownership.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_PEEK_DEFINITION_H
#define UMICOM_EDITOR_PEEK_DEFINITION_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/editor/navigation_result.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_PEEK_DEFINITION_API_VERSION 1U

typedef struct UmiEditorPeekDefinitionSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t result_count;
    size_t active_index;
    uint64_t revision;
    int visible;
    int pinned;
    int has_anchor;
    int can_select_previous;
    int can_select_next;
} UmiEditorPeekDefinitionSnapshot;

typedef struct UmiEditorPeekDefinitionModel UmiEditorPeekDefinitionModel;

UmiStatus umi_editor_peek_definition_model_create(
    UmiEditorPeekDefinitionModel **out_model);
void umi_editor_peek_definition_model_destroy(
    UmiEditorPeekDefinitionModel *model);
UmiStatus umi_editor_peek_definition_model_load(
    UmiEditorPeekDefinitionModel *model,
    const UmiEditorSourceLocation *anchor,
    const UmiEditorNavigationResultSet *results);
UmiStatus umi_editor_peek_definition_model_show(
    UmiEditorPeekDefinitionModel *model);
UmiStatus umi_editor_peek_definition_model_hide(
    UmiEditorPeekDefinitionModel *model);
UmiStatus umi_editor_peek_definition_model_set_pinned(
    UmiEditorPeekDefinitionModel *model,
    int pinned);
UmiStatus umi_editor_peek_definition_model_select(
    UmiEditorPeekDefinitionModel *model,
    size_t index);
UmiStatus umi_editor_peek_definition_model_select_next(
    UmiEditorPeekDefinitionModel *model,
    int wrap);
UmiStatus umi_editor_peek_definition_model_select_previous(
    UmiEditorPeekDefinitionModel *model,
    int wrap);
UmiStatus umi_editor_peek_definition_model_anchor(
    const UmiEditorPeekDefinitionModel *model,
    UmiEditorSourceLocation *out_anchor);
UmiStatus umi_editor_peek_definition_model_at(
    const UmiEditorPeekDefinitionModel *model,
    size_t index,
    UmiEditorNavigationResult *out_result);
UmiStatus umi_editor_peek_definition_model_active(
    const UmiEditorPeekDefinitionModel *model,
    UmiEditorNavigationResult *out_result);
UmiStatus umi_editor_peek_definition_model_snapshot(
    const UmiEditorPeekDefinitionModel *model,
    UmiEditorPeekDefinitionSnapshot *out_snapshot);
size_t umi_editor_peek_definition_model_count(
    const UmiEditorPeekDefinitionModel *model);
uint64_t umi_editor_peek_definition_model_revision(
    const UmiEditorPeekDefinitionModel *model);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_PEEK_DEFINITION_H */
