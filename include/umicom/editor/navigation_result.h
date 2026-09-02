/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/navigation_result.h
 *
 * PURPOSE:
 *   Aggregate, rank, de-duplicate and select provider-neutral source results
 *   returned by native parsers, language servers, indexes or AI assistants.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * List the named editor navigation query kind values accepted by this public contract.
 */
typedef enum UmiEditorNavigationQueryKind {
    UMI_EDITOR_NAVIGATION_QUERY_NONE = 0,
    UMI_EDITOR_NAVIGATION_QUERY_DEFINITION = 1,
    UMI_EDITOR_NAVIGATION_QUERY_DECLARATION = 2,
    UMI_EDITOR_NAVIGATION_QUERY_REFERENCE = 3,
    UMI_EDITOR_NAVIGATION_QUERY_IMPLEMENTATION = 4,
    UMI_EDITOR_NAVIGATION_QUERY_TYPE_DEFINITION = 5,
    UMI_EDITOR_NAVIGATION_QUERY_DOCUMENT_SYMBOL = 6,
    UMI_EDITOR_NAVIGATION_QUERY_WORKSPACE_SYMBOL = 7
} UmiEditorNavigationQueryKind;

/**
 * Represent the editor navigation result data shared with callers of this public contract.
 */
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

/**
 * Represent the editor navigation result set data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorNavigationResultSet
    UmiEditorNavigationResultSet;

/**
 * Initialise editor navigation result set from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_navigation_result_set_create(
    UmiEditorNavigationQueryKind query_kind,
    UmiEditorNavigationResultSet **out_result_set);
/**
 * Release or reset state held by editor navigation result set so the same storage can be
 * reused safely.
 */
void umi_editor_navigation_result_set_destroy(
    UmiEditorNavigationResultSet *result_set);
/**
 * Release or reset state held by editor navigation result set so the same storage can be
 * reused safely.
 */
UmiStatus umi_editor_navigation_result_set_reset(
    UmiEditorNavigationResultSet *result_set,
    UmiEditorNavigationQueryKind query_kind);
/**
 * Provide the editor navigation result set upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_navigation_result_set_upsert(
    UmiEditorNavigationResultSet *result_set,
    const UmiEditorNavigationResult *result);
/**
 * Provide the editor navigation result set sort operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_navigation_result_set_sort(
    UmiEditorNavigationResultSet *result_set);
/**
 * Provide the editor navigation result set select operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_navigation_result_set_select(
    UmiEditorNavigationResultSet *result_set,
    size_t index);
/**
 * Find editor navigation result set while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_editor_navigation_result_set_selected(
    const UmiEditorNavigationResultSet *result_set,
    UmiEditorNavigationResult *out_result);
/**
 * Find editor navigation result set while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_editor_navigation_result_set_at(
    const UmiEditorNavigationResultSet *result_set,
    size_t index,
    UmiEditorNavigationResult *out_result);
/**
 * Return the number of records represented by editor navigation result set without
 * changing their state.
 */
size_t umi_editor_navigation_result_set_count(
    const UmiEditorNavigationResultSet *result_set);
/**
 * Provide the editor navigation result set selected index operation used by this module
 * and its client applications.
 */
size_t umi_editor_navigation_result_set_selected_index(
    const UmiEditorNavigationResultSet *result_set);
/**
 * Provide the editor navigation result set query kind operation used by this module and
 * its client applications.
 */
UmiEditorNavigationQueryKind umi_editor_navigation_result_set_query_kind(
    const UmiEditorNavigationResultSet *result_set);
/**
 * Provide the editor navigation result set revision operation used by this module and its
 * client applications.
 */
uint64_t umi_editor_navigation_result_set_revision(
    const UmiEditorNavigationResultSet *result_set);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_NAVIGATION_RESULT_H */
