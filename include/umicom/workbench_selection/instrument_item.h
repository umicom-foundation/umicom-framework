/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/instrument_item.h
 *
 * PURPOSE:
 *   Define the reusable structured instrument item contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_INSTRUMENT_ITEM_H
#define UMICOM_WORKBENCH_SELECTION_INSTRUMENT_ITEM_H

#include "umicom/workbench_selection/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection instrument item data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchSelectionInstrumentItem {
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
} UmiWorkbenchSelectionInstrumentItem;

/**
 * Initialise workbench selection instrument item from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_selection_instrument_item_init(
    UmiWorkbenchSelectionInstrumentItem *record,
    const char *record_id);
/**
 * Check that workbench selection instrument item satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_selection_instrument_item_validate(
    const UmiWorkbenchSelectionInstrumentItem *record);
/**
 * Provide the workbench selection instrument item set source operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_instrument_item_set_source(
    UmiWorkbenchSelectionInstrumentItem *record,
    const char *source_id);
/**
 * Provide the workbench selection instrument item set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_instrument_item_set_subject(
    UmiWorkbenchSelectionInstrumentItem *record,
    const char *subject_id);
/**
 * Provide the workbench selection instrument item set secondary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_instrument_item_set_secondary(
    UmiWorkbenchSelectionInstrumentItem *record,
    const char *secondary_id);
/**
 * Provide the workbench selection instrument item set group operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_instrument_item_set_group(
    UmiWorkbenchSelectionInstrumentItem *record,
    const char *group_id);
/**
 * Provide the workbench selection instrument item set label operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_instrument_item_set_label(
    UmiWorkbenchSelectionInstrumentItem *record,
    const char *label);
/**
 * Provide the workbench selection instrument item hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_selection_instrument_item_hash(
    const UmiWorkbenchSelectionInstrumentItem *record);
/**
 * Provide the workbench selection instrument item touch operation used by this module and
 * its client applications.
 */
void umi_workbench_selection_instrument_item_touch(
    UmiWorkbenchSelectionInstrumentItem *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
