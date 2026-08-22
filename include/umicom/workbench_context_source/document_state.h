/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/document_state.h
 *
 * PURPOSE:
 *   Define the reusable document state contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_DOCUMENT_STATE_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_DOCUMENT_STATE_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextSourceDocumentState {
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
} UmiWorkbenchContextSourceDocumentState;

void umi_workbench_context_source_document_state_init(
    UmiWorkbenchContextSourceDocumentState *record,
    const char *record_id);
UmiStatus umi_workbench_context_source_document_state_validate(
    const UmiWorkbenchContextSourceDocumentState *record);
UmiStatus umi_workbench_context_source_document_state_set_source(
    UmiWorkbenchContextSourceDocumentState *record,
    const char *source_id);
UmiStatus umi_workbench_context_source_document_state_set_panel(
    UmiWorkbenchContextSourceDocumentState *record,
    const char *panel_id);
UmiStatus umi_workbench_context_source_document_state_set_subject(
    UmiWorkbenchContextSourceDocumentState *record,
    const char *subject_id);
UmiStatus umi_workbench_context_source_document_state_set_group(
    UmiWorkbenchContextSourceDocumentState *record,
    const char *group_id);
UmiStatus umi_workbench_context_source_document_state_set_label(
    UmiWorkbenchContextSourceDocumentState *record,
    const char *label);
uint64_t umi_workbench_context_source_document_state_hash(
    const UmiWorkbenchContextSourceDocumentState *record);
void umi_workbench_context_source_document_state_touch(
    UmiWorkbenchContextSourceDocumentState *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
