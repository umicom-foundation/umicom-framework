/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/workstation/command_bar.h
 *
 * PURPOSE:
 *   Render the unified command/search centre with visible scope affordances
 *   and responsive presentation for commands, panels, settings and AI.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_WORKSTATION_COMMAND_BAR_H
#define UMICOM_UI_GTK4_WORKSTATION_COMMAND_BAR_H

#include <gtk/gtk.h>
#include "umicom/ui/workstation/command_bar.h"

G_BEGIN_DECLS

typedef struct UmiGtk4WorkstationCommandBar UmiGtk4WorkstationCommandBar;

/** Values used to create one managed native command bar. */
typedef struct UmiGtk4WorkstationCommandBarConfig {
    const char *placeholder;
    const char *compact_placeholder;
    size_t maximum_visible_results;
    int32_t initial_available_width;
} UmiGtk4WorkstationCommandBarConfig;

/** Safe copied state for tests, diagnostics and thin application shells. */
typedef struct UmiGtk4WorkstationCommandBarSnapshot {
    UmiWsCommandScope scope;
    UmiWsCommandBarPresentation presentation;
    char query[UMI_UI_TEXT_CAPACITY];
    char selected_item_id[UMI_UI_ID_CAPACITY];
    size_t item_count;
    size_t result_count;
    uint64_t revision;
} UmiGtk4WorkstationCommandBarSnapshot;

/** Called after the user chooses one enabled action from the result list. */
typedef void (*UmiGtk4WorkstationCommandBarActivatedHandler)(
    const UmiWsCommandBarItem *item,
    void *user_data);

/** Return readable defaults suitable for any Umicom application shell. */
UmiGtk4WorkstationCommandBarConfig
umi_gtk4_ws_command_bar_config_default(void);

/** Create an owned native command bar from a copied portable action model. */
UmiStatus umi_gtk4_ws_command_bar_create_managed(
    const UmiGtk4WorkstationCommandBarConfig *config,
    const UmiWsCommandBarModel *model,
    UmiGtk4WorkstationCommandBar **out_command_bar);

/** Release the controller and its complete GTK widget tree. */
void umi_gtk4_ws_command_bar_destroy(
    UmiGtk4WorkstationCommandBar *command_bar);

/** Borrow the root widget for placement in an application header. */
GtkWidget *umi_gtk4_ws_command_bar_widget(
    UmiGtk4WorkstationCommandBar *command_bar);

/** Replace the action catalogue with an independent copy and refresh results. */
UmiStatus umi_gtk4_ws_command_bar_set_model(
    UmiGtk4WorkstationCommandBar *command_bar,
    const UmiWsCommandBarModel *model);

/** Set search text programmatically through the keyboard-input path. */
UmiStatus umi_gtk4_ws_command_bar_set_query_text(
    UmiGtk4WorkstationCommandBar *command_bar,
    const char *text);

/** Adapt the component without removing any registered action. */
UmiStatus umi_gtk4_ws_command_bar_set_available_width(
    UmiGtk4WorkstationCommandBar *command_bar,
    int32_t available_width);

/** Install a borrowed activation callback owned by the caller. */
UmiStatus umi_gtk4_ws_command_bar_set_activated_handler(
    UmiGtk4WorkstationCommandBar *command_bar,
    UmiGtk4WorkstationCommandBarActivatedHandler handler,
    void *user_data);

/** Copy observable command-bar state without exposing mutable GTK objects. */
UmiGtk4WorkstationCommandBarSnapshot umi_gtk4_ws_command_bar_snapshot(
    const UmiGtk4WorkstationCommandBar *command_bar);

/* These compatibility helpers retain the original lightweight entry API for
 * applications that need parsing but do not need a managed catalogue. */
GtkWidget *umi_gtk4_ws_command_bar_create(const char *placeholder);
UmiStatus umi_gtk4_ws_command_bar_query(
    GtkWidget *entry,
    UmiWsCommandBarQuery *out_query);

G_END_DECLS

#endif
