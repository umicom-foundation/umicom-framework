/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/gui_control_checks/test_lookup_status_gtk4.c
 * PURPOSE: Distinguish complete logical-tree lookup from missing or partial evidence.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/automation.h"
#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%d: %s\n", __LINE__, #x); failed = 1; goto done; } } while (0)

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *mode = argv[1];
    bool large = strcmp(mode, "wide") == 0 || strcmp(mode, "wide-duplicate") == 0;
    bool depth = strcmp(mode, "depth") == 0;
    if (!large && !depth && strcmp(mode, "missing") != 0 && strcmp(mode, "invalid") != 0) return 2;
    if (!gtk_init_check()) return 77;
    int failed = 0;
    GtkWidget *root = g_object_ref_sink(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    GtkWidget *target = gtk_label_new("Requested control");
    CHECK(umi_gtk4_automation_tag_widget(target, "lookup.target") == UMI_STATUS_OK);
    gtk_box_append(GTK_BOX(root), target);
    /* Put the target first. A traversal must still reach the end to prove
     * uniqueness, rather than returning an apparently usable prefix result. */
    if (large) {
        for (unsigned i = 0U; i < 18000U; ++i)
            gtk_box_append(GTK_BOX(root), gtk_label_new("Cell"));
        if (strcmp(mode, "wide-duplicate") == 0) {
            GtkWidget *duplicate = gtk_label_new("Later duplicate");
            CHECK(umi_gtk4_automation_tag_widget(duplicate, "lookup.target") == UMI_STATUS_OK);
            gtk_box_append(GTK_BOX(root), duplicate);
        }
    }
    if (depth) {
        GtkWidget *parent = root;
        for (unsigned i = 0U; i < 260U; ++i) {
            GtkWidget *child = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
            gtk_box_append(GTK_BOX(parent), child);
            parent = child;
        }
    }
    void *found = target;
    UmiStatus status = umi_gtk4_automation_find_tagged_widget_checked(root,
        strcmp(mode, "missing") == 0 ? "absent" : "lookup.target", &found);
    UmiStatus expected = depth ? UMI_STATUS_CAPACITY_EXCEEDED :
        strcmp(mode, "wide-duplicate") == 0 ? UMI_STATUS_ALREADY_EXISTS :
        strcmp(mode, "missing") == 0 ? UMI_STATUS_NOT_FOUND : UMI_STATUS_OK;
    CHECK(status == expected);
    CHECK(found == (expected == UMI_STATUS_OK ? target : NULL));
    if (large) {
        CHECK(umi_gtk4_automation_find_tagged_widget(root, "lookup.target") == found);
        CHECK(umi_gtk4_automation_reveal_tagged_widget(root, "lookup.target") == expected);
    }
    if (strcmp(mode, "invalid") == 0) {
        CHECK(umi_gtk4_automation_find_tagged_widget_checked(NULL, "lookup.target", &found) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(found == NULL);
        found = target;
        CHECK(umi_gtk4_automation_find_tagged_widget_checked(root, "", &found) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(found == NULL);
        CHECK(umi_gtk4_automation_find_tagged_widget_checked(root, "lookup.target", NULL) == UMI_STATUS_INVALID_ARGUMENT);
    }
done:
    g_clear_object(&root);
    return failed;
}
