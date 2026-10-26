/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/focus_handoff_private.h
 *
 * PURPOSE:
 *   Finish native text focus before disabling or moving an owned widget subtree.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_GTK4_FOCUS_HANDOFF_PRIVATE_H
#define UMICOM_GTK4_FOCUS_HANDOFF_PRIVATE_H
#include <gtk/gtk.h>

/* Call on the GTK thread after publishing the caller's busy/retired/moving
 * guard. Focus notifications may synchronously retire or detach the caller.
 * Keep the subtree and its old root alive across those notifications, clear
 * only focus owned by this subtree, and let the caller recheck its lifetime
 * guard before continuing. Unrelated editors retain their keyboard focus. */
static inline void UmiGtk4ReleaseSubtreeFocus(GtkWidget *subtree)
{
  if (!GTK_IS_WIDGET(subtree))
    return;
  g_object_ref(subtree);
  GtkRoot *root = gtk_widget_get_root(subtree);
  if (root != NULL) {
    g_object_ref(root);
    GtkWidget *focus = gtk_root_get_focus(root);
    if (focus != NULL &&
        (focus == subtree || gtk_widget_is_ancestor(focus, subtree)))
      gtk_root_set_focus(root, NULL);
    g_object_unref(root);
  }
  g_object_unref(subtree);
}
#endif
