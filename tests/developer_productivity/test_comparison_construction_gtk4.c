/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_productivity/test_comparison_construction_gtk4.c
 * PURPOSE: Keep comparison text and labels stable when native construction reenters caller code.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/text_comparison.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%d: %s\n", __LINE__, #x); failed = 1; goto cleanup; } } while (0)
typedef struct Inputs { char left[8], right[8], left_label[16], right_label[16]; unsigned changed; } Inputs;

/* A public buffer signal provides a deterministic reentrant notification. The
 * caller changes its original storage while the comparison constructs panes. */
static gboolean ChangeInputs(GSignalInvocationHint *hint, guint count, const GValue *values, gpointer data)
{
    (void)hint; (void)count; (void)values;
    Inputs *inputs = data;
    if (inputs->changed == 0U) {
        strcpy(inputs->left, "bad\n"); strcpy(inputs->right, "bad\n");
        strcpy(inputs->left_label, "Changed left"); strcpy(inputs->right_label, "Changed right");
        inputs->changed = 1U;
    }
    return TRUE;
}
static GtkWidget *Find(GtkWidget *root, const char *id)
{
    const char *actual = g_object_get_data(G_OBJECT(root), "umicom-automation-id");
    if (actual != NULL && strcmp(actual, id) == 0) return root;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child != NULL; child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = Find(child, id); if (found != NULL) return found;
    }
    return NULL;
}
static int HasLabel(GtkWidget *root, const char *expected)
{
    if (GTK_IS_LABEL(root) && strcmp(gtk_label_get_text(GTK_LABEL(root)), expected) == 0) return 1;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child != NULL; child = gtk_widget_get_next_sibling(child))
        if (HasLabel(child, expected)) return 1;
    return 0;
}
static int HasText(GtkWidget *widget, const char *expected)
{
    if (!GTK_IS_TEXT_VIEW(widget)) return 0;
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget));
    GtkTextIter begin, end; gtk_text_buffer_get_bounds(buffer, &begin, &end);
    char *text = gtk_text_buffer_get_text(buffer, &begin, &end, TRUE);
    int same = strcmp(text, expected) == 0; g_free(text); return same;
}
int main(void)
{
    if (!gtk_init_check()) return 77;
    int failed = 0; GtkWidget *root = NULL; gulong hook = 0U;
    Inputs inputs = {"old\n", "new\n", "Captured", "Proposed", 0U};
    gpointer buffer_class = g_type_class_ref(GTK_TYPE_TEXT_BUFFER);
    guint signal = g_signal_lookup("changed", GTK_TYPE_TEXT_BUFFER);
    CHECK(signal != 0U);
    hook = g_signal_add_emission_hook(signal, 0, ChangeInputs, &inputs, NULL); CHECK(hook != 0U);
    UmiStatus status = UmiGtk4TextComparisonCreate(inputs.left, 4U, inputs.right, 4U,
        inputs.left_label, inputs.right_label, &root);
    g_signal_remove_emission_hook(signal, hook); hook = 0U;
    if (root != NULL) g_object_ref_sink(root);
    CHECK(status == UMI_STATUS_OK && root != NULL && inputs.changed == 1U);
    CHECK(HasText(Find(root, "umicom.comparison.left"), "old\n"));
    CHECK(HasText(Find(root, "umicom.comparison.right"), "new\n"));
    CHECK(HasLabel(root, "Captured") && HasLabel(root, "Proposed"));
cleanup:
    if (hook != 0U) g_signal_remove_emission_hook(signal, hook);
    g_clear_object(&root); g_type_class_unref(buffer_class); return failed;
}
