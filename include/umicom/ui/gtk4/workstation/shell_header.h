/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/workstation/shell_header.h
 *
 * PURPOSE:
 *   Render a reusable professional workstation shell header for Studio, Trader, media, design and system applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_WORKSTATION_SHELL_HEADER_H
#define UMICOM_UI_GTK4_WORKSTATION_SHELL_HEADER_H

#include <stdbool.h>
#include <gtk/gtk.h>

GtkWidget *umi_gtk4_ws_shell_header_create(const char *title, const char *subtitle, const char *mode_badge, bool compact);

#endif
