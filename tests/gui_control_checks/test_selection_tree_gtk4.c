/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/gui_control_checks/test_selection_tree_gtk4.c
 * PURPOSE: Check explicit selection and logical access to collapsed GTK panels.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/selection_list.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "line %d: %s\n", __LINE__, #x); failed = 1; goto finish; } } while (0)

/* Observe the completed publication, including from GTK notifications. A model
 * update must not expose row zero as an implicit business decision. */
typedef struct SelectionProbe { unsigned calls; bool inconsistent; } SelectionProbe;
static void SelectionChanged(GObject *object, GParamSpec *property, gpointer data)
{
    (void)property;
    SelectionProbe *probe = data;
    ++probe->calls;
    if (UmiGtk4SelectionListSelected(GTK_DROP_DOWN(object)) != GTK_INVALID_LIST_POSITION)
        probe->inconsistent = true;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    if (!gtk_init_check()) return 77;
    int failed = 0;
    GtkWidget *root = g_object_ref_sink(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    GtkStringList *rows = NULL;
    /* Keep callback data alive through every failure and cleanup path. */
    SelectionProbe probe = {0};
    if (strcmp(argv[1], "logical-tree") == 0) {
        GtkWidget *expander = gtk_expander_new("Details");
        GtkWidget *content = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
        GtkWidget *entry = gtk_entry_new();
        gtk_box_append(GTK_BOX(content), entry);
        gtk_expander_set_child(GTK_EXPANDER(expander), content);
        gtk_box_append(GTK_BOX(root), expander);
        CHECK(umi_gtk4_automation_tag_widget(entry, "fixture.entry") == UMI_STATUS_OK);
        CHECK(!gtk_expander_get_expanded(GTK_EXPANDER(expander)));
        CHECK(umi_gtk4_automation_find_tagged_widget(root, "fixture.entry") == entry);
        CHECK(!gtk_expander_get_expanded(GTK_EXPANDER(expander)));
        gtk_expander_set_expanded(GTK_EXPANDER(expander), TRUE);
        CHECK(umi_gtk4_automation_find_tagged_widget(root, "fixture.entry") == entry);
        GtkWidget *duplicate = gtk_entry_new();
        CHECK(umi_gtk4_automation_tag_widget(duplicate, "fixture.entry") == UMI_STATUS_OK);
        gtk_box_append(GTK_BOX(root), duplicate);
        CHECK(umi_gtk4_automation_find_tagged_widget(root, "fixture.entry") == NULL);
        gtk_box_remove(GTK_BOX(root), duplicate);
        CHECK(umi_gtk4_automation_find_tagged_widget(root, "fixture.entry") == entry);
        CHECK(umi_gtk4_automation_find_tagged_widget(root, "fixture.absent") == NULL);
        CHECK(umi_gtk4_automation_find_tagged_widget(NULL, "fixture.entry") == NULL);
    } else if (strncmp(argv[1], "named-", 6) == 0) {
        /* Compatibility names use the same bounded logical tree as tags.
         * Collapsed panels stay collapsed and duplicate names fail closed. */
        GtkWidget *expander = gtk_expander_new("Broker controls");
        GtkWidget *content = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
        GtkWidget *button = gtk_button_new_with_label("Review");
        gtk_widget_set_name(button, "fixture.named.review");
        gtk_box_append(GTK_BOX(content), button);
        gtk_expander_set_child(GTK_EXPANDER(expander), content);
        gtk_box_append(GTK_BOX(root), expander);
        CHECK(umi_gtk4_automation_find_named_widget(root, "fixture.named.review") == button);
        CHECK(!gtk_expander_get_expanded(GTK_EXPANDER(expander)));
        CHECK(umi_gtk4_automation_find_tagged_widget(root, "fixture.named.review") == NULL);
        if (strcmp(argv[1], "named-collapsed") == 0) {
            gtk_expander_set_expanded(GTK_EXPANDER(expander), TRUE);
            CHECK(umi_gtk4_automation_find_named_widget(root, "fixture.named.review") == button);
            gtk_expander_set_expanded(GTK_EXPANDER(expander), FALSE);
            CHECK(umi_gtk4_automation_find_named_widget(root, "fixture.named.review") == button);
        } else if (strcmp(argv[1], "named-duplicate") == 0) {
            GtkWidget *duplicate = gtk_button_new();
            gtk_widget_set_name(duplicate, "fixture.named.review");
            gtk_box_append(GTK_BOX(root), duplicate);
            CHECK(umi_gtk4_automation_find_named_widget(root, "fixture.named.review") == NULL);
            gtk_box_remove(GTK_BOX(root), duplicate);
            CHECK(umi_gtk4_automation_find_named_widget(root, "fixture.named.review") == button);
        } else if (strcmp(argv[1], "named-boundary") == 0) {
            CHECK(umi_gtk4_automation_find_named_widget(NULL, "fixture.named.review") == NULL);
            CHECK(umi_gtk4_automation_find_named_widget(root, NULL) == NULL);
            CHECK(umi_gtk4_automation_find_named_widget(root, "") == NULL);
            CHECK(umi_gtk4_automation_find_named_widget(root, "fixture.missing") == NULL);
            gtk_widget_set_sensitive(button, FALSE);
            CHECK(umi_gtk4_automation_find_named_widget(root, "fixture.named.review") == button);
            CHECK(!gtk_widget_get_sensitive(button));
            /* A sibling top level is never included in a scoped lookup. */
            GtkWidget *other = g_object_ref_sink(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
            GtkWidget *foreign = gtk_button_new();
            gtk_widget_set_name(foreign, "fixture.foreign");
            gtk_box_append(GTK_BOX(other), foreign);
            CHECK(umi_gtk4_automation_find_named_widget(root, "fixture.foreign") == NULL);
            g_object_unref(other);
        } else failed = 1;
    } else if (strcmp(argv[1], "explicit-selection") == 0) {
        GtkWidget *picker = gtk_drop_down_new(NULL, NULL);
        gtk_box_append(GTK_BOX(root), picker);
        /* probe is owned by this function until root is released. */
        g_signal_connect(picker, "notify::selected", G_CALLBACK(SelectionChanged), &probe);
        g_signal_connect(picker, "notify::model", G_CALLBACK(SelectionChanged), &probe);
        const char *labels[] = {"First record", "Second record", NULL};
        rows = gtk_string_list_new(labels);
        CHECK(UmiGtk4SelectionListPublish(GTK_DROP_DOWN(picker), rows, GTK_INVALID_LIST_POSITION) == UMI_STATUS_OK);
        CHECK(probe.calls != 0U && !probe.inconsistent);
        g_signal_handlers_disconnect_by_data(picker, &probe);
        CHECK(g_list_model_get_n_items(G_LIST_MODEL(rows)) == 3U);
        CHECK(UmiGtk4SelectionListSelected(GTK_DROP_DOWN(picker)) == GTK_INVALID_LIST_POSITION);
        gtk_drop_down_set_selected(GTK_DROP_DOWN(picker), 0U);
        CHECK(UmiGtk4SelectionListSelected(GTK_DROP_DOWN(picker)) == 0U);
        gtk_drop_down_set_selected(GTK_DROP_DOWN(picker), 1U);
        CHECK(UmiGtk4SelectionListSelected(GTK_DROP_DOWN(picker)) == 1U);
        CHECK(UmiGtk4SelectionListPublish(GTK_DROP_DOWN(picker), rows, 0U) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiGtk4SelectionListSelected(GTK_DROP_DOWN(picker)) == 1U);
        g_clear_object(&rows);
        rows = gtk_string_list_new(labels);
        CHECK(UmiGtk4SelectionListPublish(GTK_DROP_DOWN(picker), rows, 1U) == UMI_STATUS_OK);
        CHECK(UmiGtk4SelectionListSelected(GTK_DROP_DOWN(picker)) == 1U);
        g_clear_object(&rows);
        rows = gtk_string_list_new(NULL);
        CHECK(UmiGtk4SelectionListPublish(GTK_DROP_DOWN(picker), rows, GTK_INVALID_LIST_POSITION) == UMI_STATUS_OK);
        CHECK(gtk_drop_down_get_model(GTK_DROP_DOWN(picker)) == NULL);
        CHECK(UmiGtk4SelectionListSelected(GTK_DROP_DOWN(picker)) == GTK_INVALID_LIST_POSITION);
        /* Empty lists follow the same single-publication ownership rule. */
        CHECK(UmiGtk4SelectionListPublish(GTK_DROP_DOWN(picker), rows,
            GTK_INVALID_LIST_POSITION) == UMI_STATUS_INVALID_ARGUMENT);
    } else failed = 1;
finish:
    g_clear_object(&rows);
    g_object_unref(root);
    return failed;
}
