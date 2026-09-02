/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/workstation/palette.h
 *
 * PURPOSE:
 *   Render searchable reusable component/tool palettes for visual design, media, kitchen and application-builder workflows.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_WORKSTATION_PALETTE_H
#define UMICOM_UI_GTK4_WORKSTATION_PALETTE_H

#include <gtk/gtk.h>
#include "umicom/ui/workstation/palette_model.h"

/**
 * Initialise gtk4 ws palette from caller-provided values so later operations receive a
 * known state.
 */
GtkWidget *umi_gtk4_ws_palette_create(const UmiWsPaletteModel *model);

#endif
