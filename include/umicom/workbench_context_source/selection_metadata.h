/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/selection_metadata.h
 *
 * PURPOSE:
 *   Define the reusable selection metadata contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_SELECTION_METADATA_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_SELECTION_METADATA_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context source selection metadata data shared with callers of
 * this public contract.
 */
typedef struct UmiWorkbenchContextSourceSelectionMetadata {
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
} UmiWorkbenchContextSourceSelectionMetadata;

/**
 * Initialise workbench context source selection metadata from caller-provided values so
 * later operations receive a known state.
 */
void umi_workbench_context_source_selection_metadata_init(
    UmiWorkbenchContextSourceSelectionMetadata *record,
    const char *record_id);
/**
 * Check that workbench context source selection metadata satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_context_source_selection_metadata_validate(
    const UmiWorkbenchContextSourceSelectionMetadata *record);
/**
 * Provide the workbench context source selection metadata set source operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_context_source_selection_metadata_set_source(
    UmiWorkbenchContextSourceSelectionMetadata *record,
    const char *source_id);
/**
 * Provide the workbench context source selection metadata set panel operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_selection_metadata_set_panel(
    UmiWorkbenchContextSourceSelectionMetadata *record,
    const char *panel_id);
/**
 * Provide the workbench context source selection metadata set subject operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_context_source_selection_metadata_set_subject(
    UmiWorkbenchContextSourceSelectionMetadata *record,
    const char *subject_id);
/**
 * Provide the workbench context source selection metadata set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_selection_metadata_set_group(
    UmiWorkbenchContextSourceSelectionMetadata *record,
    const char *group_id);
/**
 * Provide the workbench context source selection metadata set label operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_selection_metadata_set_label(
    UmiWorkbenchContextSourceSelectionMetadata *record,
    const char *label);
/**
 * Provide the workbench context source selection metadata hash operation used by this
 * module and its client applications.
 */
uint64_t umi_workbench_context_source_selection_metadata_hash(
    const UmiWorkbenchContextSourceSelectionMetadata *record);
/**
 * Provide the workbench context source selection metadata touch operation used by this
 * module and its client applications.
 */
void umi_workbench_context_source_selection_metadata_touch(
    UmiWorkbenchContextSourceSelectionMetadata *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
