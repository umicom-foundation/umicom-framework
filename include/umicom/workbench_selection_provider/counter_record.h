/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/counter_record.h
 *
 * PURPOSE:
 *   Define the reusable provider counter record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_COUNTER_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_COUNTER_RECORD_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection provider counter record data shared with callers of
 * this public contract.
 */
typedef struct UmiWorkbenchSelectionProviderCounterRecord {
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
} UmiWorkbenchSelectionProviderCounterRecord;

/**
 * Initialise workbench selection provider counter record from caller-provided values so
 * later operations receive a known state.
 */
void umi_workbench_selection_provider_counter_record_init(
    UmiWorkbenchSelectionProviderCounterRecord *record,
    const char *record_id);
/**
 * Check that workbench selection provider counter record satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_selection_provider_counter_record_validate(
    const UmiWorkbenchSelectionProviderCounterRecord *record);
/**
 * Provide the workbench selection provider counter record set provider operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_counter_record_set_provider(
    UmiWorkbenchSelectionProviderCounterRecord *record,
    const char *provider_id);
/**
 * Provide the workbench selection provider counter record set source operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_counter_record_set_source(
    UmiWorkbenchSelectionProviderCounterRecord *record,
    const char *source_id);
/**
 * Provide the workbench selection provider counter record set subject operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_counter_record_set_subject(
    UmiWorkbenchSelectionProviderCounterRecord *record,
    const char *subject_id);
/**
 * Provide the workbench selection provider counter record set related operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_counter_record_set_related(
    UmiWorkbenchSelectionProviderCounterRecord *record,
    const char *related_id);
/**
 * Provide the workbench selection provider counter record set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_counter_record_set_group(
    UmiWorkbenchSelectionProviderCounterRecord *record,
    const char *group_id);
/**
 * Provide the workbench selection provider counter record set description operation used
 * by this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_counter_record_set_description(
    UmiWorkbenchSelectionProviderCounterRecord *record,
    const char *description);
/**
 * Provide the workbench selection provider counter record hash operation used by this
 * module and its client applications.
 */
uint64_t umi_workbench_selection_provider_counter_record_hash(
    const UmiWorkbenchSelectionProviderCounterRecord *record);
/**
 * Provide the workbench selection provider counter record touch operation used by this
 * module and its client applications.
 */
void umi_workbench_selection_provider_counter_record_touch(
    UmiWorkbenchSelectionProviderCounterRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
