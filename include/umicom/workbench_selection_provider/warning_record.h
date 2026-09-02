/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/warning_record.h
 *
 * PURPOSE:
 *   Define the reusable provider warning record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_WARNING_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_WARNING_RECORD_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection provider warning record data shared with callers of
 * this public contract.
 */
typedef struct UmiWorkbenchSelectionProviderWarningRecord {
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
} UmiWorkbenchSelectionProviderWarningRecord;

/**
 * Initialise workbench selection provider warning record from caller-provided values so
 * later operations receive a known state.
 */
void umi_workbench_selection_provider_warning_record_init(
    UmiWorkbenchSelectionProviderWarningRecord *record,
    const char *record_id);
/**
 * Check that workbench selection provider warning record satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_selection_provider_warning_record_validate(
    const UmiWorkbenchSelectionProviderWarningRecord *record);
/**
 * Provide the workbench selection provider warning record set provider operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_warning_record_set_provider(
    UmiWorkbenchSelectionProviderWarningRecord *record,
    const char *provider_id);
/**
 * Provide the workbench selection provider warning record set source operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_warning_record_set_source(
    UmiWorkbenchSelectionProviderWarningRecord *record,
    const char *source_id);
/**
 * Provide the workbench selection provider warning record set subject operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_warning_record_set_subject(
    UmiWorkbenchSelectionProviderWarningRecord *record,
    const char *subject_id);
/**
 * Provide the workbench selection provider warning record set related operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_warning_record_set_related(
    UmiWorkbenchSelectionProviderWarningRecord *record,
    const char *related_id);
/**
 * Provide the workbench selection provider warning record set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_warning_record_set_group(
    UmiWorkbenchSelectionProviderWarningRecord *record,
    const char *group_id);
/**
 * Provide the workbench selection provider warning record set description operation used
 * by this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_warning_record_set_description(
    UmiWorkbenchSelectionProviderWarningRecord *record,
    const char *description);
/**
 * Provide the workbench selection provider warning record hash operation used by this
 * module and its client applications.
 */
uint64_t umi_workbench_selection_provider_warning_record_hash(
    const UmiWorkbenchSelectionProviderWarningRecord *record);
/**
 * Provide the workbench selection provider warning record touch operation used by this
 * module and its client applications.
 */
void umi_workbench_selection_provider_warning_record_touch(
    UmiWorkbenchSelectionProviderWarningRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
