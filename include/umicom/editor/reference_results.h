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

typedef struct UmiEditorReferenceGroup {
    uint32_t struct_size;
    uint32_t api_version;
    char uri[UMI_EDITOR_SOURCE_URI_CAPACITY];
    char label[UMI_EDITOR_SOURCE_LABEL_CAPACITY];
    size_t result_count;
    size_t visible_result_count;
    int expanded;
} UmiEditorReferenceGroup;

typedef struct UmiEditorReferenceEntry {
    uint32_t struct_size;
    uint32_t api_version;
    UmiEditorNavigationResult result;
    size_t group_index;
    int selected;
    int visible;
} UmiEditorReferenceEntry;

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

typedef struct UmiEditorReferenceResults UmiEditorReferenceResults;

UmiStatus umi_editor_reference_results_create(
    UmiEditorReferenceResults **out_results);
void umi_editor_reference_results_destroy(UmiEditorReferenceResults *results);
UmiStatus umi_editor_reference_results_load(
    UmiEditorReferenceResults *results,
    const UmiEditorNavigationResultSet *navigation_results);
UmiStatus umi_editor_reference_results_set_filter(
    UmiEditorReferenceResults *results,
    const char *filter);
UmiStatus umi_editor_reference_results_set_group_expanded(
    UmiEditorReferenceResults *results,
    size_t group_index,
    int expanded);
UmiStatus umi_editor_reference_results_select(
    UmiEditorReferenceResults *results,
    size_t result_index);
UmiStatus umi_editor_reference_results_select_next(
    UmiEditorReferenceResults *results,
    int wrap);
UmiStatus umi_editor_reference_results_select_previous(
    UmiEditorReferenceResults *results,
    int wrap);
UmiStatus umi_editor_reference_results_group_at(
    const UmiEditorReferenceResults *results,
    size_t group_index,
    UmiEditorReferenceGroup *out_group);
UmiStatus umi_editor_reference_results_entry_at(
    const UmiEditorReferenceResults *results,
    size_t result_index,
    UmiEditorReferenceEntry *out_entry);
UmiStatus umi_editor_reference_results_visible_entry_at(
    const UmiEditorReferenceResults *results,
    size_t visible_index,
    UmiEditorReferenceEntry *out_entry);
UmiStatus umi_editor_reference_results_selected(
    const UmiEditorReferenceResults *results,
    UmiEditorReferenceEntry *out_entry);
UmiStatus umi_editor_reference_results_snapshot(
    const UmiEditorReferenceResults *results,
    UmiEditorReferenceResultsSnapshot *out_snapshot);
size_t umi_editor_reference_results_group_count(
    const UmiEditorReferenceResults *results);
size_t umi_editor_reference_results_count(
    const UmiEditorReferenceResults *results);
size_t umi_editor_reference_results_visible_count(
    const UmiEditorReferenceResults *results);
uint64_t umi_editor_reference_results_revision(
    const UmiEditorReferenceResults *results);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_REFERENCE_RESULTS_H */
