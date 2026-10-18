/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/gui_control_checks/test_reveal_gtk4.c
 * PURPOSE: Verify explicit logical-page navigation without authorising actions.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/automation.h"
#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%d: %s\n", __LINE__, #x); failed = 1; goto done; } } while (0)

static void count_click(GtkButton *button, gpointer data)
{
    (void)button;
    ++*(unsigned *)data;
}
/* A synchronous observer may remove the destination as a section opens.
 * The shared operation must cancel rather than operate on a replacement. */
static void replace_content(GObject *object, GParamSpec *property, gpointer data)
{
    (void)property;
    if (gtk_expander_get_expanded(GTK_EXPANDER(object)))
        gtk_expander_set_child(GTK_EXPANDER(object), GTK_WIDGET(data));
}
/* Moving the original widget also invalidates a captured route, even though
 * its public identifier remains unique in the root. */
typedef struct MoveFixture {
    GtkExpander *from;
    GtkExpander *to;
    GtkWidget *button;
} MoveFixture;
static void move_content(GObject *object, GParamSpec *property, gpointer data)
{
    (void)property;
    MoveFixture *fixture = data;
    if (gtk_expander_get_expanded(GTK_EXPANDER(object))) {
        gtk_expander_set_child(fixture->from, NULL);
        gtk_expander_set_child(fixture->to, fixture->button);
    }
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *mode = argv[1];
    const char *modes[] = {"nested", "notebook", "stack", "duplicate", "missing",
        "hidden", "disabled", "replaced", "invalid", "depth", "moved"};
    bool known = false;
    for (size_t i = 0; i < G_N_ELEMENTS(modes); ++i) known |= strcmp(mode, modes[i]) == 0;
    if (!known) return 2;
    if (!gtk_init_check()) return 77;
    int failed = 0;
    unsigned clicks = 0;
    GtkWidget *root = g_object_ref_sink(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    GtkWidget *outer = gtk_expander_new("Outer"), *inner = gtk_expander_new("Inner");
    GtkWidget *button = g_object_ref_sink(gtk_button_new_with_label("Action"));
    GtkWidget *book = NULL, *stack = NULL, *destination = outer, *replacement = NULL;
    MoveFixture move = {0};
    CHECK(umi_gtk4_automation_tag_widget(button, "reveal.target") == UMI_STATUS_OK);
    g_signal_connect(button, "clicked", G_CALLBACK(count_click), &clicks);
    gtk_expander_set_child(GTK_EXPANDER(inner), button);
    gtk_expander_set_child(GTK_EXPANDER(outer), inner);
    if (strcmp(mode, "notebook") == 0) {
        book = gtk_notebook_new();
        gtk_notebook_append_page(GTK_NOTEBOOK(book), gtk_label_new("First"), gtk_label_new("First"));
        gtk_notebook_append_page(GTK_NOTEBOOK(book), outer, gtk_label_new("Target"));
        destination = book;
    } else if (strcmp(mode, "stack") == 0) {
        stack = gtk_stack_new();
        gtk_stack_add_named(GTK_STACK(stack), gtk_label_new("First"), "first");
        gtk_stack_add_named(GTK_STACK(stack), outer, "target");
        destination = stack;
    }
    gtk_box_append(GTK_BOX(root), destination);
    if (strcmp(mode, "depth") == 0) {
        /* A route over the public traversal bound must not partially expand. */
        GtkWidget *content = g_object_ref(inner);
        gtk_expander_set_child(GTK_EXPANDER(outer), NULL);
        for (unsigned i = 0; i < 260U; ++i) {
            GtkWidget *box = g_object_ref_sink(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
            gtk_box_append(GTK_BOX(box), content);
            g_object_unref(content);
            content = box;
        }
        gtk_expander_set_child(GTK_EXPANDER(outer), content);
        g_object_unref(content);
    }
    if (strcmp(mode, "duplicate") == 0) {
        GtkWidget *duplicate = gtk_button_new_with_label("Other");
        CHECK(umi_gtk4_automation_tag_widget(duplicate, "reveal.target") == UMI_STATUS_OK);
        gtk_box_append(GTK_BOX(root), duplicate);
    }
    if (strcmp(mode, "hidden") == 0) gtk_widget_set_visible(button, FALSE);
    if (strcmp(mode, "disabled") == 0) gtk_widget_set_sensitive(button, FALSE);
    if (strcmp(mode, "replaced") == 0) {
        replacement = g_object_ref_sink(gtk_button_new_with_label("Replacement"));
        CHECK(umi_gtk4_automation_tag_widget(replacement, "reveal.target") == UMI_STATUS_OK);
        g_signal_connect(outer, "notify::expanded", G_CALLBACK(replace_content), replacement);
    }
    if (strcmp(mode, "moved") == 0) {
        GtkWidget *other = gtk_expander_new("Other route");
        gtk_box_append(GTK_BOX(root), other);
        move = (MoveFixture){GTK_EXPANDER(inner), GTK_EXPANDER(other), button};
        g_signal_connect(outer, "notify::expanded", G_CALLBACK(move_content), &move);
    }
    if (strcmp(mode, "invalid") == 0) {
        CHECK(umi_gtk4_automation_reveal_tagged_widget(NULL, "reveal.target") == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_gtk4_automation_reveal_tagged_widget(root, "") == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!gtk_expander_get_expanded(GTK_EXPANDER(outer)));
        goto done;
    }
    UmiStatus status = umi_gtk4_automation_reveal_tagged_widget(root,
        strcmp(mode, "missing") == 0 ? "absent" : "reveal.target");
    UmiStatus expected = strcmp(mode, "duplicate") == 0 ? UMI_STATUS_ALREADY_EXISTS :
        strcmp(mode, "missing") == 0 ? UMI_STATUS_NOT_FOUND :
        strcmp(mode, "depth") == 0 ? UMI_STATUS_CAPACITY_EXCEEDED :
        (strcmp(mode, "replaced") == 0 || strcmp(mode, "moved") == 0) ? UMI_STATUS_CANCELLED : UMI_STATUS_OK;
    CHECK(status == expected && clicks == 0U);
    if (expected == UMI_STATUS_OK) {
        CHECK(gtk_expander_get_expanded(GTK_EXPANDER(outer)));
        CHECK(gtk_expander_get_expanded(GTK_EXPANDER(inner)));
        CHECK(umi_gtk4_automation_find_tagged_widget(root, "reveal.target") == button);
        CHECK(gtk_widget_get_visible(button) == (strcmp(mode, "hidden") != 0));
        CHECK(gtk_widget_get_sensitive(button) == (strcmp(mode, "disabled") != 0));
        if (book != NULL) CHECK(gtk_notebook_get_current_page(GTK_NOTEBOOK(book)) == 1);
        if (stack != NULL) CHECK(gtk_stack_get_visible_child(GTK_STACK(stack)) == outer);
        CHECK(umi_gtk4_automation_reveal_tagged_widget(root, "reveal.target") == UMI_STATUS_OK);
        CHECK(clicks == 0U);
    } else if (expected != UMI_STATUS_CANCELLED) {
        CHECK(!gtk_expander_get_expanded(GTK_EXPANDER(outer)));
    }
done:
    g_clear_object(&root);
    g_clear_object(&button);
    g_clear_object(&replacement);
    return failed;
}
