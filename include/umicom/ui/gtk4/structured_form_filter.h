/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Native filter editing shares typed parsing with non-GTK clients. */
#ifndef UMICOM_UI_GTK4_STRUCTURED_FORM_FILTER_H
#define UMICOM_UI_GTK4_STRUCTURED_FORM_FILTER_H
#include "umicom/application/experience/structured_form_filter_text.h"
#include <gtk/gtk.h>
#ifdef __cplusplus
extern "C" {
#endif
/* Floating widget, GTK owner thread only. The immutable schema is borrowed for
 * the widget lifetime. Secret fields and non-UTF-8 display text are refused.
 * Add up to eight AND terms; select a field/operator and enter a value. Units
 * are shown from the schema. No search, storage or authorization runs here.
 * The filter-changed(uint64 revision) signal reports accepted native edits.
 * Numeric text may remain incomplete; Read reports errors without discarding it.
 * Inactive value text is retained locally when choosing a missing-state filter
 * and is never included in the returned typed filter. */
UmiStatus UmiStructuredFormFilterGtk4New(const UmiStructuredFormSchema *schema, GtkWidget **out);
uint64_t UmiStructuredFormFilterGtk4Revision(GtkWidget *widget);
/* Stale revisions, retired controls or reentrant mutations are refused. Add
 * creates an Is known term for the first field. Removing an invalid row and
 * adding another row do not require the remaining text to be valid. Set is an
 * explicit replacement of all local drafts; output callbacks see the new set.
 * These APIs do not silently turn a changed search into an admitted operation. */
UmiStatus UmiStructuredFormFilterGtk4Add(GtkWidget *widget, uint64_t expected_revision);
UmiStatus UmiStructuredFormFilterGtk4Remove(GtkWidget *widget, size_t index,
                                            uint64_t expected_revision);
UmiStatus UmiStructuredFormFilterGtk4Set(GtkWidget *widget, const UmiStructuredFormFilter *filter,
                                         uint64_t expected_revision);
/* Read is allowed from filter-changed handlers, leaves outputs unchanged on
 * error and returns the generation associated with the copied valid filter. */
UmiStatus UmiStructuredFormFilterGtk4Read(GtkWidget *widget, UmiStructuredFormFilter *out,
                                          uint64_t *revision);
void UmiStructuredFormFilterGtk4Retire(GtkWidget *widget);
#ifdef __cplusplus
}
#endif
#endif
