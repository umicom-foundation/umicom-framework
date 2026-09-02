/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/peek_definition.h
 *
 * PURPOSE:
 *   Define a reusable, toolkit-neutral peek-definition model with multiple
 *   ranked targets, keyboard traversal, pinning and source-anchor ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * Represent the editor peek definition snapshot data shared with callers of this public
 * contract.
 */
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

/**
 * Represent the editor peek definition model data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorPeekDefinitionModel UmiEditorPeekDefinitionModel;

/**
 * Initialise editor peek definition model from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_peek_definition_model_create(
    UmiEditorPeekDefinitionModel **out_model);
/**
 * Release or reset state held by editor peek definition model so the same storage can be
 * reused safely.
 */
void umi_editor_peek_definition_model_destroy(
    UmiEditorPeekDefinitionModel *model);
/**
 * Read editor peek definition model into validated module state and return a status when
 * input cannot be used.
 */
UmiStatus umi_editor_peek_definition_model_load(
    UmiEditorPeekDefinitionModel *model,
    const UmiEditorSourceLocation *anchor,
    const UmiEditorNavigationResultSet *results);
/**
 * Provide the editor peek definition model show operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_peek_definition_model_show(
    UmiEditorPeekDefinitionModel *model);
/**
 * Provide the editor peek definition model hide operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_peek_definition_model_hide(
    UmiEditorPeekDefinitionModel *model);
/**
 * Provide the editor peek definition model set pinned operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_peek_definition_model_set_pinned(
    UmiEditorPeekDefinitionModel *model,
    int pinned);
/**
 * Provide the editor peek definition model select operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_peek_definition_model_select(
    UmiEditorPeekDefinitionModel *model,
    size_t index);
/**
 * Provide the editor peek definition model select next operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_peek_definition_model_select_next(
    UmiEditorPeekDefinitionModel *model,
    int wrap);
/**
 * Provide the editor peek definition model select previous operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_peek_definition_model_select_previous(
    UmiEditorPeekDefinitionModel *model,
    int wrap);
/**
 * Provide the editor peek definition model anchor operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_peek_definition_model_anchor(
    const UmiEditorPeekDefinitionModel *model,
    UmiEditorSourceLocation *out_anchor);
/**
 * Find editor peek definition model while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_editor_peek_definition_model_at(
    const UmiEditorPeekDefinitionModel *model,
    size_t index,
    UmiEditorNavigationResult *out_result);
/**
 * Provide the editor peek definition model active operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_peek_definition_model_active(
    const UmiEditorPeekDefinitionModel *model,
    UmiEditorNavigationResult *out_result);
/**
 * Provide the editor peek definition model snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_peek_definition_model_snapshot(
    const UmiEditorPeekDefinitionModel *model,
    UmiEditorPeekDefinitionSnapshot *out_snapshot);
/**
 * Return the number of records represented by editor peek definition model without
 * changing their state.
 */
size_t umi_editor_peek_definition_model_count(
    const UmiEditorPeekDefinitionModel *model);
/**
 * Provide the editor peek definition model revision operation used by this module and its
 * client applications.
 */
uint64_t umi_editor_peek_definition_model_revision(
    const UmiEditorPeekDefinitionModel *model);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_PEEK_DEFINITION_H */
