/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/types.h
 *
 * PURPOSE:
 *   Define stable C23 contracts for context-linked workbench panels and colour groups.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_TYPES_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/context_channel/payload.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY 160U
#define UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY 256U
#define UMI_WORKBENCH_CONTEXT_LINK_PATH_CAPACITY 512U
#define UMI_WORKBENCH_CONTEXT_LINK_MAX_GROUPS 16U
#define UMI_WORKBENCH_CONTEXT_LINK_MAX_BINDINGS 256U
#define UMI_WORKBENCH_CONTEXT_LINK_MAX_DELIVERIES 256U
#define UMI_WORKBENCH_CONTEXT_LINK_MAX_HISTORY 512U
#define UMI_WORKBENCH_CONTEXT_LINK_MAX_PINS 128U
#define UMI_WORKBENCH_CONTEXT_LINK_MAX_DIAGNOSTICS 128U
#define UMI_WORKBENCH_CONTEXT_LINK_MAX_COMMANDS 128U
#define UMI_WORKBENCH_CONTEXT_LINK_ALL_KINDS_MASK UINT64_C(0x1ff)

/**
 * List the named workbench context link mode values accepted by this public contract.
 */
typedef enum UmiWorkbenchContextLinkMode {
    UMI_WORKBENCH_CONTEXT_LINK_MODE_NONE = 0,
    UMI_WORKBENCH_CONTEXT_LINK_MODE_FOLLOW = 1,
    UMI_WORKBENCH_CONTEXT_LINK_MODE_PUBLISH = 2,
    UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL = 3
} UmiWorkbenchContextLinkMode;

/**
 * List the named workbench context link state values accepted by this public contract.
 */
typedef enum UmiWorkbenchContextLinkState {
    UMI_WORKBENCH_CONTEXT_LINK_STATE_DETACHED = 1,
    UMI_WORKBENCH_CONTEXT_LINK_STATE_ATTACHED = 2,
    UMI_WORKBENCH_CONTEXT_LINK_STATE_ACTIVE = 3,
    UMI_WORKBENCH_CONTEXT_LINK_STATE_SUSPENDED = 4,
    UMI_WORKBENCH_CONTEXT_LINK_STATE_CONFLICT = 5
} UmiWorkbenchContextLinkState;

/**
 * List the named workbench context link origin values accepted by this public contract.
 */
typedef enum UmiWorkbenchContextLinkOrigin {
    UMI_WORKBENCH_CONTEXT_LINK_ORIGIN_USER = 1,
    UMI_WORKBENCH_CONTEXT_LINK_ORIGIN_LAYOUT = 2,
    UMI_WORKBENCH_CONTEXT_LINK_ORIGIN_SESSION = 3,
    UMI_WORKBENCH_CONTEXT_LINK_ORIGIN_APPLICATION = 4
} UmiWorkbenchContextLinkOrigin;

/**
 * List the named workbench context link priority values accepted by this public contract.
 */
typedef enum UmiWorkbenchContextLinkPriority {
    UMI_WORKBENCH_CONTEXT_LINK_PRIORITY_LOW = 1,
    UMI_WORKBENCH_CONTEXT_LINK_PRIORITY_NORMAL = 2,
    UMI_WORKBENCH_CONTEXT_LINK_PRIORITY_HIGH = 3,
    UMI_WORKBENCH_CONTEXT_LINK_PRIORITY_CRITICAL = 4
} UmiWorkbenchContextLinkPriority;

/**
 * List the named workbench context link command kind values accepted by this public
 * contract.
 */
