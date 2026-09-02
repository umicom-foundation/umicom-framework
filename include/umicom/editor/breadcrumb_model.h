/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/breadcrumb_model.h
 *
 * PURPOSE:
 *   Compose file-path and enclosing-symbol breadcrumb segments from Framework
 *   source locations and symbol indexes without depending on a GUI toolkit.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_BREADCRUMB_MODEL_H
#define UMICOM_EDITOR_BREADCRUMB_MODEL_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/editor/symbol_index.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_BREADCRUMB_API_VERSION 1U
#define UMI_EDITOR_BREADCRUMB_SEGMENT_CAPACITY 64U
#define UMI_EDITOR_BREADCRUMB_ID_CAPACITY 512U

/**
 * List the named editor breadcrumb segment kind values accepted by this public contract.
 */
typedef enum UmiEditorBreadcrumbSegmentKind {
    UMI_EDITOR_BREADCRUMB_WORKSPACE = 1,
    UMI_EDITOR_BREADCRUMB_FOLDER = 2,
    UMI_EDITOR_BREADCRUMB_FILE = 3,
    UMI_EDITOR_BREADCRUMB_SYMBOL = 4
} UmiEditorBreadcrumbSegmentKind;

/**
 * Represent the editor breadcrumb segment data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorBreadcrumbSegment {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_EDITOR_BREADCRUMB_ID_CAPACITY];
    char label[UMI_EDITOR_SYMBOL_NAME_CAPACITY];
    UmiEditorBreadcrumbSegmentKind kind;
    UmiEditorIndexedSymbolKind symbol_kind;
    UmiEditorSourceLocation location;
    size_t depth;
    size_t sibling_count;
    int active;
} UmiEditorBreadcrumbSegment;

/**
 * Represent the editor breadcrumb model data shared with callers of this public contract.
 */
typedef struct UmiEditorBreadcrumbModel UmiEditorBreadcrumbModel;

/**
 * Initialise editor breadcrumb model from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_breadcrumb_model_create(
    UmiEditorBreadcrumbModel **out_model);
/**
 * Release or reset state held by editor breadcrumb model so the same storage can be reused
 * safely.
 */
void umi_editor_breadcrumb_model_destroy(UmiEditorBreadcrumbModel *model);
/**
 * Provide the editor breadcrumb model build operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_breadcrumb_model_build(
    UmiEditorBreadcrumbModel *model,
    const UmiEditorSymbolIndex *symbol_index,
    const char *workspace_label,
    const UmiEditorSourceLocation *location);
/**
 * Provide the editor breadcrumb model set active operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_breadcrumb_model_set_active(
    UmiEditorBreadcrumbModel *model,
    size_t index);
/**
 * Find editor breadcrumb model while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_editor_breadcrumb_model_at(
    const UmiEditorBreadcrumbModel *model,
    size_t index,
    UmiEditorBreadcrumbSegment *out_segment);
/**
 * Return the number of records represented by editor breadcrumb model without changing
 * their state.
 */
size_t umi_editor_breadcrumb_model_count(const UmiEditorBreadcrumbModel *model);
/**
 * Provide the editor breadcrumb model active index operation used by this module and its
 * client applications.
 */
size_t umi_editor_breadcrumb_model_active_index(
    const UmiEditorBreadcrumbModel *model);
/**
 * Provide the editor breadcrumb model revision operation used by this module and its
 * client applications.
 */
uint64_t umi_editor_breadcrumb_model_revision(
    const UmiEditorBreadcrumbModel *model);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_BREADCRUMB_MODEL_H */
