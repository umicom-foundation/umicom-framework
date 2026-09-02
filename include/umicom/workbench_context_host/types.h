/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/types.h
 *
 * PURPOSE:
 *   Define stable C23 contracts for hosting typed context links inside reusable workbench surfaces.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_TYPES_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/workbench_context_link/workbench_context_link.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY 160U
#define UMI_WORKBENCH_CONTEXT_HOST_TITLE_CAPACITY 256U
#define UMI_WORKBENCH_CONTEXT_HOST_TEXT_CAPACITY 512U
#define UMI_WORKBENCH_CONTEXT_HOST_PATH_CAPACITY 1024U
#define UMI_WORKBENCH_CONTEXT_HOST_MAX_ENDPOINTS 256U
#define UMI_WORKBENCH_CONTEXT_HOST_MAX_GROUPS 16U
#define UMI_WORKBENCH_CONTEXT_HOST_MAX_INBOX_ITEMS 128U
#define UMI_WORKBENCH_CONTEXT_HOST_MAX_PROFILE_ENDPOINTS 128U
#define UMI_WORKBENCH_CONTEXT_HOST_MAX_PROFILE_GROUPS 16U
#define UMI_WORKBENCH_CONTEXT_HOST_MAX_SESSION_ASSIGNMENTS 256U
#define UMI_WORKBENCH_CONTEXT_HOST_MAX_DIAGNOSTICS 128U

/**
 * List the named workbench context host panel role values accepted by this public
 * contract.
 */
typedef enum UmiWorkbenchContextHostPanelRole {
    UMI_WORKBENCH_CONTEXT_HOST_PANEL_GENERIC = 1,
    UMI_WORKBENCH_CONTEXT_HOST_PANEL_EXPLORER = 2,
    UMI_WORKBENCH_CONTEXT_HOST_PANEL_EDITOR = 3,
    UMI_WORKBENCH_CONTEXT_HOST_PANEL_PROBLEMS = 4,
    UMI_WORKBENCH_CONTEXT_HOST_PANEL_SOURCE_CONTROL = 5,
    UMI_WORKBENCH_CONTEXT_HOST_PANEL_TEST_EXPLORER = 6,
    UMI_WORKBENCH_CONTEXT_HOST_PANEL_DEBUGGER = 7,
    UMI_WORKBENCH_CONTEXT_HOST_PANEL_TERMINAL = 8,
    UMI_WORKBENCH_CONTEXT_HOST_PANEL_OUTPUT = 9,
    UMI_WORKBENCH_CONTEXT_HOST_PANEL_AI = 10,
    UMI_WORKBENCH_CONTEXT_HOST_PANEL_CHART = 11,
    UMI_WORKBENCH_CONTEXT_HOST_PANEL_ORDER_ENTRY = 12,
    UMI_WORKBENCH_CONTEXT_HOST_PANEL_RISK = 13,
    UMI_WORKBENCH_CONTEXT_HOST_PANEL_TRADE = 14,
    UMI_WORKBENCH_CONTEXT_HOST_PANEL_APPLICATION_LAUNCHER = 15,
    UMI_WORKBENCH_CONTEXT_HOST_PANEL_CONTEXT_INSPECTOR = 16,
    UMI_WORKBENCH_CONTEXT_HOST_PANEL_DOCUMENT = 17,
    UMI_WORKBENCH_CONTEXT_HOST_PANEL_DESIGNER = 18,
    UMI_WORKBENCH_CONTEXT_HOST_PANEL_MEDIA_TIMELINE = 19,
    UMI_WORKBENCH_CONTEXT_HOST_PANEL_MODEL = 20,
    UMI_WORKBENCH_CONTEXT_HOST_PANEL_KNOWLEDGE = 21,
    UMI_WORKBENCH_CONTEXT_HOST_PANEL_SYSTEM = 22,
    UMI_WORKBENCH_CONTEXT_HOST_PANEL_SERVICE = 23,
    UMI_WORKBENCH_CONTEXT_HOST_PANEL_CUSTOMER = 24,
    UMI_WORKBENCH_CONTEXT_HOST_PANEL_PAYMENT = 25,
    UMI_WORKBENCH_CONTEXT_HOST_PANEL_TREASURY = 26
} UmiWorkbenchContextHostPanelRole;

