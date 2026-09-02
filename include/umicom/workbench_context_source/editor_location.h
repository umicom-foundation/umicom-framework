/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/editor_location.h
 *
 * PURPOSE:
 *   Define the reusable editor location snapshot contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_EDITOR_LOCATION_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_EDITOR_LOCATION_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context source editor location data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextSourceEditorLocation {
    uint32_t structure_size;
    char record_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char source_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char panel_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char subject_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char label[UMI_WORKBENCH_CONTEXT_SOURCE_TEXT_CAPACITY];
    UmiWorkbenchContextSourceKind source_kind;
    UmiWorkbenchContextSourceTrigger trigger;
    UmiWorkbenchContextSourceState state;
    UmiContextKind context_kind;
    uint64_t flags;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchContextSourceEditorLocation;

/**
 * Initialise workbench context source editor location from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_source_editor_location_init(
    UmiWorkbenchContextSourceEditorLocation *record,
    const char *record_id);
/**
 * Check that workbench context source editor location satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_context_source_editor_location_validate(
    const UmiWorkbenchContextSourceEditorLocation *record);
/**
 * Provide the workbench context source editor location set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_editor_location_set_source(
    UmiWorkbenchContextSourceEditorLocation *record,
    const char *source_id);
/**
 * Provide the workbench context source editor location set panel operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_editor_location_set_panel(
    UmiWorkbenchContextSourceEditorLocation *record,
    const char *panel_id);
/**
 * Provide the workbench context source editor location set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_editor_location_set_subject(
    UmiWorkbenchContextSourceEditorLocation *record,
    const char *subject_id);
/**
 * Provide the workbench context source editor location set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_editor_location_set_group(
    UmiWorkbenchContextSourceEditorLocation *record,
    const char *group_id);
/**
 * Provide the workbench context source editor location set label operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_editor_location_set_label(
    UmiWorkbenchContextSourceEditorLocation *record,
    const char *label);
/**
 * Provide the workbench context source editor location hash operation used by this module
 * and its client applications.
 */
uint64_t umi_workbench_context_source_editor_location_hash(
    const UmiWorkbenchContextSourceEditorLocation *record);
/**
 * Provide the workbench context source editor location touch operation used by this module
 * and its client applications.
 */
void umi_workbench_context_source_editor_location_touch(
    UmiWorkbenchContextSourceEditorLocation *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
