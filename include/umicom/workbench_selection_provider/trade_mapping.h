/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/trade_mapping.h
 *
 * PURPOSE:
 *   Define the reusable trade model-to-selection mapping contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_TRADE_MAPPING_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_TRADE_MAPPING_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection provider trade mapping data shared with callers of
 * this public contract.
 */
typedef struct UmiWorkbenchSelectionProviderTradeMapping {
    uint32_t structure_size;
    char record_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char provider_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char source_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char subject_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char related_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char description[UMI_WORKBENCH_SELECTION_PROVIDER_TEXT_CAPACITY];
    UmiWorkbenchSelectionProviderKind provider_kind;
    UmiWorkbenchSelectionProviderRuntimeState state;
    UmiWorkbenchSelectionKind selection_kind;
    UmiContextKind context_kind;
    uint64_t flags;
    uint64_t count;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchSelectionProviderTradeMapping;

/**
 * Initialise workbench selection provider trade mapping from caller-provided values so
 * later operations receive a known state.
 */
void umi_workbench_selection_provider_trade_mapping_init(
    UmiWorkbenchSelectionProviderTradeMapping *record,
    const char *record_id);
/**
 * Check that workbench selection provider trade mapping satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_selection_provider_trade_mapping_validate(
    const UmiWorkbenchSelectionProviderTradeMapping *record);
/**
 * Provide the workbench selection provider trade mapping set provider operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_trade_mapping_set_provider(
    UmiWorkbenchSelectionProviderTradeMapping *record,
    const char *provider_id);
/**
 * Provide the workbench selection provider trade mapping set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_trade_mapping_set_source(
    UmiWorkbenchSelectionProviderTradeMapping *record,
    const char *source_id);
/**
 * Provide the workbench selection provider trade mapping set subject operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_trade_mapping_set_subject(
    UmiWorkbenchSelectionProviderTradeMapping *record,
    const char *subject_id);
/**
 * Provide the workbench selection provider trade mapping set related operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_trade_mapping_set_related(
    UmiWorkbenchSelectionProviderTradeMapping *record,
    const char *related_id);
/**
 * Provide the workbench selection provider trade mapping set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_trade_mapping_set_group(
    UmiWorkbenchSelectionProviderTradeMapping *record,
    const char *group_id);
/**
 * Provide the workbench selection provider trade mapping set description operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_trade_mapping_set_description(
    UmiWorkbenchSelectionProviderTradeMapping *record,
    const char *description);
/**
 * Provide the workbench selection provider trade mapping hash operation used by this
 * module and its client applications.
 */
uint64_t umi_workbench_selection_provider_trade_mapping_hash(
    const UmiWorkbenchSelectionProviderTradeMapping *record);
/**
 * Provide the workbench selection provider trade mapping touch operation used by this
 * module and its client applications.
 */
void umi_workbench_selection_provider_trade_mapping_touch(
    UmiWorkbenchSelectionProviderTradeMapping *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
