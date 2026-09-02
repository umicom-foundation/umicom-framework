/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/page_record.h
 *
 * PURPOSE:
 *   Define the reusable provider page record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_PAGE_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_PAGE_RECORD_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection provider page record data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchSelectionProviderPageRecord {
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
} UmiWorkbenchSelectionProviderPageRecord;

/**
 * Initialise workbench selection provider page record from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_selection_provider_page_record_init(
    UmiWorkbenchSelectionProviderPageRecord *record,
    const char *record_id);
/**
 * Check that workbench selection provider page record satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_selection_provider_page_record_validate(
    const UmiWorkbenchSelectionProviderPageRecord *record);
/**
 * Provide the workbench selection provider page record set provider operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_page_record_set_provider(
    UmiWorkbenchSelectionProviderPageRecord *record,
    const char *provider_id);
/**
 * Provide the workbench selection provider page record set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_page_record_set_source(
    UmiWorkbenchSelectionProviderPageRecord *record,
    const char *source_id);
/**
 * Provide the workbench selection provider page record set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_page_record_set_subject(
    UmiWorkbenchSelectionProviderPageRecord *record,
    const char *subject_id);
/**
 * Provide the workbench selection provider page record set related operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_page_record_set_related(
    UmiWorkbenchSelectionProviderPageRecord *record,
    const char *related_id);
/**
 * Provide the workbench selection provider page record set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_page_record_set_group(
    UmiWorkbenchSelectionProviderPageRecord *record,
    const char *group_id);
/**
 * Provide the workbench selection provider page record set description operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_page_record_set_description(
    UmiWorkbenchSelectionProviderPageRecord *record,
    const char *description);
/**
 * Provide the workbench selection provider page record hash operation used by this module
 * and its client applications.
 */
uint64_t umi_workbench_selection_provider_page_record_hash(
    const UmiWorkbenchSelectionProviderPageRecord *record);
/**
 * Provide the workbench selection provider page record touch operation used by this module
 * and its client applications.
 */
void umi_workbench_selection_provider_page_record_touch(
    UmiWorkbenchSelectionProviderPageRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
