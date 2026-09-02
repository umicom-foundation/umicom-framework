/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/schema_record.h
 *
 * PURPOSE:
 *   Define the reusable provider schema record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_SCHEMA_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_SCHEMA_RECORD_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection provider schema record data shared with callers of
 * this public contract.
 */
typedef struct UmiWorkbenchSelectionProviderSchemaRecord {
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
} UmiWorkbenchSelectionProviderSchemaRecord;

/**
 * Initialise workbench selection provider schema record from caller-provided values so
 * later operations receive a known state.
 */
void umi_workbench_selection_provider_schema_record_init(
    UmiWorkbenchSelectionProviderSchemaRecord *record,
    const char *record_id);
/**
 * Check that workbench selection provider schema record satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_selection_provider_schema_record_validate(
    const UmiWorkbenchSelectionProviderSchemaRecord *record);
/**
 * Provide the workbench selection provider schema record set provider operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_schema_record_set_provider(
    UmiWorkbenchSelectionProviderSchemaRecord *record,
    const char *provider_id);
/**
 * Provide the workbench selection provider schema record set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_schema_record_set_source(
    UmiWorkbenchSelectionProviderSchemaRecord *record,
    const char *source_id);
/**
 * Provide the workbench selection provider schema record set subject operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_schema_record_set_subject(
    UmiWorkbenchSelectionProviderSchemaRecord *record,
    const char *subject_id);
/**
 * Provide the workbench selection provider schema record set related operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_schema_record_set_related(
    UmiWorkbenchSelectionProviderSchemaRecord *record,
    const char *related_id);
/**
 * Provide the workbench selection provider schema record set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_schema_record_set_group(
    UmiWorkbenchSelectionProviderSchemaRecord *record,
    const char *group_id);
/**
 * Provide the workbench selection provider schema record set description operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_schema_record_set_description(
    UmiWorkbenchSelectionProviderSchemaRecord *record,
    const char *description);
/**
 * Provide the workbench selection provider schema record hash operation used by this
 * module and its client applications.
 */
uint64_t umi_workbench_selection_provider_schema_record_hash(
    const UmiWorkbenchSelectionProviderSchemaRecord *record);
/**
 * Provide the workbench selection provider schema record touch operation used by this
 * module and its client applications.
 */
void umi_workbench_selection_provider_schema_record_touch(
    UmiWorkbenchSelectionProviderSchemaRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
