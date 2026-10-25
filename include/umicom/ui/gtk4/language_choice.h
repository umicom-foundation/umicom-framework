/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/language_choice.h
 *
 * PURPOSE:
 *   Select a known document language using shared immutable descriptors in native application
 * hosts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_LANGUAGE_CHOICE_H
#define UMICOM_UI_GTK4_LANGUAGE_CHOICE_H
#include "umicom/document/languages.h"
#include <gtk/gtk.h>
#ifdef __cplusplus
extern "C" {
#endif
/* Owner-thread only. Returns an unparented floating dropdown. Caller sinks or
 * parents it; it owns no coordinator and opens no files or server processes. */
UmiStatus UmiGtk4LanguageChoiceCreate(const char *language_id, GtkWidget **out);
UmiStatus UmiGtk4LanguageChoiceRead(GtkWidget *widget, const UmiDocumentLanguageChoice **out);
UmiStatus UmiGtk4LanguageChoiceSelect(GtkWidget *widget, const char *language_id);
#ifdef __cplusplus
}
#endif
#endif
