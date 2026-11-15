/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/workstation/about_dialog.h
 *
 * PURPOSE:
 *   Present product metadata through a shared branded native About window with explicit ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_WORKSTATION_ABOUT_DIALOG_H
#define UMICOM_UI_GTK4_WORKSTATION_ABOUT_DIALOG_H
#include <gtk/gtk.h>
#include "umicom/product/branding.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiGtk4AboutDialogConfig {
    UmiProductBranding branding;
    const char *version;
    const char *comments;
    GtkLicense license;
} UmiGtk4AboutDialogConfig;

/** Create, but do not present, the existing GTK About layout. Strings are
 * borrowed only for this call and copied by GTK. Required identity/version
 * fields must be nonempty UTF-8 (at most 255 bytes); comments may be empty
 * (at most 4095 bytes). Invalid metadata changes no window or output.
 *
 * The embedded approved SVG icon uses Framework's shared decoder. Missing
 * SVG support returns IO_ERROR, never substitute art. The initial foreground
 * chooses its light/dark variant; close and reopen after changing the theme.
 * The existing window identity service supplies packaged SVG/native ICO
 * integration where the platform supports it.
 *
 * parent may be NULL for legacy independent windows. Otherwise it must be a
 * live window; the dialog is modal, transient and destroyed with that parent.
 * *out_window must be NULL. Success gives an owned reference: destroy the
 * window when finished, then unref it. Native Close destroys GTK's window
 * ownership but does not release the caller's reference. A retained destroyed
 * parent returns CANCELLED. Call on the GTK owning thread. */
UmiStatus UmiGtk4AboutDialogCreate(GtkWindow *parent,
    const UmiGtk4AboutDialogConfig *config, GtkWindow **out_window);

/** Present one About window per live parent, reusing its weakly cached window
 * until closed. A new call after closure creates a new window, even if another
 * observer retained the old GObject. Different metadata on a live cached
 * window returns INVALID_STATE; close it before changing product identity.
 * NULL parent preserves independent legacy presentation without a cache.
 *
 * Synchronous reentry for the same parent is BUSY. Closure, hiding or changed
 * ownership during presentation returns CANCELLED. No application pointer is
 * retained. OK means visible on return, not desktop focus or qualification.
 * No website or model/network operation is performed by this service. */
UmiStatus UmiGtk4AboutDialogPresent(GtkWindow *parent,
    const UmiGtk4AboutDialogConfig *config);
#ifdef __cplusplus
}
#endif
#endif