typedef enum UmiWorkbenchContextLinkCommandKind {
    UMI_WORKBENCH_CONTEXT_LINK_COMMAND_ATTACH = 1,
    UMI_WORKBENCH_CONTEXT_LINK_COMMAND_DETACH = 2,
    UMI_WORKBENCH_CONTEXT_LINK_COMMAND_PUBLISH = 3,
    UMI_WORKBENCH_CONTEXT_LINK_COMMAND_PIN = 4,
    UMI_WORKBENCH_CONTEXT_LINK_COMMAND_UNPIN = 5,
    UMI_WORKBENCH_CONTEXT_LINK_COMMAND_BACK = 6,
    UMI_WORKBENCH_CONTEXT_LINK_COMMAND_FORWARD = 7,
    UMI_WORKBENCH_CONTEXT_LINK_COMMAND_CLEAR_HISTORY = 8,
    UMI_WORKBENCH_CONTEXT_LINK_COMMAND_SUSPEND = 9,
    UMI_WORKBENCH_CONTEXT_LINK_COMMAND_RESUME = 10
} UmiWorkbenchContextLinkCommandKind;

/**
 * List the named workbench context link event kind values accepted by this public
 * contract.
 */
typedef enum UmiWorkbenchContextLinkEventKind {
    UMI_WORKBENCH_CONTEXT_LINK_EVENT_GROUP_CHANGED = 1,
    UMI_WORKBENCH_CONTEXT_LINK_EVENT_PANEL_ATTACHED = 2,
    UMI_WORKBENCH_CONTEXT_LINK_EVENT_PANEL_DETACHED = 3,
    UMI_WORKBENCH_CONTEXT_LINK_EVENT_CONTEXT_PUBLISHED = 4,
    UMI_WORKBENCH_CONTEXT_LINK_EVENT_CONTEXT_DELIVERED = 5,
    UMI_WORKBENCH_CONTEXT_LINK_EVENT_CONTEXT_PINNED = 6,
    UMI_WORKBENCH_CONTEXT_LINK_EVENT_CONTEXT_RESTORED = 7,
    UMI_WORKBENCH_CONTEXT_LINK_EVENT_CONFLICT = 8
} UmiWorkbenchContextLinkEventKind;

/**
 * Represent the workbench context link clock data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchContextLinkClock {
    uint64_t now_ms;
    uint64_t sequence;
} UmiWorkbenchContextLinkClock;

/**
 * Provide the workbench context link bounded length operation used by this module and its
 * client applications.
 */
size_t umi_workbench_context_link_bounded_length(const char *text, size_t capacity);
/**
 * Check that workbench context link text satisfies its contract before another service
 * relies on it.
 */
bool umi_workbench_context_link_text_is_valid(const char *text, size_t capacity);
/**
 * Provide the workbench context link copy text operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_context_link_copy_text(char *destination,
                                               size_t capacity,
                                               const char *source);
/**
 * Provide the workbench context link hash bytes operation used by this module and its
 * client applications.
 */
uint64_t umi_workbench_context_link_hash_bytes(uint64_t hash,
                                               const void *bytes,
                                               size_t count);
/**
 * Provide the workbench context link hash text operation used by this module and its
 * client applications.
 */
uint64_t umi_workbench_context_link_hash_text(uint64_t hash,
                                              const char *text,
                                              size_t capacity);
/**
 * Provide the workbench context link kind mask operation used by this module and its
 * client applications.
 */
uint64_t umi_workbench_context_link_kind_mask(UmiContextKind kind);
/**
 * Provide the workbench context link kind allowed operation used by this module and its
 * client applications.
 */
bool umi_workbench_context_link_kind_allowed(uint64_t mask, UmiContextKind kind);
/**
 * Provide the workbench context link mode text operation used by this module and its
 * client applications.
 */
const char *umi_workbench_context_link_mode_text(UmiWorkbenchContextLinkMode mode);
/**
 * Provide the workbench context link state text operation used by this module and its
 * client applications.
 */
const char *umi_workbench_context_link_state_text(UmiWorkbenchContextLinkState state);
/**
 * Provide the workbench context link origin text operation used by this module and its
 * client applications.
 */
const char *umi_workbench_context_link_origin_text(UmiWorkbenchContextLinkOrigin origin);
/**
 * Provide the workbench context link priority text operation used by this module and its
 * client applications.
 */
const char *umi_workbench_context_link_priority_text(UmiWorkbenchContextLinkPriority priority);

#ifdef __cplusplus
}
#endif

#endif
