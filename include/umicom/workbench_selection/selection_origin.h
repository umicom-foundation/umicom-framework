/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/selection_origin.h
 *
 * PURPOSE:
 *   Define the reusable selection origin contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_SELECTION_ORIGIN_H
#define UMICOM_WORKBENCH_SELECTION_SELECTION_ORIGIN_H

#include "umicom/workbench_selection/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection selection origin data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchSelectionSelectionOrigin {
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
} UmiWorkbenchSelectionSelectionOrigin;

/**
 * Initialise workbench selection selection origin from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_selection_selection_origin_init(
    UmiWorkbenchSelectionSelectionOrigin *record,
    const char *record_id);
/**
 * Check that workbench selection selection origin satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_selection_selection_origin_validate(
    const UmiWorkbenchSelectionSelectionOrigin *record);
/**
 * Provide the workbench selection selection origin set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_selection_origin_set_source(
    UmiWorkbenchSelectionSelectionOrigin *record,
    const char *source_id);
/**
 * Provide the workbench selection selection origin set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_selection_origin_set_subject(
    UmiWorkbenchSelectionSelectionOrigin *record,
    const char *subject_id);
/**
 * Provide the workbench selection selection origin set secondary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_selection_origin_set_secondary(
    UmiWorkbenchSelectionSelectionOrigin *record,
    const char *secondary_id);
/**
 * Provide the workbench selection selection origin set group operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_selection_origin_set_group(
    UmiWorkbenchSelectionSelectionOrigin *record,
    const char *group_id);
/**
 * Provide the workbench selection selection origin set label operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_selection_origin_set_label(
    UmiWorkbenchSelectionSelectionOrigin *record,
    const char *label);
/**
 * Provide the workbench selection selection origin hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_selection_selection_origin_hash(
    const UmiWorkbenchSelectionSelectionOrigin *record);
/**
 * Provide the workbench selection selection origin touch operation used by this module and
 * its client applications.
 */
void umi_workbench_selection_selection_origin_touch(
    UmiWorkbenchSelectionSelectionOrigin *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
