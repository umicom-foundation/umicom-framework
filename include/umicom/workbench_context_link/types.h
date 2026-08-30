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

typedef enum UmiWorkbenchContextLinkMode {
    UMI_WORKBENCH_CONTEXT_LINK_MODE_NONE = 0,
    UMI_WORKBENCH_CONTEXT_LINK_MODE_FOLLOW = 1,
    UMI_WORKBENCH_CONTEXT_LINK_MODE_PUBLISH = 2,
    UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL = 3
} UmiWorkbenchContextLinkMode;

typedef enum UmiWorkbenchContextLinkState {
    UMI_WORKBENCH_CONTEXT_LINK_STATE_DETACHED = 1,
    UMI_WORKBENCH_CONTEXT_LINK_STATE_ATTACHED = 2,
    UMI_WORKBENCH_CONTEXT_LINK_STATE_ACTIVE = 3,
    UMI_WORKBENCH_CONTEXT_LINK_STATE_SUSPENDED = 4,
    UMI_WORKBENCH_CONTEXT_LINK_STATE_CONFLICT = 5
} UmiWorkbenchContextLinkState;

typedef enum UmiWorkbenchContextLinkOrigin {
    UMI_WORKBENCH_CONTEXT_LINK_ORIGIN_USER = 1,
    UMI_WORKBENCH_CONTEXT_LINK_ORIGIN_LAYOUT = 2,
    UMI_WORKBENCH_CONTEXT_LINK_ORIGIN_SESSION = 3,
    UMI_WORKBENCH_CONTEXT_LINK_ORIGIN_APPLICATION = 4
} UmiWorkbenchContextLinkOrigin;

typedef enum UmiWorkbenchContextLinkPriority {
    UMI_WORKBENCH_CONTEXT_LINK_PRIORITY_LOW = 1,
    UMI_WORKBENCH_CONTEXT_LINK_PRIORITY_NORMAL = 2,
    UMI_WORKBENCH_CONTEXT_LINK_PRIORITY_HIGH = 3,
    UMI_WORKBENCH_CONTEXT_LINK_PRIORITY_CRITICAL = 4
} UmiWorkbenchContextLinkPriority;

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

typedef struct UmiWorkbenchContextLinkClock {
    uint64_t now_ms;
    uint64_t sequence;
} UmiWorkbenchContextLinkClock;

size_t umi_workbench_context_link_bounded_length(const char *text, size_t capacity);
bool umi_workbench_context_link_text_is_valid(const char *text, size_t capacity);
UmiStatus umi_workbench_context_link_copy_text(char *destination,
                                               size_t capacity,
                                               const char *source);
uint64_t umi_workbench_context_link_hash_bytes(uint64_t hash,
                                               const void *bytes,
                                               size_t count);
uint64_t umi_workbench_context_link_hash_text(uint64_t hash,
                                              const char *text,
                                              size_t capacity);
uint64_t umi_workbench_context_link_kind_mask(UmiContextKind kind);
bool umi_workbench_context_link_kind_allowed(uint64_t mask, UmiContextKind kind);
const char *umi_workbench_context_link_mode_text(UmiWorkbenchContextLinkMode mode);
const char *umi_workbench_context_link_state_text(UmiWorkbenchContextLinkState state);
const char *umi_workbench_context_link_origin_text(UmiWorkbenchContextLinkOrigin origin);
const char *umi_workbench_context_link_priority_text(UmiWorkbenchContextLinkPriority priority);

#ifdef __cplusplus
}
#endif

#endif
