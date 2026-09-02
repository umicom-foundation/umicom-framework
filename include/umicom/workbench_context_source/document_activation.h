/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/document_activation.h
 *
 * PURPOSE:
 *   Define the reusable document activation snapshot contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_DOCUMENT_ACTIVATION_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_DOCUMENT_ACTIVATION_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context source document activation data shared with callers of
 * this public contract.
 */
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

/**
 * Initialise workbench context source document activation from caller-provided values so
 * later operations receive a known state.
 */
void umi_workbench_context_source_document_activation_init(
    UmiWorkbenchContextSourceDocumentActivation *record,
    const char *record_id);
/**
 * Check that workbench context source document activation satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_context_source_document_activation_validate(
    const UmiWorkbenchContextSourceDocumentActivation *record);
/**
 * Provide the workbench context source document activation set source operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_context_source_document_activation_set_source(
    UmiWorkbenchContextSourceDocumentActivation *record,
    const char *source_id);
/**
 * Provide the workbench context source document activation set panel operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_context_source_document_activation_set_panel(
    UmiWorkbenchContextSourceDocumentActivation *record,
    const char *panel_id);
/**
 * Provide the workbench context source document activation set subject operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_context_source_document_activation_set_subject(
    UmiWorkbenchContextSourceDocumentActivation *record,
    const char *subject_id);
/**
 * Provide the workbench context source document activation set group operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_context_source_document_activation_set_group(
    UmiWorkbenchContextSourceDocumentActivation *record,
    const char *group_id);
/**
 * Provide the workbench context source document activation set label operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_context_source_document_activation_set_label(
    UmiWorkbenchContextSourceDocumentActivation *record,
    const char *label);
/**
 * Provide the workbench context source document activation hash operation used by this
 * module and its client applications.
 */
uint64_t umi_workbench_context_source_document_activation_hash(
    const UmiWorkbenchContextSourceDocumentActivation *record);
/**
 * Provide the workbench context source document activation touch operation used by this
 * module and its client applications.
 */
void umi_workbench_context_source_document_activation_touch(
    UmiWorkbenchContextSourceDocumentActivation *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
