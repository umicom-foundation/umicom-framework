/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/health_record.h
 *
 * PURPOSE:
 *   Define the reusable provider health record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_HEALTH_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_HEALTH_RECORD_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection provider health record data shared with callers of
 * this public contract.
 */
typedef struct UmiWorkbenchSelectionProviderHealthRecord {
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
} UmiWorkbenchSelectionProviderHealthRecord;

/**
 * Initialise workbench selection provider health record from caller-provided values so
 * later operations receive a known state.
 */
void umi_workbench_selection_provider_health_record_init(
    UmiWorkbenchSelectionProviderHealthRecord *record,
    const char *record_id);
/**
 * Check that workbench selection provider health record satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_selection_provider_health_record_validate(
    const UmiWorkbenchSelectionProviderHealthRecord *record);
/**
 * Provide the workbench selection provider health record set provider operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_health_record_set_provider(
    UmiWorkbenchSelectionProviderHealthRecord *record,
    const char *provider_id);
/**
 * Provide the workbench selection provider health record set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_health_record_set_source(
    UmiWorkbenchSelectionProviderHealthRecord *record,
    const char *source_id);
/**
 * Provide the workbench selection provider health record set subject operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_health_record_set_subject(
    UmiWorkbenchSelectionProviderHealthRecord *record,
    const char *subject_id);
/**
 * Provide the workbench selection provider health record set related operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_health_record_set_related(
    UmiWorkbenchSelectionProviderHealthRecord *record,
    const char *related_id);
/**
 * Provide the workbench selection provider health record set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_health_record_set_group(
    UmiWorkbenchSelectionProviderHealthRecord *record,
    const char *group_id);
/**
 * Provide the workbench selection provider health record set description operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_health_record_set_description(
    UmiWorkbenchSelectionProviderHealthRecord *record,
    const char *description);
/**
 * Provide the workbench selection provider health record hash operation used by this
 * module and its client applications.
 */
uint64_t umi_workbench_selection_provider_health_record_hash(
    const UmiWorkbenchSelectionProviderHealthRecord *record);
/**
 * Provide the workbench selection provider health record touch operation used by this
 * module and its client applications.
 */
void umi_workbench_selection_provider_health_record_touch(
    UmiWorkbenchSelectionProviderHealthRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
