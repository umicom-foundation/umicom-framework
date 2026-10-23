/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/healthcare/access_gtk4.h
 * PURPOSE: Expose the reusable synthetic access workspace to thin GTK hosts.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_HEALTHCARE_ACCESS_GTK4_H
#define UMICOM_HEALTHCARE_ACCESS_GTK4_H
#include <gtk/gtk.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
/** Create a floating GTK widget with its own in-memory synthetic registry and
 * request book. Parent it in the application's window. Use only on the GTK
 * thread; closing the widget discards its demonstration records. */
UmiStatus UmiHealthcareAccessWorkspaceNew(GtkWidget **outWidget);
/** Owner-thread query for an accepted operation or notification publication.
 * Archive-enabled builds offer explicit Open/Save; in-memory records still
 * disappear on close. Busy rejects further mutations until completion. An
 * accepted write may finish after disposal; a late open never replaces a closed
 * session. Returns false for NULL, another widget type, or a disposed root. */
bool UmiHealthcareAccessWorkspaceBusy(GtkWidget *widget);
#ifdef __cplusplus
}
#endif
#endif /* UMICOM_HEALTHCARE_ACCESS_GTK4_H */
