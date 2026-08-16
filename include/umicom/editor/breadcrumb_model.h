/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/breadcrumb_model.h
 *
 * PURPOSE:
 *   Compose file-path and enclosing-symbol breadcrumb segments from Framework
 *   source locations and symbol indexes without depending on a GUI toolkit.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef enum UmiEditorBreadcrumbSegmentKind {
    UMI_EDITOR_BREADCRUMB_WORKSPACE = 1,
    UMI_EDITOR_BREADCRUMB_FOLDER = 2,
    UMI_EDITOR_BREADCRUMB_FILE = 3,
    UMI_EDITOR_BREADCRUMB_SYMBOL = 4
} UmiEditorBreadcrumbSegmentKind;

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

typedef struct UmiEditorBreadcrumbModel UmiEditorBreadcrumbModel;

UmiStatus umi_editor_breadcrumb_model_create(
    UmiEditorBreadcrumbModel **out_model);
void umi_editor_breadcrumb_model_destroy(UmiEditorBreadcrumbModel *model);
UmiStatus umi_editor_breadcrumb_model_build(
    UmiEditorBreadcrumbModel *model,
    const UmiEditorSymbolIndex *symbol_index,
    const char *workspace_label,
    const UmiEditorSourceLocation *location);
UmiStatus umi_editor_breadcrumb_model_set_active(
    UmiEditorBreadcrumbModel *model,
    size_t index);
UmiStatus umi_editor_breadcrumb_model_at(
    const UmiEditorBreadcrumbModel *model,
    size_t index,
    UmiEditorBreadcrumbSegment *out_segment);
size_t umi_editor_breadcrumb_model_count(const UmiEditorBreadcrumbModel *model);
size_t umi_editor_breadcrumb_model_active_index(
    const UmiEditorBreadcrumbModel *model);
uint64_t umi_editor_breadcrumb_model_revision(
    const UmiEditorBreadcrumbModel *model);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_BREADCRUMB_MODEL_H */
