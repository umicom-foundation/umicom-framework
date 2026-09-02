/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/capability_record.h
 *
 * PURPOSE:
 *   Define the reusable provider capability record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_CAPABILITY_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_CAPABILITY_RECORD_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection provider capability record data shared with callers of
 * this public contract.
 */
typedef struct UmiWorkbenchSelectionProviderCapabilityRecord {
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
} UmiWorkbenchSelectionProviderCapabilityRecord;

/**
 * Initialise workbench selection provider capability record from caller-provided values so
 * later operations receive a known state.
 */
void umi_workbench_selection_provider_capability_record_init(
    UmiWorkbenchSelectionProviderCapabilityRecord *record,
    const char *record_id);
/**
 * Check that workbench selection provider capability record satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_selection_provider_capability_record_validate(
    const UmiWorkbenchSelectionProviderCapabilityRecord *record);
/**
 * Provide the workbench selection provider capability record set provider operation used
 * by this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_capability_record_set_provider(
    UmiWorkbenchSelectionProviderCapabilityRecord *record,
    const char *provider_id);
/**
 * Provide the workbench selection provider capability record set source operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_capability_record_set_source(
    UmiWorkbenchSelectionProviderCapabilityRecord *record,
    const char *source_id);
/**
 * Provide the workbench selection provider capability record set subject operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_capability_record_set_subject(
    UmiWorkbenchSelectionProviderCapabilityRecord *record,
    const char *subject_id);
/**
 * Provide the workbench selection provider capability record set related operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_capability_record_set_related(
    UmiWorkbenchSelectionProviderCapabilityRecord *record,
    const char *related_id);
/**
 * Provide the workbench selection provider capability record set group operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_capability_record_set_group(
    UmiWorkbenchSelectionProviderCapabilityRecord *record,
    const char *group_id);
/**
 * Provide the workbench selection provider capability record set description operation
 * used by this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_capability_record_set_description(
    UmiWorkbenchSelectionProviderCapabilityRecord *record,
    const char *description);
/**
 * Provide the workbench selection provider capability record hash operation used by this
 * module and its client applications.
 */
uint64_t umi_workbench_selection_provider_capability_record_hash(
    const UmiWorkbenchSelectionProviderCapabilityRecord *record);
/**
 * Provide the workbench selection provider capability record touch operation used by this
 * module and its client applications.
 */
void umi_workbench_selection_provider_capability_record_touch(
    UmiWorkbenchSelectionProviderCapabilityRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
