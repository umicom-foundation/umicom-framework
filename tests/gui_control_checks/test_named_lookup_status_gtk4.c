/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/gui_control_checks/test_named_lookup_status_gtk4.c
 *
 * PURPOSE:
 *   Verify named-control lookup reports absence, ambiguity and incomplete trees without changing UI state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/automation.h"
#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#define CHECK(condition) do { if (!(condition)) { fprintf(stderr, "%d: %s\n", __LINE__, #condition); failed = 1; goto done; } } while (0)

static void CountClick(GtkButton *button, gpointer context)
{
    (void)button;
    ++*(unsigned *)context;
}
int main(int argc, char **argv)
{
    const char *cases[] = {"unique", "missing", "duplicate", "collapsed", "expanded",
        "isolated", "invalid", "depth", "tag-separation"};
    if (argc != 2) return 2;
    bool known = false;
    for (size_t i = 0U; i < G_N_ELEMENTS(cases); ++i)
        known |= strcmp(argv[1], cases[i]) == 0;
    if (!known) return 2;
    if (!gtk_init_check()) return 77;
    const char *mode = argv[1];
    int failed = 0;
    unsigned calls = 0U;
    GtkWidget *root = g_object_ref_sink(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    GtkWidget *other = g_object_ref_sink(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    GtkWidget *expander = gtk_expander_new("Details");
    GtkWidget *button = gtk_button_new_with_label("Review");
    gtk_widget_set_name(button, "fixture.review");
    gtk_expander_set_child(GTK_EXPANDER(expander), button);
    gtk_box_append(GTK_BOX(root), expander);
    g_signal_connect(button, "clicked", G_CALLBACK(CountClick), &calls);
    if (strcmp(mode, "expanded") == 0)
        gtk_expander_set_expanded(GTK_EXPANDER(expander), TRUE);
    bool expanded = gtk_expander_get_expanded(GTK_EXPANDER(expander));
    if (strcmp(mode, "duplicate") == 0) {
        GtkWidget *duplicate = gtk_label_new("Later match");
        gtk_widget_set_name(duplicate, "fixture.review");
        gtk_box_append(GTK_BOX(root), duplicate);
    }
    if (strcmp(mode, "depth") == 0) {
        /* A match near the root does not establish uniqueness in an
         * incompletely inspected tree. Failure must clear the output. */
        GtkWidget *parent = root;
        for (unsigned i = 0U; i < 260U; ++i) {
            GtkWidget *child = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
            gtk_box_append(GTK_BOX(parent), child);
            parent = child;
        }
    }
    if (strcmp(mode, "tag-separation") == 0) {
        GtkWidget *tagged = gtk_label_new("Tagged separately");
        CHECK(umi_gtk4_automation_tag_widget(tagged, "fixture.review") == UMI_STATUS_OK);
        gtk_box_append(GTK_BOX(root), tagged);
        CHECK(umi_gtk4_automation_find_tagged_widget(root, "fixture.review") == tagged);
    }
    void *found = button;
    GtkWidget *scope = strcmp(mode, "isolated") == 0 ? other : root;
    const char *name = strcmp(mode, "missing") == 0 ? "fixture.absent" : "fixture.review";
    UmiStatus expected = strcmp(mode, "duplicate") == 0 ? UMI_STATUS_ALREADY_EXISTS :
        strcmp(mode, "depth") == 0 ? UMI_STATUS_CAPACITY_EXCEEDED :
        (strcmp(mode, "missing") == 0 || strcmp(mode, "isolated") == 0)
            ? UMI_STATUS_NOT_FOUND : UMI_STATUS_OK;
    CHECK(umi_gtk4_automation_find_named_widget_checked(scope, name, &found) == expected);
    CHECK(found == (expected == UMI_STATUS_OK ? button : NULL));
    CHECK(umi_gtk4_automation_find_named_widget(scope, name) == found);
    CHECK(gtk_expander_get_expanded(GTK_EXPANDER(expander)) == expanded);
    CHECK(calls == 0U);
    if (strcmp(mode, "invalid") == 0) {
        CHECK(umi_gtk4_automation_find_named_widget_checked(root, name, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        found = button;
        CHECK(umi_gtk4_automation_find_named_widget_checked(NULL, name, &found) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(found == NULL);
        found = button;
        CHECK(umi_gtk4_automation_find_named_widget_checked(root, NULL, &found) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(found == NULL);
        found = button;
        CHECK(umi_gtk4_automation_find_named_widget_checked(root, "", &found) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(found == NULL);
    }
done:
    g_clear_object(&other);
    g_clear_object(&root);
    return failed;
}
