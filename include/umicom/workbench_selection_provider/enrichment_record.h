/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/enrichment_record.h
 *
 * PURPOSE:
 *   Define the reusable selection enrichment record contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_ENRICHMENT_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_ENRICHMENT_RECORD_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionProviderEnrichmentRecord {
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
} UmiWorkbenchSelectionProviderEnrichmentRecord;

void umi_workbench_selection_provider_enrichment_record_init(
    UmiWorkbenchSelectionProviderEnrichmentRecord *record,
    const char *record_id);
UmiStatus umi_workbench_selection_provider_enrichment_record_validate(
    const UmiWorkbenchSelectionProviderEnrichmentRecord *record);
UmiStatus umi_workbench_selection_provider_enrichment_record_set_provider(
    UmiWorkbenchSelectionProviderEnrichmentRecord *record,
    const char *provider_id);
UmiStatus umi_workbench_selection_provider_enrichment_record_set_source(
    UmiWorkbenchSelectionProviderEnrichmentRecord *record,
    const char *source_id);
UmiStatus umi_workbench_selection_provider_enrichment_record_set_subject(
    UmiWorkbenchSelectionProviderEnrichmentRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_provider_enrichment_record_set_related(
    UmiWorkbenchSelectionProviderEnrichmentRecord *record,
    const char *related_id);
UmiStatus umi_workbench_selection_provider_enrichment_record_set_group(
    UmiWorkbenchSelectionProviderEnrichmentRecord *record,
    const char *group_id);
UmiStatus umi_workbench_selection_provider_enrichment_record_set_description(
    UmiWorkbenchSelectionProviderEnrichmentRecord *record,
    const char *description);
uint64_t umi_workbench_selection_provider_enrichment_record_hash(
    const UmiWorkbenchSelectionProviderEnrichmentRecord *record);
void umi_workbench_selection_provider_enrichment_record_touch(
    UmiWorkbenchSelectionProviderEnrichmentRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
