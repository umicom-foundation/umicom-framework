/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/dedupe_record.h
 *
 * PURPOSE:
 *   Define the reusable provider deduplication record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_DEDUPE_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_DEDUPE_RECORD_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionProviderDedupeRecord {
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
} UmiWorkbenchSelectionProviderDedupeRecord;

void umi_workbench_selection_provider_dedupe_record_init(
    UmiWorkbenchSelectionProviderDedupeRecord *record,
    const char *record_id);
UmiStatus umi_workbench_selection_provider_dedupe_record_validate(
    const UmiWorkbenchSelectionProviderDedupeRecord *record);
UmiStatus umi_workbench_selection_provider_dedupe_record_set_provider(
    UmiWorkbenchSelectionProviderDedupeRecord *record,
    const char *provider_id);
UmiStatus umi_workbench_selection_provider_dedupe_record_set_source(
    UmiWorkbenchSelectionProviderDedupeRecord *record,
    const char *source_id);
UmiStatus umi_workbench_selection_provider_dedupe_record_set_subject(
    UmiWorkbenchSelectionProviderDedupeRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_provider_dedupe_record_set_related(
    UmiWorkbenchSelectionProviderDedupeRecord *record,
    const char *related_id);
UmiStatus umi_workbench_selection_provider_dedupe_record_set_group(
    UmiWorkbenchSelectionProviderDedupeRecord *record,
    const char *group_id);
UmiStatus umi_workbench_selection_provider_dedupe_record_set_description(
    UmiWorkbenchSelectionProviderDedupeRecord *record,
    const char *description);
uint64_t umi_workbench_selection_provider_dedupe_record_hash(
    const UmiWorkbenchSelectionProviderDedupeRecord *record);
void umi_workbench_selection_provider_dedupe_record_touch(
    UmiWorkbenchSelectionProviderDedupeRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
