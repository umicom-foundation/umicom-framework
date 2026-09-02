/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/projection_record.h
 *
 * PURPOSE:
 *   Define the reusable provider projection record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_PROJECTION_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_PROJECTION_RECORD_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection provider projection record data shared with callers of
 * this public contract.
 */
typedef struct UmiWorkbenchSelectionProviderProjectionRecord {
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
} UmiWorkbenchSelectionProviderProjectionRecord;

/**
 * Initialise workbench selection provider projection record from caller-provided values so
 * later operations receive a known state.
 */
void umi_workbench_selection_provider_projection_record_init(
    UmiWorkbenchSelectionProviderProjectionRecord *record,
    const char *record_id);
/**
 * Check that workbench selection provider projection record satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_selection_provider_projection_record_validate(
    const UmiWorkbenchSelectionProviderProjectionRecord *record);
/**
 * Provide the workbench selection provider projection record set provider operation used
 * by this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_projection_record_set_provider(
    UmiWorkbenchSelectionProviderProjectionRecord *record,
    const char *provider_id);
/**
 * Provide the workbench selection provider projection record set source operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_projection_record_set_source(
    UmiWorkbenchSelectionProviderProjectionRecord *record,
    const char *source_id);
/**
 * Provide the workbench selection provider projection record set subject operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_projection_record_set_subject(
    UmiWorkbenchSelectionProviderProjectionRecord *record,
    const char *subject_id);
/**
 * Provide the workbench selection provider projection record set related operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_projection_record_set_related(
    UmiWorkbenchSelectionProviderProjectionRecord *record,
    const char *related_id);
/**
 * Provide the workbench selection provider projection record set group operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_projection_record_set_group(
    UmiWorkbenchSelectionProviderProjectionRecord *record,
    const char *group_id);
/**
 * Provide the workbench selection provider projection record set description operation
 * used by this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_projection_record_set_description(
    UmiWorkbenchSelectionProviderProjectionRecord *record,
    const char *description);
/**
 * Provide the workbench selection provider projection record hash operation used by this
 * module and its client applications.
 */
uint64_t umi_workbench_selection_provider_projection_record_hash(
    const UmiWorkbenchSelectionProviderProjectionRecord *record);
/**
 * Provide the workbench selection provider projection record touch operation used by this
 * module and its client applications.
 */
void umi_workbench_selection_provider_projection_record_touch(
    UmiWorkbenchSelectionProviderProjectionRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
