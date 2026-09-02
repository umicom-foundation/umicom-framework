/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/trade_item.h
 *
 * PURPOSE:
 *   Define the reusable structured trade item contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_TRADE_ITEM_H
#define UMICOM_WORKBENCH_SELECTION_TRADE_ITEM_H

#include "umicom/workbench_selection/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection trade item data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchSelectionTradeItem {
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
} UmiWorkbenchSelectionTradeItem;

/**
 * Initialise workbench selection trade item from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_selection_trade_item_init(
    UmiWorkbenchSelectionTradeItem *record,
    const char *record_id);
/**
 * Check that workbench selection trade item satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_workbench_selection_trade_item_validate(
    const UmiWorkbenchSelectionTradeItem *record);
/**
 * Provide the workbench selection trade item set source operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_selection_trade_item_set_source(
    UmiWorkbenchSelectionTradeItem *record,
    const char *source_id);
/**
 * Provide the workbench selection trade item set subject operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_selection_trade_item_set_subject(
    UmiWorkbenchSelectionTradeItem *record,
    const char *subject_id);
/**
 * Provide the workbench selection trade item set secondary operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_trade_item_set_secondary(
    UmiWorkbenchSelectionTradeItem *record,
    const char *secondary_id);
/**
 * Provide the workbench selection trade item set group operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_selection_trade_item_set_group(
    UmiWorkbenchSelectionTradeItem *record,
    const char *group_id);
/**
 * Provide the workbench selection trade item set label operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_selection_trade_item_set_label(
    UmiWorkbenchSelectionTradeItem *record,
    const char *label);
/**
 * Provide the workbench selection trade item hash operation used by this module and its
 * client applications.
 */
uint64_t umi_workbench_selection_trade_item_hash(
    const UmiWorkbenchSelectionTradeItem *record);
/**
 * Provide the workbench selection trade item touch operation used by this module and its
 * client applications.
 */
void umi_workbench_selection_trade_item_touch(
    UmiWorkbenchSelectionTradeItem *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
