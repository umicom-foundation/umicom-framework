/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/test_mapping.h
 *
 * PURPOSE:
 *   Define the reusable test model-to-selection mapping contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_TEST_MAPPING_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_TEST_MAPPING_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionProviderTestMapping {
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
} UmiWorkbenchSelectionProviderTestMapping;

void umi_workbench_selection_provider_test_mapping_init(
    UmiWorkbenchSelectionProviderTestMapping *record,
    const char *record_id);
UmiStatus umi_workbench_selection_provider_test_mapping_validate(
    const UmiWorkbenchSelectionProviderTestMapping *record);
UmiStatus umi_workbench_selection_provider_test_mapping_set_provider(
    UmiWorkbenchSelectionProviderTestMapping *record,
    const char *provider_id);
UmiStatus umi_workbench_selection_provider_test_mapping_set_source(
    UmiWorkbenchSelectionProviderTestMapping *record,
    const char *source_id);
UmiStatus umi_workbench_selection_provider_test_mapping_set_subject(
    UmiWorkbenchSelectionProviderTestMapping *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_provider_test_mapping_set_related(
    UmiWorkbenchSelectionProviderTestMapping *record,
    const char *related_id);
UmiStatus umi_workbench_selection_provider_test_mapping_set_group(
    UmiWorkbenchSelectionProviderTestMapping *record,
    const char *group_id);
UmiStatus umi_workbench_selection_provider_test_mapping_set_description(
    UmiWorkbenchSelectionProviderTestMapping *record,
    const char *description);
uint64_t umi_workbench_selection_provider_test_mapping_hash(
    const UmiWorkbenchSelectionProviderTestMapping *record);
void umi_workbench_selection_provider_test_mapping_touch(
    UmiWorkbenchSelectionProviderTestMapping *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
