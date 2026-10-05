/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/security/gtk4/profile_keys.h
 * PURPOSE: Present explicit local-profile and provider-key management without network access.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SECURITY_GTK4_PROFILE_KEYS_H
#define UMICOM_SECURITY_GTK4_PROFILE_KEYS_H
#include <gtk/gtk.h>
#include <stdbool.h>
#include "umicom/security/profile_secrets.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiProfileKeysGtk UmiProfileKeysGtk;
/* Names are copied after ScopeValidate. Create constructs widgets only; a
 * user action opens native stores on a worker. No database receives key bytes.
 * All panel functions require the GTK owning thread. Widget is borrowed.
 * Busy rejects a normal close. Destroy immediately clears fields and retires
 * callbacks, but an operation already started may still complete its write.
 * Reopen and check the alias after forced shutdown; do not assume rollback.
 * No key is displayed, exported, or sent to a remote provider by this panel. */
UmiStatus UmiProfileKeysGtkCreate(const char *application_id, const char *profile_name,
    UmiProfileKeysGtk **out);
GtkWidget *UmiProfileKeysGtkWidget(UmiProfileKeysGtk *panel);
bool UmiProfileKeysGtkBusy(const UmiProfileKeysGtk *panel);
bool UmiProfileKeysGtkCanClose(UmiProfileKeysGtk *panel);
void UmiProfileKeysGtkDestroy(UmiProfileKeysGtk *panel);
UmiStatus UmiProfileKeysGtkPresent(GtkWindow *parent, const char *application_id,
    const char *profile_name);
#ifdef __cplusplus
}
#endif
#endif
