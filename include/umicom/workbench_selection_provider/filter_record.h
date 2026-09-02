/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/filter_record.h
 *
 * PURPOSE:
 *   Define the reusable provider filter record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_FILTER_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_FILTER_RECORD_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection provider filter record data shared with callers of
 * this public contract.
 */
typedef struct UmiWorkbenchSelectionProviderFilterRecord {
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
} UmiWorkbenchSelectionProviderFilterRecord;

/**
 * Initialise workbench selection provider filter record from caller-provided values so
 * later operations receive a known state.
 */
void umi_workbench_selection_provider_filter_record_init(
    UmiWorkbenchSelectionProviderFilterRecord *record,
    const char *record_id);
/**
 * Check that workbench selection provider filter record satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_selection_provider_filter_record_validate(
    const UmiWorkbenchSelectionProviderFilterRecord *record);
/**
 * Provide the workbench selection provider filter record set provider operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_filter_record_set_provider(
    UmiWorkbenchSelectionProviderFilterRecord *record,
    const char *provider_id);
/**
 * Provide the workbench selection provider filter record set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_filter_record_set_source(
    UmiWorkbenchSelectionProviderFilterRecord *record,
    const char *source_id);
/**
 * Provide the workbench selection provider filter record set subject operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_filter_record_set_subject(
    UmiWorkbenchSelectionProviderFilterRecord *record,
    const char *subject_id);
/**
 * Provide the workbench selection provider filter record set related operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_filter_record_set_related(
    UmiWorkbenchSelectionProviderFilterRecord *record,
    const char *related_id);
/**
 * Provide the workbench selection provider filter record set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_filter_record_set_group(
    UmiWorkbenchSelectionProviderFilterRecord *record,
    const char *group_id);
/**
 * Provide the workbench selection provider filter record set description operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_filter_record_set_description(
    UmiWorkbenchSelectionProviderFilterRecord *record,
    const char *description);
/**
 * Provide the workbench selection provider filter record hash operation used by this
 * module and its client applications.
 */
uint64_t umi_workbench_selection_provider_filter_record_hash(
    const UmiWorkbenchSelectionProviderFilterRecord *record);
/**
 * Provide the workbench selection provider filter record touch operation used by this
 * module and its client applications.
 */
void umi_workbench_selection_provider_filter_record_touch(
    UmiWorkbenchSelectionProviderFilterRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
