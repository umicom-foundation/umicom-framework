/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/media_generation/heygen_gtk4.h
 * PURPOSE: Expose a shared native avatar workflow with local credential ownership.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_MEDIA_GENERATION_HEYGEN_GTK4_H
#define UMICOM_MEDIA_GENERATION_HEYGEN_GTK4_H
#include <gtk/gtk.h>
#include "umicom/media_generation/heygen.h"
#ifdef __cplusplus
extern "C"
{
#endif
    /* GTK thread only. Creates a floating widget without opening storage or the
 * network. Scopes are copied. Workers own their plans/passwords and a weak root;
 * retained buttons become inert when the root dies. Removing the widget cannot
 * undo a remote creation. Copy a returned resource ID before closing the page.
 * The shared key manager uses the same application/profile namespace. */
    UmiStatus UmiHeyGenGtkCreate(const char *application_id, const char *profile_id, GtkWidget **out_widget);
#ifdef __cplusplus
}
#endif
#endif
