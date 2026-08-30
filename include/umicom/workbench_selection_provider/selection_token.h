/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/selection_token.h
 *
 * PURPOSE:
 *   Define the reusable structured selection token contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_SELECTION_TOKEN_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_SELECTION_TOKEN_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionProviderSelectionToken {
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
} UmiWorkbenchSelectionProviderSelectionToken;

void umi_workbench_selection_provider_selection_token_init(
    UmiWorkbenchSelectionProviderSelectionToken *record,
    const char *record_id);
UmiStatus umi_workbench_selection_provider_selection_token_validate(
    const UmiWorkbenchSelectionProviderSelectionToken *record);
UmiStatus umi_workbench_selection_provider_selection_token_set_provider(
    UmiWorkbenchSelectionProviderSelectionToken *record,
    const char *provider_id);
UmiStatus umi_workbench_selection_provider_selection_token_set_source(
    UmiWorkbenchSelectionProviderSelectionToken *record,
    const char *source_id);
UmiStatus umi_workbench_selection_provider_selection_token_set_subject(
    UmiWorkbenchSelectionProviderSelectionToken *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_provider_selection_token_set_related(
    UmiWorkbenchSelectionProviderSelectionToken *record,
    const char *related_id);
UmiStatus umi_workbench_selection_provider_selection_token_set_group(
    UmiWorkbenchSelectionProviderSelectionToken *record,
    const char *group_id);
UmiStatus umi_workbench_selection_provider_selection_token_set_description(
    UmiWorkbenchSelectionProviderSelectionToken *record,
    const char *description);
uint64_t umi_workbench_selection_provider_selection_token_hash(
    const UmiWorkbenchSelectionProviderSelectionToken *record);
void umi_workbench_selection_provider_selection_token_touch(
    UmiWorkbenchSelectionProviderSelectionToken *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
