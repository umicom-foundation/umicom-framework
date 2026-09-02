/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/capability.h
 *
 * PURPOSE:
 *   Define the reusable source capability contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_CAPABILITY_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_CAPABILITY_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context source capability data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextSourceCapability {
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
} UmiWorkbenchContextSourceCapability;

/**
 * Initialise workbench context source capability from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_source_capability_init(
    UmiWorkbenchContextSourceCapability *record,
    const char *record_id);
/**
 * Check that workbench context source capability satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_source_capability_validate(
    const UmiWorkbenchContextSourceCapability *record);
/**
 * Provide the workbench context source capability set source operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_source_capability_set_source(
    UmiWorkbenchContextSourceCapability *record,
    const char *source_id);
/**
 * Provide the workbench context source capability set panel operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_source_capability_set_panel(
    UmiWorkbenchContextSourceCapability *record,
    const char *panel_id);
/**
 * Provide the workbench context source capability set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_capability_set_subject(
    UmiWorkbenchContextSourceCapability *record,
    const char *subject_id);
/**
 * Provide the workbench context source capability set group operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_source_capability_set_group(
    UmiWorkbenchContextSourceCapability *record,
    const char *group_id);
/**
 * Provide the workbench context source capability set label operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_source_capability_set_label(
    UmiWorkbenchContextSourceCapability *record,
    const char *label);
/**
 * Provide the workbench context source capability hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_context_source_capability_hash(
    const UmiWorkbenchContextSourceCapability *record);
/**
 * Provide the workbench context source capability touch operation used by this module and
 * its client applications.
 */
void umi_workbench_context_source_capability_touch(
    UmiWorkbenchContextSourceCapability *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
