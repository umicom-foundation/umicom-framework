/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/error_record.h
 *
 * PURPOSE:
 *   Define the reusable provider error record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_ERROR_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_ERROR_RECORD_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection provider error record data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchSelectionProviderErrorRecord {
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
} UmiWorkbenchSelectionProviderErrorRecord;

/**
 * Initialise workbench selection provider error record from caller-provided values so
 * later operations receive a known state.
 */
void umi_workbench_selection_provider_error_record_init(
    UmiWorkbenchSelectionProviderErrorRecord *record,
    const char *record_id);
/**
 * Check that workbench selection provider error record satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_selection_provider_error_record_validate(
    const UmiWorkbenchSelectionProviderErrorRecord *record);
/**
 * Provide the workbench selection provider error record set provider operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_error_record_set_provider(
    UmiWorkbenchSelectionProviderErrorRecord *record,
    const char *provider_id);
/**
 * Provide the workbench selection provider error record set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_error_record_set_source(
    UmiWorkbenchSelectionProviderErrorRecord *record,
    const char *source_id);
/**
 * Provide the workbench selection provider error record set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_error_record_set_subject(
    UmiWorkbenchSelectionProviderErrorRecord *record,
    const char *subject_id);
/**
 * Provide the workbench selection provider error record set related operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_error_record_set_related(
    UmiWorkbenchSelectionProviderErrorRecord *record,
    const char *related_id);
/**
 * Provide the workbench selection provider error record set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_error_record_set_group(
    UmiWorkbenchSelectionProviderErrorRecord *record,
    const char *group_id);
/**
 * Provide the workbench selection provider error record set description operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_error_record_set_description(
    UmiWorkbenchSelectionProviderErrorRecord *record,
    const char *description);
/**
 * Provide the workbench selection provider error record hash operation used by this module
 * and its client applications.
 */
uint64_t umi_workbench_selection_provider_error_record_hash(
    const UmiWorkbenchSelectionProviderErrorRecord *record);
/**
 * Provide the workbench selection provider error record touch operation used by this
 * module and its client applications.
 */
void umi_workbench_selection_provider_error_record_touch(
    UmiWorkbenchSelectionProviderErrorRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
