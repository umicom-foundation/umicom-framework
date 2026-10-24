/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/structured_form.h
 * PURPOSE: Compose native answer editing with the shared structured-form service.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_GTK4_STRUCTURED_FORM_H
#define UMICOM_UI_GTK4_STRUCTURED_FORM_H
#include "umicom/application/experience/structured_form.h"
#include <gtk/gtk.h>
#ifdef __cplusplus
extern "C" {
#endif
/* Main-thread only. The immutable schema remains borrowed for widget lifetime.
 * Initial answers are copied, or checked defaults are used when initial is NULL.
 * Returns a floating widget; NULL means invalid schema/answers/UTF-8 or allocation
 * failure. A detached or disposed editor is permanently retired. */
GtkWidget *UmiStructuredFormGtk4New(const UmiStructuredFormSchema *schema,
                                    const UmiStructuredForm *initial);
/* Parse the entire visible draft before applying any answer. submitting=1 also
 * checks required fields. A rejected field leaves the accepted model unchanged.
 * These functions do not persist, authorize or send the form to any service. */
UmiStatus UmiStructuredFormGtk4Apply(GtkWidget *widget, int submitting);
UmiStatus UmiStructuredFormGtk4Snapshot(GtkWidget *widget, UmiStructuredForm *out);
UmiStatus UmiStructuredFormGtk4MarkSaved(GtkWidget *widget, uint64_t revision);
/* Both unapplied text and accepted-but-unsaved edits block routine close. The
 * application owns its discard prompt; destroying the widget discards its draft. */
int UmiStructuredFormGtk4CanClose(GtkWidget *widget);
void UmiStructuredFormGtk4Retire(GtkWidget *widget);
#ifdef __cplusplus
}
#endif
#endif
