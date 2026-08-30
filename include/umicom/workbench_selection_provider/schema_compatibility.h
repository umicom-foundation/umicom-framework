/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/schema_compatibility.h
 *
 * PURPOSE:
 *   Define the reusable provider schema compatibility record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_SCHEMA_COMPATIBILITY_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_SCHEMA_COMPATIBILITY_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionProviderSchemaCompatibility {
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
} UmiWorkbenchSelectionProviderSchemaCompatibility;

void umi_workbench_selection_provider_schema_compatibility_init(
    UmiWorkbenchSelectionProviderSchemaCompatibility *record,
    const char *record_id);
UmiStatus umi_workbench_selection_provider_schema_compatibility_validate(
    const UmiWorkbenchSelectionProviderSchemaCompatibility *record);
UmiStatus umi_workbench_selection_provider_schema_compatibility_set_provider(
    UmiWorkbenchSelectionProviderSchemaCompatibility *record,
    const char *provider_id);
UmiStatus umi_workbench_selection_provider_schema_compatibility_set_source(
    UmiWorkbenchSelectionProviderSchemaCompatibility *record,
    const char *source_id);
UmiStatus umi_workbench_selection_provider_schema_compatibility_set_subject(
    UmiWorkbenchSelectionProviderSchemaCompatibility *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_provider_schema_compatibility_set_related(
    UmiWorkbenchSelectionProviderSchemaCompatibility *record,
    const char *related_id);
UmiStatus umi_workbench_selection_provider_schema_compatibility_set_group(
    UmiWorkbenchSelectionProviderSchemaCompatibility *record,
    const char *group_id);
UmiStatus umi_workbench_selection_provider_schema_compatibility_set_description(
    UmiWorkbenchSelectionProviderSchemaCompatibility *record,
    const char *description);
uint64_t umi_workbench_selection_provider_schema_compatibility_hash(
    const UmiWorkbenchSelectionProviderSchemaCompatibility *record);
void umi_workbench_selection_provider_schema_compatibility_touch(
    UmiWorkbenchSelectionProviderSchemaCompatibility *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
