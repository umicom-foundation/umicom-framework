/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/workstation/command_bar.h
 *
 * PURPOSE:
 *   Render the unified command/search centre with visible scope affordances for commands, panels, settings, symbols and AI.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_WORKSTATION_COMMAND_BAR_H
#define UMICOM_UI_GTK4_WORKSTATION_COMMAND_BAR_H

#include <gtk/gtk.h>
#include "umicom/ui/workstation/command_bar.h"

GtkWidget *umi_gtk4_ws_command_bar_create(const char *placeholder);
UmiStatus umi_gtk4_ws_command_bar_query(GtkWidget *entry, UmiWsCommandBarQuery *out_query);

#endif
