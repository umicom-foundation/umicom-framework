/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/navigation_history.h
 *
 * PURPOSE:
 *   Provide browser-style source navigation history with deterministic back,
 *   forward, branch truncation and adjacent-location coalescing behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_NAVIGATION_HISTORY_H
#define UMICOM_EDITOR_NAVIGATION_HISTORY_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/editor/source_location.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_NAVIGATION_HISTORY_API_VERSION 1U
#define UMI_EDITOR_NAVIGATION_HISTORY_DEFAULT_CAPACITY 256U

typedef struct UmiEditorNavigationHistory UmiEditorNavigationHistory;

typedef struct UmiEditorNavigationHistorySnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t count;
    size_t current_index;
    size_t capacity;
    uint64_t revision;
    int has_current;
    int can_go_back;
    int can_go_forward;
} UmiEditorNavigationHistorySnapshot;

UmiStatus umi_editor_navigation_history_create(
    size_t capacity,
    UmiEditorNavigationHistory **out_history);
void umi_editor_navigation_history_destroy(
    UmiEditorNavigationHistory *history);
UmiStatus umi_editor_navigation_history_record(
    UmiEditorNavigationHistory *history,
    const UmiEditorSourceLocation *location);
UmiStatus umi_editor_navigation_history_replace_current(
    UmiEditorNavigationHistory *history,
    const UmiEditorSourceLocation *location);
UmiStatus umi_editor_navigation_history_current(
    const UmiEditorNavigationHistory *history,
    UmiEditorSourceLocation *out_location);
UmiStatus umi_editor_navigation_history_go_back(
    UmiEditorNavigationHistory *history,
    UmiEditorSourceLocation *out_location);
UmiStatus umi_editor_navigation_history_go_forward(
    UmiEditorNavigationHistory *history,
    UmiEditorSourceLocation *out_location);
UmiStatus umi_editor_navigation_history_at(
    const UmiEditorNavigationHistory *history,
    size_t index,
    UmiEditorSourceLocation *out_location);
UmiStatus umi_editor_navigation_history_clear(
    UmiEditorNavigationHistory *history);
UmiStatus umi_editor_navigation_history_snapshot(
    const UmiEditorNavigationHistory *history,
    UmiEditorNavigationHistorySnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_NAVIGATION_HISTORY_H */
