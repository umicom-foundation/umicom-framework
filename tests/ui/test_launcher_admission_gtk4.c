/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui/test_launcher_admission_gtk4.c
 *
 * PURPOSE:
 *   Exercise real finance launchers under inherited disabling, native removal, detachment and re-enabling.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ui/gtk4/forward_practice.h"
#include "umicom/ui/gtk4/accounting_practice.h"
#include "umicom/ui/gtk4/cash_plan.h"
#include "umicom/ui/gtk4/bank_feed.h"
#include <stdio.h>
#include <string.h>

#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); failed = 1; goto cleanup; } } while (0)

static GtkWidget *CreateLauncher(const char *kind)
{
    if (strcmp(kind, "forward") == 0) return UmiGtk4ForwardPracticeLauncherCreate();
    if (strcmp(kind, "accounting") == 0) return UmiGtk4AccountingPracticeLauncherCreate();
    if (strcmp(kind, "cash") == 0) return UmiGtk4CashPlanLauncherCreate();
    if (strcmp(kind, "bank") == 0) return UmiBankFeedGtk4LauncherCreate();
    return NULL;
}

/* Collect owned child references before destruction changes the toplevel model.
 * This cleanup never closes a window merely because it is in the model. */
static void CloseChildren(GtkWindow *first, GtkWindow *second)
{
    GPtrArray *children = g_ptr_array_new_with_free_func(g_object_unref);
    GListModel *windows = gtk_window_get_toplevels();
    for (guint i = 0U; i < g_list_model_get_n_items(windows); ++i) {
        GtkWindow *item = g_list_model_get_item(windows, i);
        GtkWindow *parent = gtk_window_get_transient_for(item);
        if (item != first && item != second &&
            (parent == first || (second != NULL && parent == second)))
            g_ptr_array_add(children, item);
        else g_object_unref(item);
    }
    for (guint i = 0U; i < children->len; ++i)
        gtk_window_destroy(g_ptr_array_index(children, i));
    g_ptr_array_unref(children);
}
int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    const char *kind = argv[1], *scenario = argv[2];
    const char *scenarios[] = {
        "disabled-self", "disabled-ancestor", "detached", "retired-parent",
        "reparent-disabled", "reenabled", "active"
    };
    int known = 0;
    for (size_t i = 0U; i < sizeof scenarios / sizeof scenarios[0]; ++i)
        if (strcmp(scenario, scenarios[i]) == 0) known = 1;
    if (!known || (strcmp(kind, "forward") != 0 && strcmp(kind, "accounting") != 0 &&
        strcmp(kind, "cash") != 0 && strcmp(kind, "bank") != 0)) return 2;
    (void)g_setenv("GTK_A11Y", "test", TRUE);
    if (!gtk_init_check()) return 77;
    int failed = 0;
    GtkWindow *parent = g_object_ref(GTK_WINDOW(gtk_window_new()));
    GtkWindow *other = NULL;
    GtkWidget *container = g_object_ref_sink(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    GtkWidget *otherContainer = NULL;
    GtkWidget *launcher = CreateLauncher(kind);
    CHECK(GTK_IS_BUTTON(launcher));
    g_object_ref_sink(launcher);
    gtk_window_set_child(parent, container);
    gtk_box_append(GTK_BOX(container), launcher);

    if (strcmp(scenario, "disabled-self") == 0)
        gtk_widget_set_sensitive(launcher, FALSE);
    else if (strcmp(scenario, "disabled-ancestor") == 0) {
        gtk_widget_set_sensitive(container, FALSE);
        /* A local property is insufficient: the button itself remains enabled. */
        CHECK(gtk_widget_get_sensitive(launcher));
        CHECK(!gtk_widget_is_sensitive(launcher));
    } else if (strcmp(scenario, "detached") == 0)
        gtk_box_remove(GTK_BOX(container), launcher);
    else if (strcmp(scenario, "retired-parent") == 0)
        gtk_window_destroy(parent);
    else if (strcmp(scenario, "reparent-disabled") == 0) {
        other = g_object_ref(GTK_WINDOW(gtk_window_new()));
        otherContainer = g_object_ref_sink(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
        gtk_window_set_child(other, otherContainer);
        gtk_widget_set_sensitive(otherContainer, FALSE);
        gtk_box_remove(GTK_BOX(container), launcher);
        gtk_box_append(GTK_BOX(otherContainer), launcher);
    } else if (strcmp(scenario, "reenabled") == 0) {
        gtk_widget_set_sensitive(container, FALSE);
        gtk_widget_set_sensitive(container, TRUE);
    }

    GListModel *windows = gtk_window_get_toplevels();
    const guint before = g_list_model_get_n_items(windows);
    const gboolean allowed = strcmp(scenario, "active") == 0 || strcmp(scenario, "reenabled") == 0;
    /* Deliberately bypass pointer activation to verify the callback's own guard. */
    g_signal_emit_by_name(launcher, "clicked");
    CHECK(g_list_model_get_n_items(windows) == before + (allowed ? 1U : 0U));
    if (!allowed) {
        g_signal_emit_by_name(launcher, "clicked");
        CHECK(g_list_model_get_n_items(windows) == before);
    }
cleanup:
    CloseChildren(parent, other);
    gtk_window_destroy(parent);
    if (other != NULL) gtk_window_destroy(other);
    g_clear_object(&launcher);
    g_clear_object(&container);
    g_clear_object(&otherContainer);
    g_clear_object(&parent);
    g_clear_object(&other);
    return failed;
}
