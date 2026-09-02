/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/components/controls_gtk4.c
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

/*
 * This file keeps one responsibility small and explicit. Read the public
 * structure/function declarations first, then follow the implementation in
 * the matching source file.
 */
#include "umicom/ui/gtk4/controls.h"
/* Provide the gtk4 button new operation used by this module and its client applications. */
GtkWidget *umi_gtk4_button_new(const UmiUiComponentSpec*s){return gtk_button_new_with_label(s&&s->text[0]?s->text:"");}
/* Provide the gtk4 label new operation used by this module and its client applications. */
GtkWidget *umi_gtk4_label_new(const UmiUiComponentSpec*s){GtkWidget*w=gtk_label_new(s&&s->text[0]?s->text:"");gtk_label_set_xalign(GTK_LABEL(w),0.0f);return w;}
/* Provide the gtk4 entry new operation used by this module and its client applications. */
GtkWidget *umi_gtk4_entry_new(const UmiUiComponentSpec*s){GtkWidget*w=gtk_entry_new();/* Apply this branch only when its contract condition is satisfied. */ if(s&&s->text[0])gtk_editable_set_text(GTK_EDITABLE(w),s->text);return w;}
/*
 * Provide the gtk4 check button new operation used by this module and its client
 * applications.
 */
GtkWidget *umi_gtk4_check_button_new(const UmiUiComponentSpec*s){return gtk_check_button_new_with_label(s&&s->text[0]?s->text:"");}
/* Provide the gtk4 switch new operation used by this module and its client applications. */
GtkWidget *umi_gtk4_switch_new(const UmiUiComponentSpec*s){(void)s;return gtk_switch_new();}
/* Provide the gtk4 progress new operation used by this module and its client applications. */
GtkWidget *umi_gtk4_progress_new(const UmiUiComponentSpec*s){GtkWidget*w=gtk_progress_bar_new();/* Apply this branch only when its contract condition is satisfied. */ if(s){double v=s->numeric_value;/* Apply this branch only when its contract condition is satisfied. */ if(v<0.0)v=0.0;/* Apply this branch only when its contract condition is satisfied. */ if(v>1.0)v=1.0;gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(w),v);}return w;}
/* Provide the gtk4 spinner new operation used by this module and its client applications. */
GtkWidget *umi_gtk4_spinner_new(const UmiUiComponentSpec*s){(void)s;GtkWidget*w=gtk_spinner_new();gtk_spinner_start(GTK_SPINNER(w));return w;}
