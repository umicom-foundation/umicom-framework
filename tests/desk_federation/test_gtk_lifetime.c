/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desk_federation/test_gtk_lifetime.c
 *
 * PURPOSE:
 *   Check inert GTK construction and retained callbacks after runtime invalidation.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#include "test_runtime_fixture.h"
#include "umicom/ui/gtk4/desk_federation.h"
#include <gtk/gtk.h>
static GtkWidget *FindButton(GtkWidget *root, const char *label)
{
    GtkWidget *child;
    if (GTK_IS_BUTTON(root) && g_strcmp0(gtk_button_get_label(GTK_BUTTON(root)), label) == 0) return root;
    for (child = gtk_widget_get_first_child(root); child != NULL; child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = FindButton(child, label);
        if (found != NULL) return found;
    }
    return NULL;
}
int main(void)
{
    UmiDeskRuntime *runtime = NULL;
    GtkWidget *root, *retained[8];
    size_t calls = 0U;
    const char *const labels[] = {"Open / reload saved workspaces", "Save selected as group",
        "Capture running session", "Preview saved workspace", "Preview link", "Confirm reviewed requests",
        "Remove saved entry", "Mark activity read"};
    if (!gtk_init_check()) { puts("SKIP: GTK display unavailable"); return 77; }
    REQUIRE(FixtureRuntime(&calls, &runtime) == UMI_STATUS_OK);
    root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0); g_object_ref_sink(root);
    REQUIRE(UmiDeskFederationGtkAttach(root, runtime) == UMI_STATUS_OK);
    REQUIRE(UmiDeskFederationGtkAttach(root, runtime) == UMI_STATUS_ALREADY_EXISTS);
    REQUIRE(UmiDeskFederationGtkPoll(root) == UMI_STATUS_OK);
    REQUIRE(UmiDeskFederationGtkOpenStorage(root, "relative.sqlite") == UMI_STATUS_INVALID_ARGUMENT);
    REQUIRE(calls == 0U);
    for (size_t index = 0U; index < G_N_ELEMENTS(labels); ++index) {
        retained[index] = FindButton(root, labels[index]); REQUIRE(retained[index] != NULL);
        g_object_ref(retained[index]);
    }
    UmiDeskFederationGtkInvalidate(root);
    UmiDeskFederationGtkInvalidate(root); /* Shutdown is safely repeatable. */
    umi_desk_runtime_destroy(runtime); runtime = NULL;
    REQUIRE(UmiDeskFederationGtkPoll(root) == UMI_STATUS_INVALID_STATE);
    REQUIRE(UmiDeskFederationGtkOpenStorage(root, NULL) == UMI_STATUS_INVALID_STATE);
    for (size_t index = 0U; index < G_N_ELEMENTS(labels); ++index)
        g_signal_emit_by_name(retained[index], "clicked");
    g_object_unref(root); /* Object-bound callbacks must also disconnect here. */
    for (size_t index = 0U; index < G_N_ELEMENTS(labels); ++index) {
        g_signal_emit_by_name(retained[index], "clicked"); g_object_unref(retained[index]);
    }
    REQUIRE(calls == 0U);
    puts("PASS retained GTK callbacks are inert after runtime shutdown; storage never opened");
    return 0;
}
