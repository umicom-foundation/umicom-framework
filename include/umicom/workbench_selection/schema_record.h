/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/schema_record.h
 *
 * PURPOSE:
 *   Define the reusable selection schema record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_SCHEMA_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_SCHEMA_RECORD_H
#include "umicom/workbench_selection/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection schema record data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchSelectionSchemaRecord {
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
} UmiWorkbenchSelectionSchemaRecord;

/**
 * Initialise workbench selection schema record from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_selection_schema_record_init(
    UmiWorkbenchSelectionSchemaRecord *record,
    const char *record_id);
/**
 * Check that workbench selection schema record satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_selection_schema_record_validate(
    const UmiWorkbenchSelectionSchemaRecord *record);
/**
 * Provide the workbench selection schema record set source operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_schema_record_set_source(
    UmiWorkbenchSelectionSchemaRecord *record,
    const char *source_id);
/**
 * Provide the workbench selection schema record set subject operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_schema_record_set_subject(
    UmiWorkbenchSelectionSchemaRecord *record,
    const char *subject_id);
/**
 * Provide the workbench selection schema record set related operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_schema_record_set_related(
    UmiWorkbenchSelectionSchemaRecord *record,
    const char *related_id);
/**
 * Provide the workbench selection schema record set group operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_schema_record_set_group(
    UmiWorkbenchSelectionSchemaRecord *record,
    const char *group_id);
/**
 * Provide the workbench selection schema record set description operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_schema_record_set_description(
    UmiWorkbenchSelectionSchemaRecord *record,
    const char *description);
/**
 * Provide the workbench selection schema record hash operation used by this module and its
 * client applications.
 */
uint64_t umi_workbench_selection_schema_record_hash(
    const UmiWorkbenchSelectionSchemaRecord *record);
/**
 * Provide the workbench selection schema record touch operation used by this module and
 * its client applications.
 */
void umi_workbench_selection_schema_record_touch(
    UmiWorkbenchSelectionSchemaRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
