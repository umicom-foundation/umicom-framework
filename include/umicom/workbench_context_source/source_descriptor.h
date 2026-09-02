/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/source_descriptor.h
 *
 * PURPOSE:
 *   Define the reusable live source descriptor contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_SOURCE_DESCRIPTOR_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_SOURCE_DESCRIPTOR_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context source source descriptor data shared with callers of
 * this public contract.
 */
typedef struct UmiWorkbenchContextSourceSourceDescriptor {
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
} UmiWorkbenchContextSourceSourceDescriptor;

/**
 * Initialise workbench context source source descriptor from caller-provided values so
 * later operations receive a known state.
 */
void umi_workbench_context_source_source_descriptor_init(
    UmiWorkbenchContextSourceSourceDescriptor *record,
    const char *record_id);
/**
 * Check that workbench context source source descriptor satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_context_source_source_descriptor_validate(
    const UmiWorkbenchContextSourceSourceDescriptor *record);
/**
 * Provide the workbench context source source descriptor set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_source_descriptor_set_source(
    UmiWorkbenchContextSourceSourceDescriptor *record,
    const char *source_id);
/**
 * Provide the workbench context source source descriptor set panel operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_source_descriptor_set_panel(
    UmiWorkbenchContextSourceSourceDescriptor *record,
    const char *panel_id);
/**
 * Provide the workbench context source source descriptor set subject operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_context_source_source_descriptor_set_subject(
    UmiWorkbenchContextSourceSourceDescriptor *record,
    const char *subject_id);
/**
 * Provide the workbench context source source descriptor set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_source_descriptor_set_group(
    UmiWorkbenchContextSourceSourceDescriptor *record,
    const char *group_id);
/**
 * Provide the workbench context source source descriptor set label operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_source_descriptor_set_label(
    UmiWorkbenchContextSourceSourceDescriptor *record,
    const char *label);
/**
 * Provide the workbench context source source descriptor hash operation used by this
 * module and its client applications.
 */
uint64_t umi_workbench_context_source_source_descriptor_hash(
    const UmiWorkbenchContextSourceSourceDescriptor *record);
/**
 * Provide the workbench context source source descriptor touch operation used by this
 * module and its client applications.
 */
void umi_workbench_context_source_source_descriptor_touch(
    UmiWorkbenchContextSourceSourceDescriptor *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
