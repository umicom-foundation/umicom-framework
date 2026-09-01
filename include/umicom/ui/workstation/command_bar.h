/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/command_bar.h
 *
 * PURPOSE:
 *   Define a unified workstation command/search bar model for commands,
 *   panels, layouts, settings, symbols and AI actions. The model is independent
 *   of GTK, so desktop, mobile and web frontends can expose the same actions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_COMMAND_BAR_H
#define UMICOM_UI_WORKSTATION_COMMAND_BAR_H

#include "umicom/ui/workstation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiWsCommandScope {
    UMI_WS_COMMAND_SCOPE_ALL = 0,
    UMI_WS_COMMAND_SCOPE_COMMAND = 1,
    UMI_WS_COMMAND_SCOPE_SYMBOL = 2,
    UMI_WS_COMMAND_SCOPE_TEXT = 3,
    UMI_WS_COMMAND_SCOPE_LINE = 4,
    UMI_WS_COMMAND_SCOPE_SETTING = 5,
    UMI_WS_COMMAND_SCOPE_PANEL = 6,
    UMI_WS_COMMAND_SCOPE_AI = 7
} UmiWsCommandScope;

typedef struct UmiWsCommandBarQuery {
    UmiWsCommandScope scope;
    char text[UMI_UI_TEXT_CAPACITY];
} UmiWsCommandBarQuery;

/** Describes how much horizontal space the command bar may use. */
typedef enum UmiWsCommandBarPresentation {
    UMI_WS_COMMAND_BAR_PRESENTATION_EXPANDED = 1,
    UMI_WS_COMMAND_BAR_PRESENTATION_COMPACT = 2,
    UMI_WS_COMMAND_BAR_PRESENTATION_BUTTON = 3
} UmiWsCommandBarPresentation;

/** One application action that can be discovered from the command bar. */
typedef struct UmiWsCommandBarItem {
    char item_id[UMI_UI_ID_CAPACITY];
    char title[UMI_UI_TEXT_CAPACITY];
    char description[UMI_UI_TEXT_CAPACITY];
    char command_id[UMI_UI_ID_CAPACITY];
    char keywords[UMI_UI_TEXT_CAPACITY];
    UmiWsCommandScope scope;
    uint32_t priority;
    bool enabled;
} UmiWsCommandBarItem;

/**
 * Owns the searchable action catalogue and the current filtered view.
 *
 * Result entries are indices rather than pointers. This keeps the value safe
 * to copy and avoids dangling pointers when it is stored by a frontend.
 */
typedef struct UmiWsCommandBarModel {
    UmiWsCommandBarItem items[UMI_WS_MAX_PALETTE_ITEMS];
    size_t result_indices[UMI_WS_MAX_PALETTE_ITEMS];
    size_t count;
    size_t result_count;
    size_t selected_result;
    UmiWsCommandBarQuery query;
    UmiWsCommandBarPresentation presentation;
    uint64_t revision;
} UmiWsCommandBarModel;

/** Remove all actions and return the model to a valid empty state. */
void umi_ws_command_bar_model_init(UmiWsCommandBarModel *model);

/**
 * Add one owned action to the catalogue.
 *
 * Identifiers must be unique. Text is copied only after every field has been
 * validated, so a rejected item cannot leave a half-written catalogue entry.
 */
UmiStatus umi_ws_command_bar_model_add(
    UmiWsCommandBarModel *model,
    const char *item_id,
    const char *title,
    const char *description,
    const char *command_id,
    const char *keywords,
    UmiWsCommandScope scope,
    uint32_t priority);

/** Parse text, apply its optional scope prefix and rebuild matching results. */
UmiStatus umi_ws_command_bar_model_set_query(
    UmiWsCommandBarModel *model,
    const char *input);

/** Enable or disable an existing action without removing its documentation. */
UmiStatus umi_ws_command_bar_model_set_enabled(
    UmiWsCommandBarModel *model,
    const char *item_id,
    bool enabled);

/** Return a borrowed result item, or `NULL` when the index is out of range. */
const UmiWsCommandBarItem *umi_ws_command_bar_model_result_at(
    const UmiWsCommandBarModel *model,
    size_t result_index);

/** Move selection by a signed number of rows while staying inside the list. */
UmiStatus umi_ws_command_bar_model_move_selection(
    UmiWsCommandBarModel *model,
    int32_t offset);

/** Return the borrowed selected action, or `NULL` when there is no result. */
const UmiWsCommandBarItem *umi_ws_command_bar_model_selected(
    const UmiWsCommandBarModel *model);

/** Choose an expanded, compact or button presentation for a pixel width. */
UmiWsCommandBarPresentation umi_ws_command_bar_presentation_for_width(
    int32_t available_width);

/** Store the presentation selected for a new amount of available space. */
UmiStatus umi_ws_command_bar_model_set_available_width(
    UmiWsCommandBarModel *model,
    int32_t available_width);

/** Split a user's optional prefix from the plain text that should be matched. */
UmiStatus umi_ws_command_bar_parse(
    const char *input,
    UmiWsCommandBarQuery *out_query);

/** Return the one-character prefix used to request a particular scope. */
char umi_ws_command_bar_scope_prefix(UmiWsCommandScope scope);

#ifdef __cplusplus
}
#endif

#endif
