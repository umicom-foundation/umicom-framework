/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/test_row_record.h
 *
 * PURPOSE:
 *   Define the reusable structured test row record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_TEST_ROW_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_TEST_ROW_RECORD_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionProviderTestRowRecord {
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
} UmiWorkbenchSelectionProviderTestRowRecord;

void umi_workbench_selection_provider_test_row_record_init(
    UmiWorkbenchSelectionProviderTestRowRecord *record,
    const char *record_id);
UmiStatus umi_workbench_selection_provider_test_row_record_validate(
    const UmiWorkbenchSelectionProviderTestRowRecord *record);
UmiStatus umi_workbench_selection_provider_test_row_record_set_provider(
    UmiWorkbenchSelectionProviderTestRowRecord *record,
    const char *provider_id);
UmiStatus umi_workbench_selection_provider_test_row_record_set_source(
    UmiWorkbenchSelectionProviderTestRowRecord *record,
    const char *source_id);
UmiStatus umi_workbench_selection_provider_test_row_record_set_subject(
    UmiWorkbenchSelectionProviderTestRowRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_provider_test_row_record_set_related(
    UmiWorkbenchSelectionProviderTestRowRecord *record,
    const char *related_id);
UmiStatus umi_workbench_selection_provider_test_row_record_set_group(
    UmiWorkbenchSelectionProviderTestRowRecord *record,
    const char *group_id);
UmiStatus umi_workbench_selection_provider_test_row_record_set_description(
    UmiWorkbenchSelectionProviderTestRowRecord *record,
    const char *description);
uint64_t umi_workbench_selection_provider_test_row_record_hash(
    const UmiWorkbenchSelectionProviderTestRowRecord *record);
void umi_workbench_selection_provider_test_row_record_touch(
    UmiWorkbenchSelectionProviderTestRowRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
