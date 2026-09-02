/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/diagnostic_item.h
 *
 * PURPOSE:
 *   Define the reusable structured diagnostic item contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_DIAGNOSTIC_ITEM_H
#define UMICOM_WORKBENCH_SELECTION_DIAGNOSTIC_ITEM_H

#include "umicom/workbench_selection/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection diagnostic item data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchSelectionDiagnosticItem {
    uint32_t structure_size;
    char record_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char source_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char subject_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char secondary_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char label[UMI_WORKBENCH_SELECTION_TEXT_CAPACITY];
    UmiWorkbenchSelectionKind selection_kind;
    UmiWorkbenchSelectionActivation activation;
    UmiWorkbenchSelectionState state;
    UmiContextKind context_kind;
    uint64_t flags;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchSelectionDiagnosticItem;

/**
 * Initialise workbench selection diagnostic item from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_selection_diagnostic_item_init(
    UmiWorkbenchSelectionDiagnosticItem *record,
    const char *record_id);
/**
 * Check that workbench selection diagnostic item satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_selection_diagnostic_item_validate(
    const UmiWorkbenchSelectionDiagnosticItem *record);
/**
 * Provide the workbench selection diagnostic item set source operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_diagnostic_item_set_source(
    UmiWorkbenchSelectionDiagnosticItem *record,
    const char *source_id);
/**
 * Provide the workbench selection diagnostic item set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_diagnostic_item_set_subject(
    UmiWorkbenchSelectionDiagnosticItem *record,
    const char *subject_id);
/**
 * Provide the workbench selection diagnostic item set secondary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_diagnostic_item_set_secondary(
    UmiWorkbenchSelectionDiagnosticItem *record,
    const char *secondary_id);
/**
 * Provide the workbench selection diagnostic item set group operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_diagnostic_item_set_group(
    UmiWorkbenchSelectionDiagnosticItem *record,
    const char *group_id);
/**
 * Provide the workbench selection diagnostic item set label operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_diagnostic_item_set_label(
    UmiWorkbenchSelectionDiagnosticItem *record,
    const char *label);
/**
 * Provide the workbench selection diagnostic item hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_selection_diagnostic_item_hash(
    const UmiWorkbenchSelectionDiagnosticItem *record);
/**
 * Provide the workbench selection diagnostic item touch operation used by this module and
 * its client applications.
 */
void umi_workbench_selection_diagnostic_item_touch(
    UmiWorkbenchSelectionDiagnosticItem *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
