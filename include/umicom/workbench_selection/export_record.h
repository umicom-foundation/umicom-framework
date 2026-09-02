/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/export_record.h
 *
 * PURPOSE:
 *   Define the reusable selection export record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_EXPORT_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_EXPORT_RECORD_H
#include "umicom/workbench_selection/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection export record data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchSelectionExportRecord {
    uint32_t structure_size;
    char record_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char source_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char subject_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char related_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char description[UMI_WORKBENCH_SELECTION_TEXT_CAPACITY];
    UmiWorkbenchSelectionKind selection_kind;
    UmiWorkbenchSelectionActivation activation;
    UmiWorkbenchSelectionState state;
    UmiContextKind context_kind;
    uint64_t flags;
    uint64_t count;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchSelectionExportRecord;

/**
 * Initialise workbench selection export record from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_selection_export_record_init(
    UmiWorkbenchSelectionExportRecord *record,
    const char *record_id);
/**
 * Check that workbench selection export record satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_selection_export_record_validate(
    const UmiWorkbenchSelectionExportRecord *record);
/**
 * Provide the workbench selection export record set source operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_export_record_set_source(
    UmiWorkbenchSelectionExportRecord *record,
    const char *source_id);
/**
 * Provide the workbench selection export record set subject operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_export_record_set_subject(
    UmiWorkbenchSelectionExportRecord *record,
    const char *subject_id);
/**
 * Provide the workbench selection export record set related operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_export_record_set_related(
    UmiWorkbenchSelectionExportRecord *record,
    const char *related_id);
/**
 * Provide the workbench selection export record set group operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_export_record_set_group(
    UmiWorkbenchSelectionExportRecord *record,
    const char *group_id);
/**
 * Provide the workbench selection export record set description operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_export_record_set_description(
    UmiWorkbenchSelectionExportRecord *record,
    const char *description);
/**
 * Provide the workbench selection export record hash operation used by this module and its
 * client applications.
 */
uint64_t umi_workbench_selection_export_record_hash(
    const UmiWorkbenchSelectionExportRecord *record);
/**
 * Provide the workbench selection export record touch operation used by this module and
 * its client applications.
 */
void umi_workbench_selection_export_record_touch(
    UmiWorkbenchSelectionExportRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