/**
 * List the named workbench context host endpoint state values accepted by this public
 * contract.
 */
typedef enum UmiWorkbenchContextHostEndpointState {
    UMI_WORKBENCH_CONTEXT_HOST_ENDPOINT_DISABLED = 1,
    UMI_WORKBENCH_CONTEXT_HOST_ENDPOINT_READY = 2,
    UMI_WORKBENCH_CONTEXT_HOST_ENDPOINT_ACTIVE = 3,
    UMI_WORKBENCH_CONTEXT_HOST_ENDPOINT_SUSPENDED = 4,
    UMI_WORKBENCH_CONTEXT_HOST_ENDPOINT_ERROR = 5
} UmiWorkbenchContextHostEndpointState;

/**
 * List the named workbench context host delivery disposition values accepted by this
 * public contract.
 */
typedef enum UmiWorkbenchContextHostDeliveryDisposition {
    UMI_WORKBENCH_CONTEXT_HOST_DELIVERY_QUEUED = 1,
    UMI_WORKBENCH_CONTEXT_HOST_DELIVERY_CONSUMED = 2,
    UMI_WORKBENCH_CONTEXT_HOST_DELIVERY_REJECTED = 3,
    UMI_WORKBENCH_CONTEXT_HOST_DELIVERY_DROPPED = 4
} UmiWorkbenchContextHostDeliveryDisposition;

/**
 * List the named workbench context host observation flag values accepted by this public
 * contract.
 */
typedef enum UmiWorkbenchContextHostObservationFlag {
    UMI_WORKBENCH_CONTEXT_HOST_OBSERVATION_NONE = 0,
    UMI_WORKBENCH_CONTEXT_HOST_OBSERVATION_WORKSPACE = 1 << 0,
    UMI_WORKBENCH_CONTEXT_HOST_OBSERVATION_PERSPECTIVE = 1 << 1,
    UMI_WORKBENCH_CONTEXT_HOST_OBSERVATION_DOCUMENT = 1 << 2,
    UMI_WORKBENCH_CONTEXT_HOST_OBSERVATION_ACTIVITY = 1 << 3,
    UMI_WORKBENCH_CONTEXT_HOST_OBSERVATION_VIEW_CONTAINER = 1 << 4
} UmiWorkbenchContextHostObservationFlag;

/**
 * List the named workbench context host command kind values accepted by this public
 * contract.
 */
typedef enum UmiWorkbenchContextHostCommandKind {
    UMI_WORKBENCH_CONTEXT_HOST_COMMAND_SET_ACTIVE_GROUP = 1,
    UMI_WORKBENCH_CONTEXT_HOST_COMMAND_ASSIGN_PANEL = 2,
    UMI_WORKBENCH_CONTEXT_HOST_COMMAND_UNASSIGN_PANEL = 3,
    UMI_WORKBENCH_CONTEXT_HOST_COMMAND_PUBLISH_SELECTION = 4,
    UMI_WORKBENCH_CONTEXT_HOST_COMMAND_BACK = 5,
    UMI_WORKBENCH_CONTEXT_HOST_COMMAND_FORWARD = 6,
    UMI_WORKBENCH_CONTEXT_HOST_COMMAND_PIN = 7,
    UMI_WORKBENCH_CONTEXT_HOST_COMMAND_CLEAR_PANEL_INBOX = 8,
    UMI_WORKBENCH_CONTEXT_HOST_COMMAND_SUSPEND = 9,
    UMI_WORKBENCH_CONTEXT_HOST_COMMAND_RESUME = 10
} UmiWorkbenchContextHostCommandKind;

/**
 * List the named workbench context host event kind values accepted by this public
 * contract.
 */
