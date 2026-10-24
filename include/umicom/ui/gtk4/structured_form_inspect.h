/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Show an owned, read-only snapshot without granting editing authority. */
#ifndef UMICOM_UI_GTK4_STRUCTURED_FORM_INSPECT_H
#define UMICOM_UI_GTK4_STRUCTURED_FORM_INSPECT_H
#include "umicom/application/experience/structured_form.h"
#include <gtk/gtk.h>
#ifdef __cplusplus
extern "C" {
#endif
/* Validate a draft and copy its visible answers into labels. Required answers
 * may be unanswered: inspection does not imply submission or factual approval.
 * Known values retain units; missing states retain reasons. Strings must be
 * UTF-8, secret schemas are refused, and no input is mutated. Output is unchanged
 * on failure. The floating widget owns its display text; inputs need remain
 * stable only until return. Calls belong to the GTK owner thread. No storage,
 * callbacks, worker scheduling, credential handling or domain decisions occur. */
UmiStatus UmiStructuredFormInspectGtk4New(const UmiStructuredFormSchema *schema,
    const UmiStructuredForm *form, GtkWidget **out);
#ifdef __cplusplus
}
#endif
#endif
