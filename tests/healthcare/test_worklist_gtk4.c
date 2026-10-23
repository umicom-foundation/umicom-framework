/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/healthcare/test_worklist_gtk4.c
 * PURPOSE: Exercise filtered assignment identity and reentrant native host disposal.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "umicom/healthcare/access_gtk4.h"
#include "umicom/healthcare/access_worklist.h"
#include <stdio.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #c); result = 1; goto cleanup; } } while (0)
static GtkWidget *find(GtkWidget *root, const char *name)
{
    if (!strcmp(gtk_widget_get_name(root), name)) return root;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child; child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *match = find(child, name); if (match) return match;
    }
    return NULL;
}
static bool click(GtkWidget *root, const char *name)
{
    GtkWidget *button = find(root, name);
    if (!button) return false;
    g_signal_emit_by_name(button, "clicked"); return true;
}
typedef struct Observer { GtkWidget *root; bool dispose; bool fired; } Observer;
static void observe(GObject *object, GParamSpec *spec, gpointer data)
{
    (void)object; (void)spec;
    Observer *observer = data;
    if (observer->fired) return;
    observer->fired = true;
    if (observer->dispose) g_object_run_dispose(G_OBJECT(observer->root));
    else (void)click(observer->root, "healthcare-request");
}
int main(int argc, char **argv)
{
    const char *name = argc == 2 ? argv[1] : "";
    GtkWidget *root = NULL, *retained = NULL, *summary = NULL;
    int result = 0;
    if (!gtk_init_check()) return 77;
    CHECK(UmiHealthcareAccessWorkspaceNew(&root) == UMI_STATUS_OK);
    g_object_ref_sink(root);
    CHECK(click(root, "healthcare-register"));
    CHECK(click(root, "healthcare-request")); /* index zero is not a follow-up */
    CHECK(click(root, "healthcare-request"));
    CHECK(click(root, "healthcare-no-response")); CHECK(click(root, "healthcare-unmet"));
    CHECK(click(root, "healthcare-request"));
    CHECK(click(root, "healthcare-unavailable")); CHECK(click(root, "healthcare-unmet"));
    GtkDropDown *filter = GTK_DROP_DOWN(find(root, "healthcare-worklist-filter"));
    GtkDropDown *selector = GTK_DROP_DOWN(find(root, "healthcare-requests"));
    GtkWidget *details = find(root, "healthcare-details");
    CHECK(filter && selector && details);
    gtk_drop_down_set_selected(filter, UMI_HEALTHCARE_ACCESS_FOLLOWUP);
    CHECK(g_list_model_get_n_items(gtk_drop_down_get_model(selector)) == 2U);
    gtk_drop_down_set_selected(selector, 0U);
    if (!strcmp(name, "identity")) {
        CHECK(click(root, "healthcare-followup"));
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(details)), "STF-000001") != NULL);
        gtk_drop_down_set_selected(filter, UMI_HEALTHCARE_ACCESS_ALL);
        gtk_drop_down_set_selected(selector, 0U);
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(details)), "Not assigned") != NULL);
        gtk_drop_down_set_selected(selector, 1U);
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(details)), "STF-000001") != NULL);
    } else if (!strcmp(name, "owner")) {
        CHECK(click(root, "healthcare-followup"));
        gtk_editable_set_text(GTK_EDITABLE(find(root, "healthcare-owner-filter")), "STF-000001");
        CHECK(click(root, "healthcare-worklist-refresh"));
        CHECK(g_list_model_get_n_items(gtk_drop_down_get_model(selector)) == 1U);
        gtk_editable_set_text(GTK_EDITABLE(find(root, "healthcare-owner-filter")), "a person");
        CHECK(click(root, "healthcare-worklist-refresh"));
        CHECK(g_list_model_get_n_items(gtk_drop_down_get_model(selector)) == 1U);
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(find(root, "healthcare-feedback"))), "unchanged") != NULL);
    } else if (!strcmp(name, "selection")) {
        gtk_drop_down_set_selected(filter, UMI_HEALTHCARE_ACCESS_UNASSIGNED);
        gtk_drop_down_set_selected(selector, 0U);
        CHECK(click(root, "healthcare-followup"));
        CHECK(g_list_model_get_n_items(gtk_drop_down_get_model(selector)) == 1U);
        CHECK(gtk_drop_down_get_selected(selector) == GTK_INVALID_LIST_POSITION);
        CHECK(click(root, "healthcare-followup"));
        gtk_drop_down_set_selected(selector, 0U);
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(details)), "Not assigned") != NULL);
    } else if (!strcmp(name, "reentry") || !strcmp(name, "dispose")) {
        Observer observer = {root, !strcmp(name, "dispose"), false};
        summary = g_object_ref(find(root, "healthcare-worklist-summary"));
        retained = g_object_ref(find(root, "healthcare-request"));
        gtk_drop_down_set_selected(filter, UMI_HEALTHCARE_ACCESS_UNASSIGNED);
        gulong signal = g_signal_connect(summary, "notify::label", G_CALLBACK(observe), &observer);
        CHECK(click(root, "healthcare-followup"));
        CHECK(observer.fired);
        g_signal_handler_disconnect(summary, signal);
        if (!observer.dispose) {
            gtk_drop_down_set_selected(filter, UMI_HEALTHCARE_ACCESS_ALL);
            CHECK(g_list_model_get_n_items(gtk_drop_down_get_model(selector)) == 3U);
        } else g_signal_emit_by_name(retained, "clicked");
    } else CHECK(0 && "unknown case");
cleanup:
    if (root) g_object_unref(root);
    g_clear_object(&summary); g_clear_object(&retained);
    return result;
}
