/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/document_state.h
 *
 * PURPOSE:
 *   Define the reusable document state contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_DOCUMENT_STATE_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_DOCUMENT_STATE_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context source document state data shared with callers of this
 * public contract.
 */
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

/**
 * Initialise workbench context source document state from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_source_document_state_init(
    UmiWorkbenchContextSourceDocumentState *record,
    const char *record_id);
/**
 * Check that workbench context source document state satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_source_document_state_validate(
    const UmiWorkbenchContextSourceDocumentState *record);
/**
 * Provide the workbench context source document state set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_document_state_set_source(
    UmiWorkbenchContextSourceDocumentState *record,
    const char *source_id);
/**
 * Provide the workbench context source document state set panel operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_document_state_set_panel(
    UmiWorkbenchContextSourceDocumentState *record,
    const char *panel_id);
/**
 * Provide the workbench context source document state set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_document_state_set_subject(
    UmiWorkbenchContextSourceDocumentState *record,
    const char *subject_id);
/**
 * Provide the workbench context source document state set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_document_state_set_group(
    UmiWorkbenchContextSourceDocumentState *record,
    const char *group_id);
/**
 * Provide the workbench context source document state set label operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_document_state_set_label(
    UmiWorkbenchContextSourceDocumentState *record,
    const char *label);
/**
 * Provide the workbench context source document state hash operation used by this module
 * and its client applications.
 */
uint64_t umi_workbench_context_source_document_state_hash(
    const UmiWorkbenchContextSourceDocumentState *record);
/**
 * Provide the workbench context source document state touch operation used by this module
 * and its client applications.
 */
void umi_workbench_context_source_document_state_touch(
    UmiWorkbenchContextSourceDocumentState *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