typedef enum UmiWorkbenchContextHostEventKind {
    UMI_WORKBENCH_CONTEXT_HOST_EVENT_ENDPOINT_REGISTERED = 1,
    UMI_WORKBENCH_CONTEXT_HOST_EVENT_ENDPOINT_REMOVED = 2,
    UMI_WORKBENCH_CONTEXT_HOST_EVENT_ACTIVE_GROUP_CHANGED = 3,
    UMI_WORKBENCH_CONTEXT_HOST_EVENT_CONTEXT_PUBLISHED = 4,
    UMI_WORKBENCH_CONTEXT_HOST_EVENT_CONTEXT_QUEUED = 5,
    UMI_WORKBENCH_CONTEXT_HOST_EVENT_CONTEXT_CONSUMED = 6,
    UMI_WORKBENCH_CONTEXT_HOST_EVENT_WORKBENCH_OBSERVED = 7,
    UMI_WORKBENCH_CONTEXT_HOST_EVENT_NAVIGATION = 8,
    UMI_WORKBENCH_CONTEXT_HOST_EVENT_PINNED = 9,
    UMI_WORKBENCH_CONTEXT_HOST_EVENT_ERROR = 10
} UmiWorkbenchContextHostEventKind;

/**
 * List the named workbench context host query kind values accepted by this public
 * contract.
 */
typedef enum UmiWorkbenchContextHostQueryKind {
    UMI_WORKBENCH_CONTEXT_HOST_QUERY_SNAPSHOT = 1,
    UMI_WORKBENCH_CONTEXT_HOST_QUERY_ENDPOINT = 2,
    UMI_WORKBENCH_CONTEXT_HOST_QUERY_ACTIVE_CONTEXT = 3,
    UMI_WORKBENCH_CONTEXT_HOST_QUERY_PANEL_INBOX = 4,
    UMI_WORKBENCH_CONTEXT_HOST_QUERY_HEALTH = 5,
    UMI_WORKBENCH_CONTEXT_HOST_QUERY_METRICS = 6
} UmiWorkbenchContextHostQueryKind;

/**
 * Represent the workbench context host clock data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchContextHostClock {
    uint64_t now_ms;
    uint64_t sequence;
} UmiWorkbenchContextHostClock;

/**
 * Provide the workbench context host bounded length operation used by this module and its
 * client applications.
 */
size_t umi_workbench_context_host_bounded_length(const char *text, size_t capacity);
/**
 * Check that workbench context host text satisfies its contract before another service
 * relies on it.
 */
bool umi_workbench_context_host_text_is_valid(const char *text, size_t capacity);
/**
 * Provide the workbench context host copy text operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_context_host_copy_text(
    char *destination,
    size_t capacity,
    const char *source);
/**
 * Provide the workbench context host hash text operation used by this module and its
 * client applications.
 */
uint64_t umi_workbench_context_host_hash_text(
    uint64_t hash,
    const char *text,
    size_t capacity);
/**
 * Provide the workbench context host kind mask operation used by this module and its
 * client applications.
 */
uint64_t umi_workbench_context_host_kind_mask(UmiContextKind kind);
/**
 * Provide the workbench context host kind allowed operation used by this module and its
 * client applications.
 */
bool umi_workbench_context_host_kind_allowed(uint64_t mask, UmiContextKind kind);
/**
 * Provide the workbench context host panel role text operation used by this module and its
 * client applications.
 */
const char *umi_workbench_context_host_panel_role_text(
    UmiWorkbenchContextHostPanelRole role);
/**
 * Provide the workbench context host endpoint state text operation used by this module and
 * its client applications.
 */
const char *umi_workbench_context_host_endpoint_state_text(
    UmiWorkbenchContextHostEndpointState state);
/**
 * Provide the workbench context host delivery disposition text operation used by this
 * module and its client applications.
 */
const char *umi_workbench_context_host_delivery_disposition_text(
    UmiWorkbenchContextHostDeliveryDisposition disposition);

#ifdef __cplusplus
}
#endif

#endif
