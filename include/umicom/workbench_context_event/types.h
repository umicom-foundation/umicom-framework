/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/types.h
 *
 * PURPOSE:
 *   Define stable C23 contracts for normalising real workbench interactions before typed-context publication.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_TYPES_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/context_channel/payload.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_WORKBENCH_CONTEXT_EVENT_ID_CAPACITY 160U
#define UMI_WORKBENCH_CONTEXT_EVENT_TEXT_CAPACITY 512U
#define UMI_WORKBENCH_CONTEXT_EVENT_PATH_CAPACITY 1024U
#define UMI_WORKBENCH_CONTEXT_EVENT_MAX_METADATA 16U
#define UMI_WORKBENCH_CONTEXT_EVENT_MAX_SOURCES 256U
#define UMI_WORKBENCH_CONTEXT_EVENT_MAX_SUBSCRIPTIONS 256U
#define UMI_WORKBENCH_CONTEXT_EVENT_MAX_QUEUE 1024U
#define UMI_WORKBENCH_CONTEXT_EVENT_MAX_HISTORY 2048U

/**
 * List the named workbench context event kind values accepted by this public contract.
 */
typedef enum UmiWorkbenchContextEventKind {
    UMI_WORKBENCH_CONTEXT_EVENT_NONE = 0,
    UMI_WORKBENCH_CONTEXT_EVENT_PROJECT_SELECTION = 1,
    UMI_WORKBENCH_CONTEXT_EVENT_EDITOR_OPEN = 2,
    UMI_WORKBENCH_CONTEXT_EVENT_EDITOR_CARET = 3,
    UMI_WORKBENCH_CONTEXT_EVENT_EDITOR_SELECTION = 4,
    UMI_WORKBENCH_CONTEXT_EVENT_DIAGNOSTIC_SELECTION = 5,
    UMI_WORKBENCH_CONTEXT_EVENT_SOURCE_CONTROL_SELECTION = 6,
    UMI_WORKBENCH_CONTEXT_EVENT_TEST_SELECTION = 7,
    UMI_WORKBENCH_CONTEXT_EVENT_DEBUG_LOCATION = 8,
    UMI_WORKBENCH_CONTEXT_EVENT_TERMINAL_SESSION = 9,
    UMI_WORKBENCH_CONTEXT_EVENT_AI_SELECTION = 10,
    UMI_WORKBENCH_CONTEXT_EVENT_APPLICATION_SELECTION = 11,
    UMI_WORKBENCH_CONTEXT_EVENT_INSTRUMENT_SELECTION = 12,
    UMI_WORKBENCH_CONTEXT_EVENT_ACCOUNT_SELECTION = 13,
    UMI_WORKBENCH_CONTEXT_EVENT_TRADE_SELECTION = 14,
    UMI_WORKBENCH_CONTEXT_EVENT_WORKSPACE_CHANGE = 15,
    UMI_WORKBENCH_CONTEXT_EVENT_MEDIA_SELECTION = 16,
    UMI_WORKBENCH_CONTEXT_EVENT_GENERIC_SELECTION = 17
} UmiWorkbenchContextEventKind;

/**
 * List the named workbench context event source kind values accepted by this public
 * contract.
 */
typedef enum UmiWorkbenchContextEventSourceKind {
    UMI_WORKBENCH_CONTEXT_EVENT_SOURCE_MODEL = 1,
    UMI_WORKBENCH_CONTEXT_EVENT_SOURCE_GTK4 = 2,
    UMI_WORKBENCH_CONTEXT_EVENT_SOURCE_COMMAND = 3,
    UMI_WORKBENCH_CONTEXT_EVENT_SOURCE_SERVICE = 4,
    UMI_WORKBENCH_CONTEXT_EVENT_SOURCE_EXTERNAL = 5
} UmiWorkbenchContextEventSourceKind;

/**
 * List the named workbench context event state values accepted by this public contract.
 */
typedef enum UmiWorkbenchContextEventState {
    UMI_WORKBENCH_CONTEXT_EVENT_CREATED = 1,
    UMI_WORKBENCH_CONTEXT_EVENT_ACCEPTED = 2,
    UMI_WORKBENCH_CONTEXT_EVENT_COALESCED = 3,
    UMI_WORKBENCH_CONTEXT_EVENT_ROUTED = 4,
    UMI_WORKBENCH_CONTEXT_EVENT_REJECTED = 5,
    UMI_WORKBENCH_CONTEXT_EVENT_DROPPED = 6
} UmiWorkbenchContextEventState;

/**
 * List the named workbench context event priority values accepted by this public contract.
 */
typedef enum UmiWorkbenchContextEventPriority {
    UMI_WORKBENCH_CONTEXT_EVENT_PRIORITY_BACKGROUND = 1,
    UMI_WORKBENCH_CONTEXT_EVENT_PRIORITY_NORMAL = 2,
    UMI_WORKBENCH_CONTEXT_EVENT_PRIORITY_INTERACTIVE = 3,
    UMI_WORKBENCH_CONTEXT_EVENT_PRIORITY_URGENT = 4
} UmiWorkbenchContextEventPriority;

/**
 * List the named workbench context event coalescing mode values accepted by this public
 * contract.
 */
typedef enum UmiWorkbenchContextEventCoalescingMode {
    UMI_WORKBENCH_CONTEXT_EVENT_COALESCE_NONE = 0,
    UMI_WORKBENCH_CONTEXT_EVENT_COALESCE_BY_SOURCE = 1,
    UMI_WORKBENCH_CONTEXT_EVENT_COALESCE_BY_KIND = 2,
    UMI_WORKBENCH_CONTEXT_EVENT_COALESCE_BY_SUBJECT = 3
} UmiWorkbenchContextEventCoalescingMode;

/**
 * Represent the workbench context event metadata data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchContextEventMetadata {
    char name[UMI_WORKBENCH_CONTEXT_EVENT_ID_CAPACITY];
    char value[UMI_WORKBENCH_CONTEXT_EVENT_TEXT_CAPACITY];
} UmiWorkbenchContextEventMetadata;

/**
 * Provide the workbench context event bounded length operation used by this module and its
 * client applications.
 */
size_t umi_workbench_context_event_bounded_length(const char *text, size_t capacity);
/**
 * Check that workbench context event text satisfies its contract before another service
 * relies on it.
 */
bool umi_workbench_context_event_text_is_valid(const char *text, size_t capacity);
/**
 * Provide the workbench context event copy text operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_context_event_copy_text(
    char *destination, size_t capacity, const char *source);
/**
 * Provide the workbench context event hash text operation used by this module and its
 * client applications.
 */
uint64_t umi_workbench_context_event_hash_text(
    uint64_t hash, const char *text, size_t capacity);
/**
 * Provide the workbench context event kind text operation used by this module and its
 * client applications.
 */
const char *umi_workbench_context_event_kind_text(UmiWorkbenchContextEventKind kind);
/**
 * Provide the workbench context event state text operation used by this module and its
 * client applications.
 */
const char *umi_workbench_context_event_state_text(UmiWorkbenchContextEventState state);
/**
 * Provide the workbench context event priority text operation used by this module and its
 * client applications.
 */
const char *umi_workbench_context_event_priority_text(UmiWorkbenchContextEventPriority priority);

#ifdef __cplusplus
}
#endif
#endif
