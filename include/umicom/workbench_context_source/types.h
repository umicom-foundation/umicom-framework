/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/types.h
 *
 * PURPOSE:
 *   Define stable C23 contracts for binding real UI/model interaction sources to the workbench context event pipeline.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_TYPES_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/workbench_context_event/event.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY 160U
#define UMI_WORKBENCH_CONTEXT_SOURCE_TEXT_CAPACITY 512U
#define UMI_WORKBENCH_CONTEXT_SOURCE_PATH_CAPACITY 1024U
#define UMI_WORKBENCH_CONTEXT_SOURCE_MAX_SOURCES 256U
#define UMI_WORKBENCH_CONTEXT_SOURCE_MAX_METADATA 16U
#define UMI_WORKBENCH_CONTEXT_SOURCE_MAX_HISTORY 1024U
#define UMI_WORKBENCH_CONTEXT_SOURCE_MAX_DIAGNOSTICS 128U
#define UMI_WORKBENCH_CONTEXT_SOURCE_ALL_KINDS_MASK UINT64_C(0x1ff)

/**
 * List the named workbench context source kind values accepted by this public contract.
 */
typedef enum UmiWorkbenchContextSourceKind {
    UMI_WORKBENCH_CONTEXT_SOURCE_GENERIC = 1,
    UMI_WORKBENCH_CONTEXT_SOURCE_EDITOR = 2,
    UMI_WORKBENCH_CONTEXT_SOURCE_PROJECT_EXPLORER = 3,
    UMI_WORKBENCH_CONTEXT_SOURCE_PROBLEMS = 4,
    UMI_WORKBENCH_CONTEXT_SOURCE_SOURCE_CONTROL = 5,
    UMI_WORKBENCH_CONTEXT_SOURCE_TEST_EXPLORER = 6,
    UMI_WORKBENCH_CONTEXT_SOURCE_DEBUGGER = 7,
    UMI_WORKBENCH_CONTEXT_SOURCE_TERMINAL = 8,
    UMI_WORKBENCH_CONTEXT_SOURCE_AI = 9,
    UMI_WORKBENCH_CONTEXT_SOURCE_APPLICATION_LAUNCHER = 10,
    UMI_WORKBENCH_CONTEXT_SOURCE_WATCHLIST = 11,
    UMI_WORKBENCH_CONTEXT_SOURCE_ACCOUNT_SELECTOR = 12,
    UMI_WORKBENCH_CONTEXT_SOURCE_TRADE_BLOTTER = 13,
    UMI_WORKBENCH_CONTEXT_SOURCE_CHART = 14,
    UMI_WORKBENCH_CONTEXT_SOURCE_RISK = 15,
    UMI_WORKBENCH_CONTEXT_SOURCE_MEDIA = 16
} UmiWorkbenchContextSourceKind;

/**
 * List the named workbench context source trigger values accepted by this public contract.
 */
typedef enum UmiWorkbenchContextSourceTrigger {
    UMI_WORKBENCH_CONTEXT_SOURCE_TRIGGER_ACTIVATE = 1,
    UMI_WORKBENCH_CONTEXT_SOURCE_TRIGGER_SELECT = 2,
    UMI_WORKBENCH_CONTEXT_SOURCE_TRIGGER_CARET = 3,
    UMI_WORKBENCH_CONTEXT_SOURCE_TRIGGER_RANGE = 4,
    UMI_WORKBENCH_CONTEXT_SOURCE_TRIGGER_CHANGE = 5,
    UMI_WORKBENCH_CONTEXT_SOURCE_TRIGGER_FOCUS = 6,
    UMI_WORKBENCH_CONTEXT_SOURCE_TRIGGER_OPEN = 7,
    UMI_WORKBENCH_CONTEXT_SOURCE_TRIGGER_NAVIGATE = 8
} UmiWorkbenchContextSourceTrigger;

/**
 * List the named workbench context source state values accepted by this public contract.
 */
typedef enum UmiWorkbenchContextSourceState {
    UMI_WORKBENCH_CONTEXT_SOURCE_STATE_CREATED = 1,
    UMI_WORKBENCH_CONTEXT_SOURCE_STATE_READY = 2,
    UMI_WORKBENCH_CONTEXT_SOURCE_STATE_ACTIVE = 3,
    UMI_WORKBENCH_CONTEXT_SOURCE_STATE_SUSPENDED = 4,
    UMI_WORKBENCH_CONTEXT_SOURCE_STATE_FAILED = 5,
    UMI_WORKBENCH_CONTEXT_SOURCE_STATE_STOPPED = 6
} UmiWorkbenchContextSourceState;

/**
 * List the named workbench context source policy decision values accepted by this public
 * contract.
 */
typedef enum UmiWorkbenchContextSourcePolicyDecision {
    UMI_WORKBENCH_CONTEXT_SOURCE_POLICY_ACCEPT = 1,
    UMI_WORKBENCH_CONTEXT_SOURCE_POLICY_COALESCE = 2,
    UMI_WORKBENCH_CONTEXT_SOURCE_POLICY_THROTTLE = 3,
    UMI_WORKBENCH_CONTEXT_SOURCE_POLICY_REJECT = 4
} UmiWorkbenchContextSourcePolicyDecision;

/**
 * Provide the workbench context source bounded length operation used by this module and
 * its client applications.
 */
size_t umi_workbench_context_source_bounded_length(
    const char *text,
    size_t capacity);
/**
 * Check that workbench context source text satisfies its contract before another service
 * relies on it.
 */
bool umi_workbench_context_source_text_is_valid(
    const char *text,
    size_t capacity);
/**
 * Provide the workbench context source copy text operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_context_source_copy_text(
    char *destination,
    size_t capacity,
    const char *source);
/**
 * Provide the workbench context source hash text operation used by this module and its
 * client applications.
 */
uint64_t umi_workbench_context_source_hash_text(
    uint64_t hash,
    const char *text,
    size_t capacity);
/**
 * Provide the workbench context source kind text operation used by this module and its
 * client applications.
 */
const char *umi_workbench_context_source_kind_text(
    UmiWorkbenchContextSourceKind kind);
/**
 * Provide the workbench context source trigger text operation used by this module and its
 * client applications.
 */
const char *umi_workbench_context_source_trigger_text(
    UmiWorkbenchContextSourceTrigger trigger);
/**
 * Provide the workbench context source state text operation used by this module and its
 * client applications.
 */
const char *umi_workbench_context_source_state_text(
    UmiWorkbenchContextSourceState state);

#ifdef __cplusplus
}
#endif
#endif
