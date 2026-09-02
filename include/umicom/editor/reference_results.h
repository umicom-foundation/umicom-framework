/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/reference_results.h
 *
 * PURPOSE:
 *   Group, filter, collapse and traverse reference results by source document
 *   independently of any application shell or graphical toolkit.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_REFERENCE_RESULTS_H
#define UMICOM_EDITOR_REFERENCE_RESULTS_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/editor/navigation_result.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_REFERENCE_RESULTS_API_VERSION 1U
#define UMI_EDITOR_REFERENCE_FILTER_CAPACITY 256U

/**
 * Represent the editor reference group data shared with callers of this public contract.
 */
typedef struct UmiEditorReferenceGroup {
    uint32_t struct_size;
    uint32_t api_version;
    char uri[UMI_EDITOR_SOURCE_URI_CAPACITY];
    char label[UMI_EDITOR_SOURCE_LABEL_CAPACITY];
    size_t result_count;
    size_t visible_result_count;
    int expanded;
} UmiEditorReferenceGroup;

/**
 * Represent the editor reference entry data shared with callers of this public contract.
 */
typedef struct UmiEditorReferenceEntry {
    uint32_t struct_size;
    uint32_t api_version;
    UmiEditorNavigationResult result;
    size_t group_index;
    int selected;
    int visible;
} UmiEditorReferenceEntry;

/**
 * Represent the editor reference results snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorReferenceResultsSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t group_count;
    size_t result_count;
    size_t visible_result_count;
    size_t selected_index;
    char filter[UMI_EDITOR_REFERENCE_FILTER_CAPACITY];
    uint64_t revision;
    int has_selection;
} UmiEditorReferenceResultsSnapshot;

/**
 * Represent the editor reference results data shared with callers of this public contract.
 */
typedef struct UmiEditorReferenceResults UmiEditorReferenceResults;

/**
 * Initialise editor reference results from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_reference_results_create(
    UmiEditorReferenceResults **out_results);
/**
 * Release or reset state held by editor reference results so the same storage can be
 * reused safely.
 */
void umi_editor_reference_results_destroy(UmiEditorReferenceResults *results);
/**
 * Read editor reference results into validated module state and return a status when input
 * cannot be used.
 */
UmiStatus umi_editor_reference_results_load(
    UmiEditorReferenceResults *results,
    const UmiEditorNavigationResultSet *navigation_results);
/**
 * Provide the editor reference results set filter operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_reference_results_set_filter(
    UmiEditorReferenceResults *results,
    const char *filter);
/**
 * Provide the editor reference results set group expanded operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_reference_results_set_group_expanded(
    UmiEditorReferenceResults *results,
    size_t group_index,
    int expanded);
/**
 * Provide the editor reference results select operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_reference_results_select(
    UmiEditorReferenceResults *results,
    size_t result_index);
/**
 * Provide the editor reference results select next operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_reference_results_select_next(
    UmiEditorReferenceResults *results,
    int wrap);
/**
 * Provide the editor reference results select previous operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_reference_results_select_previous(
    UmiEditorReferenceResults *results,
    int wrap);
/**
 * Find editor reference results group while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_editor_reference_results_group_at(
    const UmiEditorReferenceResults *results,
    size_t group_index,
    UmiEditorReferenceGroup *out_group);
/**
 * Find editor reference results entry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_editor_reference_results_entry_at(
    const UmiEditorReferenceResults *results,
    size_t result_index,
    UmiEditorReferenceEntry *out_entry);
/**
 * Find editor reference results visible entry while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiStatus umi_editor_reference_results_visible_entry_at(
    const UmiEditorReferenceResults *results,
    size_t visible_index,
    UmiEditorReferenceEntry *out_entry);
/**
 * Find editor reference results while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_editor_reference_results_selected(
    const UmiEditorReferenceResults *results,
    UmiEditorReferenceEntry *out_entry);
/**
 * Provide the editor reference results snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_reference_results_snapshot(
    const UmiEditorReferenceResults *results,
    UmiEditorReferenceResultsSnapshot *out_snapshot);
/**
 * Return the number of records represented by editor reference results group without
 * changing their state.
 */
size_t umi_editor_reference_results_group_count(
    const UmiEditorReferenceResults *results);
/**
 * Return the number of records represented by editor reference results without changing
 * their state.
 */
size_t umi_editor_reference_results_count(
    const UmiEditorReferenceResults *results);
/**
 * Return the number of records represented by editor reference results visible without
 * changing their state.
 */
size_t umi_editor_reference_results_visible_count(
    const UmiEditorReferenceResults *results);
/**
 * Provide the editor reference results revision operation used by this module and its
 * client applications.
 */
uint64_t umi_editor_reference_results_revision(
    const UmiEditorReferenceResults *results);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_REFERENCE_RESULTS_H */
