/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/types.h
 *
 * PURPOSE:
 *   Define stable C23 types for structured workbench selections that can replace presentation-string routing.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_TYPES_H
#define UMICOM_WORKBENCH_SELECTION_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/context_channel/payload.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_WORKBENCH_SELECTION_ID_CAPACITY 160U
#define UMI_WORKBENCH_SELECTION_TEXT_CAPACITY 512U
#define UMI_WORKBENCH_SELECTION_PATH_CAPACITY 1024U
#define UMI_WORKBENCH_SELECTION_MAX_FIELDS 24U
#define UMI_WORKBENCH_SELECTION_MAX_ITEMS 512U
#define UMI_WORKBENCH_SELECTION_MAX_HISTORY 1024U
#define UMI_WORKBENCH_SELECTION_MAX_DIAGNOSTICS 128U

typedef enum UmiWorkbenchSelectionKind {
    UMI_WORKBENCH_SELECTION_GENERIC = 1,
    UMI_WORKBENCH_SELECTION_FILE = 2,
    UMI_WORKBENCH_SELECTION_PROJECT = 3,
    UMI_WORKBENCH_SELECTION_DIAGNOSTIC = 4,
    UMI_WORKBENCH_SELECTION_SOURCE_CONTROL_CHANGE = 5,
    UMI_WORKBENCH_SELECTION_SOURCE_CONTROL_COMMIT = 6,
    UMI_WORKBENCH_SELECTION_SOURCE_CONTROL_BRANCH = 7,
    UMI_WORKBENCH_SELECTION_TEST = 8,
    UMI_WORKBENCH_SELECTION_DEBUG_LOCATION = 9,
    UMI_WORKBENCH_SELECTION_TERMINAL = 10,
    UMI_WORKBENCH_SELECTION_AI_MESSAGE = 11,
    UMI_WORKBENCH_SELECTION_APPLICATION = 12,
    UMI_WORKBENCH_SELECTION_INSTRUMENT = 13,
    UMI_WORKBENCH_SELECTION_ACCOUNT = 14,
    UMI_WORKBENCH_SELECTION_TRADE = 15,
    UMI_WORKBENCH_SELECTION_WORKSPACE = 16,
    UMI_WORKBENCH_SELECTION_MEDIA = 17
} UmiWorkbenchSelectionKind;

typedef enum UmiWorkbenchSelectionActivation {
    UMI_WORKBENCH_SELECTION_ACTIVATION_SELECT = 1,
    UMI_WORKBENCH_SELECTION_ACTIVATION_OPEN = 2,
    UMI_WORKBENCH_SELECTION_ACTIVATION_NAVIGATE = 3,
    UMI_WORKBENCH_SELECTION_ACTIVATION_FOCUS = 4,
    UMI_WORKBENCH_SELECTION_ACTIVATION_PREVIEW = 5
} UmiWorkbenchSelectionActivation;

typedef enum UmiWorkbenchSelectionValueKind {
    UMI_WORKBENCH_SELECTION_VALUE_NONE = 0,
    UMI_WORKBENCH_SELECTION_VALUE_TEXT = 1,
    UMI_WORKBENCH_SELECTION_VALUE_INTEGER = 2,
    UMI_WORKBENCH_SELECTION_VALUE_UNSIGNED = 3,
    UMI_WORKBENCH_SELECTION_VALUE_DECIMAL = 4,
    UMI_WORKBENCH_SELECTION_VALUE_BOOLEAN = 5
} UmiWorkbenchSelectionValueKind;

typedef enum UmiWorkbenchSelectionState {
    UMI_WORKBENCH_SELECTION_STATE_CREATED = 1,
    UMI_WORKBENCH_SELECTION_STATE_RESOLVED = 2,
    UMI_WORKBENCH_SELECTION_STATE_PUBLISHED = 3,
    UMI_WORKBENCH_SELECTION_STATE_REJECTED = 4,
    UMI_WORKBENCH_SELECTION_STATE_STALE = 5
} UmiWorkbenchSelectionState;

size_t umi_workbench_selection_bounded_length(
    const char *text,
    size_t capacity);
bool umi_workbench_selection_text_is_valid(
    const char *text,
    size_t capacity);
UmiStatus umi_workbench_selection_copy_text(
    char *destination,
    size_t capacity,
    const char *source);
uint64_t umi_workbench_selection_hash_text(
    uint64_t hash,
    const char *text,
    size_t capacity);
const char *umi_workbench_selection_kind_text(
    UmiWorkbenchSelectionKind kind);
const char *umi_workbench_selection_activation_text(
    UmiWorkbenchSelectionActivation activation);
const char *umi_workbench_selection_state_text(
    UmiWorkbenchSelectionState state);

#ifdef __cplusplus
}
#endif
#endif
