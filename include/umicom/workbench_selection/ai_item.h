/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/ai_item.h
 *
 * PURPOSE:
 *   Define the reusable structured AI message/evidence item contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_AI_ITEM_H
#define UMICOM_WORKBENCH_SELECTION_AI_ITEM_H

#include "umicom/workbench_selection/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection ai item data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchSelectionAiItem {
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
} UmiWorkbenchSelectionAiItem;

/**
 * Initialise workbench selection ai item from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_selection_ai_item_init(
    UmiWorkbenchSelectionAiItem *record,
    const char *record_id);
/**
 * Check that workbench selection ai item satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_workbench_selection_ai_item_validate(
    const UmiWorkbenchSelectionAiItem *record);
/**
 * Provide the workbench selection ai item set source operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_selection_ai_item_set_source(
    UmiWorkbenchSelectionAiItem *record,
    const char *source_id);
/**
 * Provide the workbench selection ai item set subject operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_selection_ai_item_set_subject(
    UmiWorkbenchSelectionAiItem *record,
    const char *subject_id);
/**
 * Provide the workbench selection ai item set secondary operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_selection_ai_item_set_secondary(
    UmiWorkbenchSelectionAiItem *record,
    const char *secondary_id);
/**
 * Provide the workbench selection ai item set group operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_selection_ai_item_set_group(
    UmiWorkbenchSelectionAiItem *record,
    const char *group_id);
/**
 * Provide the workbench selection ai item set label operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_selection_ai_item_set_label(
    UmiWorkbenchSelectionAiItem *record,
    const char *label);
/**
 * Provide the workbench selection ai item hash operation used by this module and its
 * client applications.
 */
uint64_t umi_workbench_selection_ai_item_hash(
    const UmiWorkbenchSelectionAiItem *record);
/**
 * Provide the workbench selection ai item touch operation used by this module and its
 * client applications.
 */
void umi_workbench_selection_ai_item_touch(
    UmiWorkbenchSelectionAiItem *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
