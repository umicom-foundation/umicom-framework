/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/synchronization_record.h
 *
 * PURPOSE:
 *   Define the reusable provider synchronisation record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_SYNCHRONIZATION_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_SYNCHRONIZATION_RECORD_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection provider synchronization record data shared with
 * callers of this public contract.
 */
typedef struct UmiWorkbenchSelectionProviderSynchronizationRecord {
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
} UmiWorkbenchSelectionProviderSynchronizationRecord;

/**
 * Initialise workbench selection provider synchronization record from caller-provided
 * values so later operations receive a known state.
 */
void umi_workbench_selection_provider_synchronization_record_init(
    UmiWorkbenchSelectionProviderSynchronizationRecord *record,
    const char *record_id);
/**
 * Check that workbench selection provider synchronization record satisfies its contract
 * before another service relies on it.
 */
UmiStatus umi_workbench_selection_provider_synchronization_record_validate(
    const UmiWorkbenchSelectionProviderSynchronizationRecord *record);
/**
 * Provide the workbench selection provider synchronization record set provider operation
 * used by this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_synchronization_record_set_provider(
    UmiWorkbenchSelectionProviderSynchronizationRecord *record,
    const char *provider_id);
/**
 * Provide the workbench selection provider synchronization record set source operation
 * used by this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_synchronization_record_set_source(
    UmiWorkbenchSelectionProviderSynchronizationRecord *record,
    const char *source_id);
/**
 * Provide the workbench selection provider synchronization record set subject operation
 * used by this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_synchronization_record_set_subject(
    UmiWorkbenchSelectionProviderSynchronizationRecord *record,
    const char *subject_id);
/**
 * Provide the workbench selection provider synchronization record set related operation
 * used by this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_synchronization_record_set_related(
    UmiWorkbenchSelectionProviderSynchronizationRecord *record,
    const char *related_id);
/**
 * Provide the workbench selection provider synchronization record set group operation used
 * by this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_synchronization_record_set_group(
    UmiWorkbenchSelectionProviderSynchronizationRecord *record,
    const char *group_id);
/**
 * Provide the workbench selection provider synchronization record set description
 * operation used by this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_synchronization_record_set_description(
    UmiWorkbenchSelectionProviderSynchronizationRecord *record,
    const char *description);
/**
 * Provide the workbench selection provider synchronization record hash operation used by
 * this module and its client applications.
 */
uint64_t umi_workbench_selection_provider_synchronization_record_hash(
    const UmiWorkbenchSelectionProviderSynchronizationRecord *record);
/**
 * Provide the workbench selection provider synchronization record touch operation used by
 * this module and its client applications.
 */
void umi_workbench_selection_provider_synchronization_record_touch(
    UmiWorkbenchSelectionProviderSynchronizationRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
