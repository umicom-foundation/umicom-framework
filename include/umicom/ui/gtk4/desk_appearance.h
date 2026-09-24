/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/desk_appearance.h
 *
 * PURPOSE:
 *   Apply a toolkit-neutral Umicom appearance profile to the GTK4 Desk
 *   presentation without moving palette values into the Desktop application.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_GTK4_DESK_APPEARANCE_H
#define UMICOM_UI_GTK4_DESK_APPEARANCE_H

#include <gtk/gtk.h>

#include "umicom/base/status.h"
#include "umicom/ui/appearance.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Apply one validated appearance snapshot to the native Desk window.
 *
 * The profile is borrowed only for this call. The generated GTK provider is
 * owned by the display and replaces the previous Desk provider on that display.
 * This function changes presentation only; application discovery, selection,
 * launch policy and layout state remain owned by the existing Desk runtime.
 */
UmiStatus umi_gtk4_desk_apply_appearance(
    GtkWindow *window,
    const UmiUiAppearanceProfile *profile);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_UI_GTK4_DESK_APPEARANCE_H */
