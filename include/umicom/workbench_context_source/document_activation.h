/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/document_activation.h
 *
 * PURPOSE:
 *   Define the reusable document activation snapshot contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_DOCUMENT_ACTIVATION_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_DOCUMENT_ACTIVATION_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextSourceDocumentActivation {
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
} UmiWorkbenchContextSourceDocumentActivation;

void umi_workbench_context_source_document_activation_init(
    UmiWorkbenchContextSourceDocumentActivation *record,
    const char *record_id);
UmiStatus umi_workbench_context_source_document_activation_validate(
    const UmiWorkbenchContextSourceDocumentActivation *record);
UmiStatus umi_workbench_context_source_document_activation_set_source(
    UmiWorkbenchContextSourceDocumentActivation *record,
    const char *source_id);
UmiStatus umi_workbench_context_source_document_activation_set_panel(
    UmiWorkbenchContextSourceDocumentActivation *record,
    const char *panel_id);
UmiStatus umi_workbench_context_source_document_activation_set_subject(
    UmiWorkbenchContextSourceDocumentActivation *record,
    const char *subject_id);
UmiStatus umi_workbench_context_source_document_activation_set_group(
    UmiWorkbenchContextSourceDocumentActivation *record,
    const char *group_id);
UmiStatus umi_workbench_context_source_document_activation_set_label(
    UmiWorkbenchContextSourceDocumentActivation *record,
    const char *label);
uint64_t umi_workbench_context_source_document_activation_hash(
    const UmiWorkbenchContextSourceDocumentActivation *record);
void umi_workbench_context_source_document_activation_touch(
    UmiWorkbenchContextSourceDocumentActivation *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
