/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/backpressure_record.h
 *
 * PURPOSE:
 *   Define the reusable provider backpressure record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_BACKPRESSURE_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_BACKPRESSURE_RECORD_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection provider backpressure record data shared with callers
 * of this public contract.
 */
typedef struct UmiWorkbenchSelectionProviderBackpressureRecord {
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
} UmiWorkbenchSelectionProviderBackpressureRecord;

/**
 * Initialise workbench selection provider backpressure record from caller-provided values
 * so later operations receive a known state.
 */
void umi_workbench_selection_provider_backpressure_record_init(
    UmiWorkbenchSelectionProviderBackpressureRecord *record,
    const char *record_id);
/**
 * Check that workbench selection provider backpressure record satisfies its contract
 * before another service relies on it.
 */
UmiStatus umi_workbench_selection_provider_backpressure_record_validate(
    const UmiWorkbenchSelectionProviderBackpressureRecord *record);
/**
 * Provide the workbench selection provider backpressure record set provider operation used
 * by this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_backpressure_record_set_provider(
    UmiWorkbenchSelectionProviderBackpressureRecord *record,
    const char *provider_id);
/**
 * Provide the workbench selection provider backpressure record set source operation used
 * by this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_backpressure_record_set_source(
    UmiWorkbenchSelectionProviderBackpressureRecord *record,
    const char *source_id);
/**
 * Provide the workbench selection provider backpressure record set subject operation used
 * by this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_backpressure_record_set_subject(
    UmiWorkbenchSelectionProviderBackpressureRecord *record,
    const char *subject_id);
/**
 * Provide the workbench selection provider backpressure record set related operation used
 * by this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_backpressure_record_set_related(
    UmiWorkbenchSelectionProviderBackpressureRecord *record,
    const char *related_id);
/**
 * Provide the workbench selection provider backpressure record set group operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_backpressure_record_set_group(
    UmiWorkbenchSelectionProviderBackpressureRecord *record,
    const char *group_id);
/**
 * Provide the workbench selection provider backpressure record set description operation
 * used by this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_backpressure_record_set_description(
    UmiWorkbenchSelectionProviderBackpressureRecord *record,
    const char *description);
/**
 * Provide the workbench selection provider backpressure record hash operation used by this
 * module and its client applications.
 */
uint64_t umi_workbench_selection_provider_backpressure_record_hash(
    const UmiWorkbenchSelectionProviderBackpressureRecord *record);
/**
 * Provide the workbench selection provider backpressure record touch operation used by
 * this module and its client applications.
 */
void umi_workbench_selection_provider_backpressure_record_touch(
    UmiWorkbenchSelectionProviderBackpressureRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
