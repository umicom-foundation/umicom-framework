/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/kanban_details/native_fixture.h
 *
 * PURPOSE:
 *   Share bounded native card-form lookup and owner-context dispatch fixtures.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_KANBAN_DETAILS_NATIVE_FIXTURE_H
#define UMICOM_KANBAN_DETAILS_NATIVE_FIXTURE_H
#include "../kanban/fixture.h"
#include "../support/main_context.h"
#include "umicom/ui/gtk4/kanban.h"
static GtkWidget *Find(GtkWidget *root, const char *text)
{
    if (!strcmp(gtk_widget_get_name(root), text) ||
        (GTK_IS_BUTTON(root) && !g_strcmp0(gtk_button_get_label(GTK_BUTTON(root)), text))) return root;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child; child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = Find(child, text); if (found) return found;
    }
    return NULL;
}
static void Click(GtkWidget *root, const char *text)
{ GtkWidget *button = Find(root, text); REQUIRE(GTK_IS_BUTTON(button)); g_signal_emit_by_name(button, "clicked"); }
static void Set(GtkWidget *root, const char *name, const char *value)
{ GtkWidget *entry = Find(root, name); REQUIRE(GTK_IS_EDITABLE(entry)); gtk_editable_set_text(GTK_EDITABLE(entry), value); }
static void Drain(void)
{
    for (unsigned i = 0U; i < 30U; ++i) {
        (void)UmiTestDispatchSlice(NULL, g_get_monotonic_time() + 1000, 32U); g_usleep(1000U);
    }
}

#endif
