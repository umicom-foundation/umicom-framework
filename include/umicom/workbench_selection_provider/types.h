/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/types.h
 *
 * PURPOSE:
 *   Define stable C23 contracts for adapting authoritative Framework model snapshots into structured workbench selections.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_TYPES_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/workbench_selection/workbench_selection.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY 160U
#define UMI_WORKBENCH_SELECTION_PROVIDER_TEXT_CAPACITY 512U
#define UMI_WORKBENCH_SELECTION_PROVIDER_PATH_CAPACITY 1024U
#define UMI_WORKBENCH_SELECTION_PROVIDER_MAX_PROVIDERS 128U
#define UMI_WORKBENCH_SELECTION_PROVIDER_MAX_HISTORY 2048U
#define UMI_WORKBENCH_SELECTION_PROVIDER_MAX_DIAGNOSTICS 256U
#define UMI_WORKBENCH_SELECTION_PROVIDER_MAX_RULES 256U
#define UMI_WORKBENCH_SELECTION_PROVIDER_MAX_ROWS 4096U

/**
 * List the named workbench selection provider kind values accepted by this public
 * contract.
 */
typedef enum UmiWorkbenchSelectionProviderKind {
    UMI_WORKBENCH_SELECTION_PROVIDER_GENERIC = 1,
    UMI_WORKBENCH_SELECTION_PROVIDER_PROJECT = 2,
    UMI_WORKBENCH_SELECTION_PROVIDER_FILE = 3,
    UMI_WORKBENCH_SELECTION_PROVIDER_PROBLEM = 4,
    UMI_WORKBENCH_SELECTION_PROVIDER_SOURCE_CONTROL_CHANGE = 5,
    UMI_WORKBENCH_SELECTION_PROVIDER_SOURCE_CONTROL_COMMIT = 6,
    UMI_WORKBENCH_SELECTION_PROVIDER_SOURCE_CONTROL_BRANCH = 7,
    UMI_WORKBENCH_SELECTION_PROVIDER_TEST = 8,
    UMI_WORKBENCH_SELECTION_PROVIDER_AI = 9,
    UMI_WORKBENCH_SELECTION_PROVIDER_APPLICATION = 10,
    UMI_WORKBENCH_SELECTION_PROVIDER_INSTRUMENT = 11,
    UMI_WORKBENCH_SELECTION_PROVIDER_ACCOUNT = 12,
    UMI_WORKBENCH_SELECTION_PROVIDER_TRADE = 13,
    UMI_WORKBENCH_SELECTION_PROVIDER_WORKSPACE = 14,
    UMI_WORKBENCH_SELECTION_PROVIDER_MEDIA = 15
} UmiWorkbenchSelectionProviderKind;

/**
 * List the named workbench selection provider runtime state values accepted by this public
 * contract.
 */
typedef enum UmiWorkbenchSelectionProviderRuntimeState {
    UMI_WORKBENCH_SELECTION_PROVIDER_CREATED = 1,
    UMI_WORKBENCH_SELECTION_PROVIDER_READY = 2,
    UMI_WORKBENCH_SELECTION_PROVIDER_ACTIVE = 3,
    UMI_WORKBENCH_SELECTION_PROVIDER_SUSPENDED = 4,
    UMI_WORKBENCH_SELECTION_PROVIDER_FAILED = 5,
    UMI_WORKBENCH_SELECTION_PROVIDER_STOPPED = 6
} UmiWorkbenchSelectionProviderRuntimeState;

/**
 * List the named workbench selection provider result values accepted by this public
 * contract.
 */
typedef enum UmiWorkbenchSelectionProviderResult {
    UMI_WORKBENCH_SELECTION_PROVIDER_RESULT_RESOLVED = 1,
    UMI_WORKBENCH_SELECTION_PROVIDER_RESULT_NOT_FOUND = 2,
    UMI_WORKBENCH_SELECTION_PROVIDER_RESULT_REJECTED = 3,
    UMI_WORKBENCH_SELECTION_PROVIDER_RESULT_STALE = 4,
    UMI_WORKBENCH_SELECTION_PROVIDER_RESULT_ERROR = 5
} UmiWorkbenchSelectionProviderResult;

/**
 * List the named workbench selection provider action values accepted by this public
 * contract.
 */
typedef enum UmiWorkbenchSelectionProviderAction {
    UMI_WORKBENCH_SELECTION_PROVIDER_ACTION_SELECT = 1,
    UMI_WORKBENCH_SELECTION_PROVIDER_ACTION_ACTIVATE = 2,
    UMI_WORKBENCH_SELECTION_PROVIDER_ACTION_OPEN = 3,
    UMI_WORKBENCH_SELECTION_PROVIDER_ACTION_NAVIGATE = 4,
    UMI_WORKBENCH_SELECTION_PROVIDER_ACTION_PREVIEW = 5,
    UMI_WORKBENCH_SELECTION_PROVIDER_ACTION_FOCUS = 6
} UmiWorkbenchSelectionProviderAction;

/**
 * Provide the workbench selection provider bounded length operation used by this module
 * and its client applications.
 */
size_t umi_workbench_selection_provider_bounded_length(
    const char *text,
    size_t capacity);
/**
 * Check that workbench selection provider text satisfies its contract before another
 * service relies on it.
 */
bool umi_workbench_selection_provider_text_is_valid(
    const char *text,
    size_t capacity);
/**
 * Provide the workbench selection provider copy text operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_selection_provider_copy_text(
    char *destination,
    size_t capacity,
    const char *source);
/**
 * Provide the workbench selection provider hash text operation used by this module and its
 * client applications.
 */
uint64_t umi_workbench_selection_provider_hash_text(
    uint64_t hash,
    const char *text,
    size_t capacity);
/**
 * Provide the workbench selection provider kind text operation used by this module and its
 * client applications.
 */
const char *umi_workbench_selection_provider_kind_text(
    UmiWorkbenchSelectionProviderKind kind);
/**
 * Provide the workbench selection provider state text operation used by this module and
 * its client applications.
 */
const char *umi_workbench_selection_provider_state_text(
    UmiWorkbenchSelectionProviderRuntimeState state);
/**
 * Provide the workbench selection provider result text operation used by this module and
 * its client applications.
 */
const char *umi_workbench_selection_provider_result_text(
    UmiWorkbenchSelectionProviderResult result);

#ifdef __cplusplus
}
#endif
#endif
