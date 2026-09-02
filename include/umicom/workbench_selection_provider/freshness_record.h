/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/freshness_record.h
 *
 * PURPOSE:
 *   Define the reusable provider freshness record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_FRESHNESS_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_FRESHNESS_RECORD_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection provider freshness record data shared with callers of
 * this public contract.
 */
typedef struct UmiWorkbenchSelectionProviderFreshnessRecord {
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
} UmiWorkbenchSelectionProviderFreshnessRecord;

/**
 * Initialise workbench selection provider freshness record from caller-provided values so
 * later operations receive a known state.
 */
void umi_workbench_selection_provider_freshness_record_init(
    UmiWorkbenchSelectionProviderFreshnessRecord *record,
    const char *record_id);
/**
 * Check that workbench selection provider freshness record satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_selection_provider_freshness_record_validate(
    const UmiWorkbenchSelectionProviderFreshnessRecord *record);
/**
 * Provide the workbench selection provider freshness record set provider operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_freshness_record_set_provider(
    UmiWorkbenchSelectionProviderFreshnessRecord *record,
    const char *provider_id);
/**
 * Provide the workbench selection provider freshness record set source operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_freshness_record_set_source(
    UmiWorkbenchSelectionProviderFreshnessRecord *record,
    const char *source_id);
/**
 * Provide the workbench selection provider freshness record set subject operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_freshness_record_set_subject(
    UmiWorkbenchSelectionProviderFreshnessRecord *record,
    const char *subject_id);
/**
 * Provide the workbench selection provider freshness record set related operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_freshness_record_set_related(
    UmiWorkbenchSelectionProviderFreshnessRecord *record,
    const char *related_id);
/**
 * Provide the workbench selection provider freshness record set group operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_freshness_record_set_group(
    UmiWorkbenchSelectionProviderFreshnessRecord *record,
    const char *group_id);
/**
 * Provide the workbench selection provider freshness record set description operation used
 * by this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_freshness_record_set_description(
    UmiWorkbenchSelectionProviderFreshnessRecord *record,
    const char *description);
/**
 * Provide the workbench selection provider freshness record hash operation used by this
 * module and its client applications.
 */
uint64_t umi_workbench_selection_provider_freshness_record_hash(
    const UmiWorkbenchSelectionProviderFreshnessRecord *record);
/**
 * Provide the workbench selection provider freshness record touch operation used by this
 * module and its client applications.
 */
void umi_workbench_selection_provider_freshness_record_touch(
    UmiWorkbenchSelectionProviderFreshnessRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
