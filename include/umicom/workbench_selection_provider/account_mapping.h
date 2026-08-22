/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/account_mapping.h
 *
 * PURPOSE:
 *   Define the reusable account model-to-selection mapping contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_ACCOUNT_MAPPING_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_ACCOUNT_MAPPING_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionProviderAccountMapping {
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
} UmiWorkbenchSelectionProviderAccountMapping;

void umi_workbench_selection_provider_account_mapping_init(
    UmiWorkbenchSelectionProviderAccountMapping *record,
    const char *record_id);
UmiStatus umi_workbench_selection_provider_account_mapping_validate(
    const UmiWorkbenchSelectionProviderAccountMapping *record);
UmiStatus umi_workbench_selection_provider_account_mapping_set_provider(
    UmiWorkbenchSelectionProviderAccountMapping *record,
    const char *provider_id);
UmiStatus umi_workbench_selection_provider_account_mapping_set_source(
    UmiWorkbenchSelectionProviderAccountMapping *record,
    const char *source_id);
UmiStatus umi_workbench_selection_provider_account_mapping_set_subject(
    UmiWorkbenchSelectionProviderAccountMapping *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_provider_account_mapping_set_related(
    UmiWorkbenchSelectionProviderAccountMapping *record,
    const char *related_id);
UmiStatus umi_workbench_selection_provider_account_mapping_set_group(
    UmiWorkbenchSelectionProviderAccountMapping *record,
    const char *group_id);
UmiStatus umi_workbench_selection_provider_account_mapping_set_description(
    UmiWorkbenchSelectionProviderAccountMapping *record,
    const char *description);
uint64_t umi_workbench_selection_provider_account_mapping_hash(
    const UmiWorkbenchSelectionProviderAccountMapping *record);
void umi_workbench_selection_provider_account_mapping_touch(
    UmiWorkbenchSelectionProviderAccountMapping *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
