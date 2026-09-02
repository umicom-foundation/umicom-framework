/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/bookmark.h
 *
 * PURPOSE:
 *   Define the reusable event bookmark contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_BOOKMARK_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_BOOKMARK_H

#include "umicom/workbench_context_event/event.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context event bookmark data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchContextEventBookmark {
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
} UmiWorkbenchContextEventBookmark;

/**
 * Initialise workbench context event bookmark from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_event_bookmark_init(
    UmiWorkbenchContextEventBookmark *record,
    const char *record_id);
/**
 * Check that workbench context event bookmark satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_event_bookmark_validate(
    const UmiWorkbenchContextEventBookmark *record);
/**
 * Provide the workbench context event bookmark set source operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_event_bookmark_set_source(
    UmiWorkbenchContextEventBookmark *record,
    const char *source_id);
/**
 * Provide the workbench context event bookmark set subject operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_event_bookmark_set_subject(
    UmiWorkbenchContextEventBookmark *record,
    const char *subject_id);
/**
 * Provide the workbench context event bookmark set group operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_event_bookmark_set_group(
    UmiWorkbenchContextEventBookmark *record,
    const char *group_id);
/**
 * Provide the workbench context event bookmark set label operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_event_bookmark_set_label(
    UmiWorkbenchContextEventBookmark *record,
    const char *label);
/**
 * Provide the workbench context event bookmark hash operation used by this module and its
 * client applications.
 */
uint64_t umi_workbench_context_event_bookmark_hash(
    const UmiWorkbenchContextEventBookmark *record);
/**
 * Provide the workbench context event bookmark touch operation used by this module and its
 * client applications.
 */
void umi_workbench_context_event_bookmark_touch(
    UmiWorkbenchContextEventBookmark *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
