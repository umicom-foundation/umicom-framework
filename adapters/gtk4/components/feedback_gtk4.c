/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/components/feedback_gtk4.c
 *
 * PURPOSE:
 *   Implement reusable GTK4 rendering for common semantic Umicom components.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file keeps one responsibility small and explicit. Read the public
 * structure/function declarations first, then follow the implementation in
 * the matching source file.
 */
#include "umicom/ui/gtk4/feedback.h"
GtkWidget *umi_gtk4_status_message_new(const UmiUiComponentSpec*s){GtkWidget*w=gtk_label_new(s&&s->text[0]?s->text:"");gtk_widget_add_css_class(w,"dim-label");return w;}
