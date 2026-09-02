/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/tree_record.h
 *
 * PURPOSE:
 *   Define the reusable structured tree record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_TREE_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_TREE_RECORD_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection provider tree record data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchSelectionProviderTreeRecord {
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
} UmiWorkbenchSelectionProviderTreeRecord;

/**
 * Initialise workbench selection provider tree record from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_selection_provider_tree_record_init(
    UmiWorkbenchSelectionProviderTreeRecord *record,
    const char *record_id);
/**
 * Check that workbench selection provider tree record satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_selection_provider_tree_record_validate(
    const UmiWorkbenchSelectionProviderTreeRecord *record);
/**
 * Provide the workbench selection provider tree record set provider operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_tree_record_set_provider(
    UmiWorkbenchSelectionProviderTreeRecord *record,
    const char *provider_id);
/**
 * Provide the workbench selection provider tree record set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_tree_record_set_source(
    UmiWorkbenchSelectionProviderTreeRecord *record,
    const char *source_id);
/**
 * Provide the workbench selection provider tree record set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_tree_record_set_subject(
    UmiWorkbenchSelectionProviderTreeRecord *record,
    const char *subject_id);
/**
 * Provide the workbench selection provider tree record set related operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_tree_record_set_related(
    UmiWorkbenchSelectionProviderTreeRecord *record,
    const char *related_id);
/**
 * Provide the workbench selection provider tree record set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_tree_record_set_group(
    UmiWorkbenchSelectionProviderTreeRecord *record,
    const char *group_id);
/**
 * Provide the workbench selection provider tree record set description operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_tree_record_set_description(
    UmiWorkbenchSelectionProviderTreeRecord *record,
    const char *description);
/**
 * Provide the workbench selection provider tree record hash operation used by this module
 * and its client applications.
 */
uint64_t umi_workbench_selection_provider_tree_record_hash(
    const UmiWorkbenchSelectionProviderTreeRecord *record);
/**
 * Provide the workbench selection provider tree record touch operation used by this module
 * and its client applications.
 */
void umi_workbench_selection_provider_tree_record_touch(
    UmiWorkbenchSelectionProviderTreeRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
