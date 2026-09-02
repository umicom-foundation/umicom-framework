/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/group_binding.h
 *
 * PURPOSE:
 *   Define the reusable group event binding contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_GROUP_BINDING_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_GROUP_BINDING_H

#include "umicom/workbench_context_event/event.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context event group binding data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextEventGroupBinding {
    uint32_t structure_size;
    char record_id[UMI_WORKBENCH_CONTEXT_EVENT_ID_CAPACITY];
    char source_id[UMI_WORKBENCH_CONTEXT_EVENT_ID_CAPACITY];
    char subject_id[UMI_WORKBENCH_CONTEXT_EVENT_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_EVENT_ID_CAPACITY];
    char label[UMI_WORKBENCH_CONTEXT_EVENT_TEXT_CAPACITY];
    UmiWorkbenchContextEventKind event_kind;
    UmiContextKind context_kind;
    UmiWorkbenchContextEventPriority priority;
    UmiWorkbenchContextEventState state;
    uint64_t flags;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchContextEventGroupBinding;

/**
 * Initialise workbench context event group binding from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_event_group_binding_init(
    UmiWorkbenchContextEventGroupBinding *record,
    const char *record_id);
/**
 * Check that workbench context event group binding satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_event_group_binding_validate(
    const UmiWorkbenchContextEventGroupBinding *record);
/**
 * Provide the workbench context event group binding set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_group_binding_set_source(
    UmiWorkbenchContextEventGroupBinding *record,
    const char *source_id);
/**
 * Provide the workbench context event group binding set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_group_binding_set_subject(
    UmiWorkbenchContextEventGroupBinding *record,
    const char *subject_id);
/**
 * Provide the workbench context event group binding set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_group_binding_set_group(
    UmiWorkbenchContextEventGroupBinding *record,
    const char *group_id);
/**
 * Provide the workbench context event group binding set label operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_group_binding_set_label(
    UmiWorkbenchContextEventGroupBinding *record,
    const char *label);
/**
 * Provide the workbench context event group binding hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_context_event_group_binding_hash(
    const UmiWorkbenchContextEventGroupBinding *record);
/**
 * Provide the workbench context event group binding touch operation used by this module
 * and its client applications.
 */
void umi_workbench_context_event_group_binding_touch(
    UmiWorkbenchContextEventGroupBinding *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
