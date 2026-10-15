/*-----------------------------------------------------------------------------
 * Umicom Framework
 * PURPOSE: Expose the reusable synthetic access workspace to thin GTK hosts.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_HEALTHCARE_ACCESS_GTK4_H
#define UMICOM_HEALTHCARE_ACCESS_GTK4_H
#include <gtk/gtk.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
/** Create a floating GTK widget with its own in-memory synthetic registry and
 * request book. Parent it in the application's window. Use only on the GTK
 * thread; closing the widget discards its demonstration records. */
UmiStatus UmiHealthcareAccessWorkspaceNew(GtkWidget **outWidget);
#ifdef __cplusplus
}
#endif
#endif /* UMICOM_HEALTHCARE_ACCESS_GTK4_H */
