/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/navigation_result.h
 *
 * PURPOSE:
 *   Aggregate, rank, de-duplicate and select provider-neutral source results
 *   returned by native parsers, language servers, indexes or AI assistants.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_NAVIGATION_RESULT_H
#define UMICOM_EDITOR_NAVIGATION_RESULT_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/editor/source_location.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_NAVIGATION_RESULT_API_VERSION 1U
#define UMI_EDITOR_NAVIGATION_PROVIDER_ID_CAPACITY 128U
#define UMI_EDITOR_NAVIGATION_DETAIL_CAPACITY 512U

typedef enum UmiEditorNavigationQueryKind {
    UMI_EDITOR_NAVIGATION_QUERY_DEFINITION = 1,
    UMI_EDITOR_NAVIGATION_QUERY_DECLARATION = 2,
    UMI_EDITOR_NAVIGATION_QUERY_REFERENCE = 3,
    UMI_EDITOR_NAVIGATION_QUERY_IMPLEMENTATION = 4,
    UMI_EDITOR_NAVIGATION_QUERY_TYPE_DEFINITION = 5,
    UMI_EDITOR_NAVIGATION_QUERY_DOCUMENT_SYMBOL = 6,
    UMI_EDITOR_NAVIGATION_QUERY_WORKSPACE_SYMBOL = 7
} UmiEditorNavigationQueryKind;

typedef struct UmiEditorNavigationResult {
    uint32_t struct_size;
    uint32_t api_version;
    char provider_id[UMI_EDITOR_NAVIGATION_PROVIDER_ID_CAPACITY];
    char detail[UMI_EDITOR_NAVIGATION_DETAIL_CAPACITY];
    UmiEditorSourceLocation location;
    uint64_t sequence;
    int32_t rank;
    int primary;
} UmiEditorNavigationResult;

typedef struct UmiEditorNavigationResultSet
    UmiEditorNavigationResultSet;

UmiStatus umi_editor_navigation_result_set_create(
    UmiEditorNavigationQueryKind query_kind,
    UmiEditorNavigationResultSet **out_result_set);
void umi_editor_navigation_result_set_destroy(
    UmiEditorNavigationResultSet *result_set);
UmiStatus umi_editor_navigation_result_set_reset(
    UmiEditorNavigationResultSet *result_set,
    UmiEditorNavigationQueryKind query_kind);
UmiStatus umi_editor_navigation_result_set_upsert(
    UmiEditorNavigationResultSet *result_set,
    const UmiEditorNavigationResult *result);
UmiStatus umi_editor_navigation_result_set_sort(
    UmiEditorNavigationResultSet *result_set);
UmiStatus umi_editor_navigation_result_set_select(
    UmiEditorNavigationResultSet *result_set,
    size_t index);
UmiStatus umi_editor_navigation_result_set_selected(
    const UmiEditorNavigationResultSet *result_set,
    UmiEditorNavigationResult *out_result);
UmiStatus umi_editor_navigation_result_set_at(
    const UmiEditorNavigationResultSet *result_set,
    size_t index,
    UmiEditorNavigationResult *out_result);
size_t umi_editor_navigation_result_set_count(
    const UmiEditorNavigationResultSet *result_set);
size_t umi_editor_navigation_result_set_selected_index(
    const UmiEditorNavigationResultSet *result_set);
UmiEditorNavigationQueryKind umi_editor_navigation_result_set_query_kind(
    const UmiEditorNavigationResultSet *result_set);
uint64_t umi_editor_navigation_result_set_revision(
    const UmiEditorNavigationResultSet *result_set);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_NAVIGATION_RESULT_H */
