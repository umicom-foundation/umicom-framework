/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/profile.h
 *
 * PURPOSE:
 *   Define the reusable event routing profile contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_PROFILE_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_PROFILE_H

#include "umicom/workbench_context_event/event.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context event profile data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchContextEventProfile {
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
} UmiWorkbenchContextEventProfile;

/**
 * Initialise workbench context event profile from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_event_profile_init(
    UmiWorkbenchContextEventProfile *record,
    const char *record_id);
/**
 * Check that workbench context event profile satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_workbench_context_event_profile_validate(
    const UmiWorkbenchContextEventProfile *record);
/**
 * Provide the workbench context event profile set source operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_event_profile_set_source(
    UmiWorkbenchContextEventProfile *record,
    const char *source_id);
/**
 * Provide the workbench context event profile set subject operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_event_profile_set_subject(
    UmiWorkbenchContextEventProfile *record,
    const char *subject_id);
/**
 * Provide the workbench context event profile set group operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_event_profile_set_group(
    UmiWorkbenchContextEventProfile *record,
    const char *group_id);
/**
 * Provide the workbench context event profile set label operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_event_profile_set_label(
    UmiWorkbenchContextEventProfile *record,
    const char *label);
/**
 * Provide the workbench context event profile hash operation used by this module and its
 * client applications.
 */
uint64_t umi_workbench_context_event_profile_hash(
    const UmiWorkbenchContextEventProfile *record);
/**
 * Provide the workbench context event profile touch operation used by this module and its
 * client applications.
 */
void umi_workbench_context_event_profile_touch(
    UmiWorkbenchContextEventProfile *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
