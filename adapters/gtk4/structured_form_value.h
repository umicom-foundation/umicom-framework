/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/structured_form_value.h
 *
 * PURPOSE:
 *   Private native value controls; shared answer validation stays in application.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_GTK4_STRUCTURED_FORM_VALUE_H
#define UMICOM_GTK4_STRUCTURED_FORM_VALUE_H
#include "umicom/application/experience/structured_form_text.h"
#include <gtk/gtk.h>
/* Boolean and choice controls reserve row zero for an explicit non-answer.
 * Stored answers use schema identifiers, never labels or display positions.
 * Reading the non-answer row refuses the capture without changing output. */
GtkWidget *umi_structured_form_value_new(const UmiStructuredFormSchema *schema, size_t index,
                                         const UmiFormAnswer *answer);
UmiStatus umi_structured_form_value_read(GtkWidget *widget, char *out, size_t capacity);
#endif
