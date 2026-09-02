/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/selection.h
 *
 * PURPOSE:
 *   Define the canonical structured selection exchanged between view models and context source routing.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_SELECTION_H
#define UMICOM_WORKBENCH_SELECTION_SELECTION_H

#include "umicom/workbench_selection/field.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection data shared with callers of this public contract.
 */
typedef struct UmiWorkbenchSelection {
    uint32_t structure_size;
    char selection_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char application_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char panel_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char workspace_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char subject_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char secondary_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char path[UMI_WORKBENCH_SELECTION_PATH_CAPACITY];
    char display_text[UMI_WORKBENCH_SELECTION_TEXT_CAPACITY];
    char group_hint[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    UmiWorkbenchSelectionKind kind;
    UmiWorkbenchSelectionActivation activation;
    UmiWorkbenchSelectionState state;
    UmiContextKind context_kind;
    UmiWorkbenchSelectionField fields[UMI_WORKBENCH_SELECTION_MAX_FIELDS];
    size_t field_count;
    uint32_t line;
    uint32_t column;
    uint32_t selection_length;
    int64_t signed_value;
    uint64_t unsigned_value;
    bool boolean_value;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
    uint64_t content_hash;
} UmiWorkbenchSelection;

/**
 * Initialise workbench selection from caller-provided values so later operations receive a
 * known state.
 */
void umi_workbench_selection_init(
    UmiWorkbenchSelection *selection,
    UmiWorkbenchSelectionKind kind,
    UmiContextKind context_kind,
    const char *selection_id);
/**
 * Provide the workbench selection set origin operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_selection_set_origin(
    UmiWorkbenchSelection *selection,
    const char *application_id,
    const char *panel_id,
    const char *workspace_id);
/**
 * Provide the workbench selection set subject operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_selection_set_subject(
    UmiWorkbenchSelection *selection,
    const char *subject_id,
    const char *secondary_id);
/**
 * Provide the workbench selection set location operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_selection_set_location(
    UmiWorkbenchSelection *selection,
    const char *path,
    uint32_t line,
    uint32_t column,
    uint32_t selection_length);
/**
 * Provide the workbench selection set display text operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_selection_set_display_text(
    UmiWorkbenchSelection *selection,
    const char *display_text);
/**
 * Provide the workbench selection set group hint operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_selection_set_group_hint(
    UmiWorkbenchSelection *selection,
    const char *group_id);
/**
 * Provide the workbench selection add field operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_selection_add_field(
    UmiWorkbenchSelection *selection,
    const UmiWorkbenchSelectionField *field);
/**
 * Provide the workbench selection find field operation used by this module and its client
 * applications.
 */
const UmiWorkbenchSelectionField *umi_workbench_selection_find_field(
    const UmiWorkbenchSelection *selection,
    const char *name);
/**
 * Check that workbench selection satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_workbench_selection_validate(
    const UmiWorkbenchSelection *selection);
/**
 * Provide the workbench selection refresh hash operation used by this module and its
 * client applications.
 */
uint64_t umi_workbench_selection_refresh_hash(
    UmiWorkbenchSelection *selection);

#ifdef __cplusplus
}
#endif
#endif
