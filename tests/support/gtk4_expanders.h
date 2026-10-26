/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/support/gtk4_expanders.h
 *
 * PURPOSE:
 *   Open fixture sections before exercising their visible controls.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_GTK4_EXPANDERS_H
#define UMICOM_TEST_GTK4_EXPANDERS_H
#include <gtk/gtk.h>

/* Run only during fixture setup, before installing observers that can retire
 * or rebuild the tree. GtkExpander owns a collapsed child outside the widget
 * tree; reveal it through the public operation before walking descendants.
 * This models opening the section rather than changing product defaults. */
static inline void UmiTestExpandSections(GtkWidget *root)
{
  if (GTK_IS_EXPANDER(root))
    gtk_expander_set_expanded(GTK_EXPANDER(root), TRUE);
  for (GtkWidget *child = gtk_widget_get_first_child(root); child != NULL;
       child = gtk_widget_get_next_sibling(child))
    UmiTestExpandSections(child);
}
#endif
