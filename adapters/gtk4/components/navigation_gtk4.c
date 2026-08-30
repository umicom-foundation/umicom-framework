/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/components/navigation_gtk4.c
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
#include "umicom/ui/gtk4/navigation.h"
GtkWidget *umi_gtk4_tab_host_new(const UmiUiComponentSpec*s){(void)s;return gtk_notebook_new();}
GtkWidget *umi_gtk4_stack_new(const UmiUiComponentSpec*s){(void)s;return gtk_stack_new();}
GtkWidget *umi_gtk4_stack_switcher_new(const UmiUiComponentSpec*s){(void)s;return gtk_stack_switcher_new();}
