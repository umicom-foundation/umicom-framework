/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/resolver_record.h
 *
 * PURPOSE:
 *   Define the reusable selection resolver record contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_RESOLVER_RECORD_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_RESOLVER_RECORD_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextSourceResolverRecord {
    uint32_t structure_size;
    char record_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char source_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char panel_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char subject_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char label[UMI_WORKBENCH_CONTEXT_SOURCE_TEXT_CAPACITY];
    UmiWorkbenchContextSourceKind source_kind;
    UmiWorkbenchContextSourceTrigger trigger;
    UmiWorkbenchContextSourceState state;
    UmiContextKind context_kind;
    uint64_t flags;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchContextSourceResolverRecord;

void umi_workbench_context_source_resolver_record_init(
    UmiWorkbenchContextSourceResolverRecord *record,
    const char *record_id);
UmiStatus umi_workbench_context_source_resolver_record_validate(
    const UmiWorkbenchContextSourceResolverRecord *record);
UmiStatus umi_workbench_context_source_resolver_record_set_source(
    UmiWorkbenchContextSourceResolverRecord *record,
    const char *source_id);
UmiStatus umi_workbench_context_source_resolver_record_set_panel(
    UmiWorkbenchContextSourceResolverRecord *record,
    const char *panel_id);
UmiStatus umi_workbench_context_source_resolver_record_set_subject(
    UmiWorkbenchContextSourceResolverRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_context_source_resolver_record_set_group(
    UmiWorkbenchContextSourceResolverRecord *record,
    const char *group_id);
UmiStatus umi_workbench_context_source_resolver_record_set_label(
    UmiWorkbenchContextSourceResolverRecord *record,
    const char *label);
uint64_t umi_workbench_context_source_resolver_record_hash(
    const UmiWorkbenchContextSourceResolverRecord *record);
void umi_workbench_context_source_resolver_record_touch(
    UmiWorkbenchContextSourceResolverRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
