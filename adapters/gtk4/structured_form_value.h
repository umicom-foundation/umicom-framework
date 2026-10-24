/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Private native value controls; shared answer validation stays in application. */
#ifndef UMICOM_GTK4_STRUCTURED_FORM_VALUE_H
#define UMICOM_GTK4_STRUCTURED_FORM_VALUE_H
#include "umicom/application/experience/structured_form_text.h"
#include <gtk/gtk.h>
GtkWidget *umi_structured_form_value_new(const UmiStructuredFormSchema *schema, size_t index,
                                         const UmiFormAnswer *answer);
UmiStatus umi_structured_form_value_read(GtkWidget *widget, char *out, size_t capacity);
#endif
