/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/support/widget_diagnostics.h
 *
 * PURPOSE:
 *   Record bounded widget ownership and allocation evidence before a fatal GTK diagnostic.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_WIDGET_DIAGNOSTICS_H
#define UMICOM_TEST_WIDGET_DIAGNOSTICS_H
#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>

/* Test-only, single installation for one executable. A weak root avoids
 * changing the lifetime being tested. Never measure or dispatch events from
 * the logger: a sizing diagnostic may already be inside GTK measurement. */
static GWeakRef UmiTestDiagnosticRoot;
static GThread *UmiTestDiagnosticThread;
static gint UmiTestDiagnosticWritten;

static void UmiTestDiagnosticTree(GtkWidget *widget, unsigned depth, unsigned *remaining)
{
    if (*remaining == 0U || depth > 32U) return;
    --*remaining;
    int requestedWidth = -1, requestedHeight = -1;
    gtk_widget_get_size_request(widget, &requestedWidth, &requestedHeight);
    /* Addresses can be matched to GTK's warning. Deliberately omit entry,
     * label, buffer and tooltip text, which can contain private content. */
    fprintf(stderr, "widget depth=%u address=%p parent=%p type=%s allocation=%dx%d request=%dx%d visible=%d mapped=%d\n",
        depth, (void *)widget, (void *)gtk_widget_get_parent(widget), G_OBJECT_TYPE_NAME(widget),
        gtk_widget_get_width(widget), gtk_widget_get_height(widget), requestedWidth, requestedHeight,
        gtk_widget_get_visible(widget), gtk_widget_get_mapped(widget));
    for (GtkWidget *child = gtk_widget_get_first_child(widget); child != NULL && *remaining > 0U;
         child = gtk_widget_get_next_sibling(child))
        UmiTestDiagnosticTree(child, depth + 1U, remaining);
}

static GLogWriterOutput UmiTestDiagnosticWriter(GLogLevelFlags level,
    const GLogField *fields, gsize count, gpointer data)
{
    (void)data;
    gboolean gtkDiagnostic = FALSE;
    for (gsize i = 0U; i < count; ++i) {
        if (strcmp(fields[i].key, "GLIB_DOMAIN") == 0 && fields[i].value != NULL) {
            const char *domain = fields[i].value;
            gtkDiagnostic = fields[i].length < 0 ? strcmp(domain, "Gtk") == 0 :
                fields[i].length == 3 && memcmp(domain, "Gtk", 3U) == 0;
        }
    }
    if (gtkDiagnostic && (level & (G_LOG_LEVEL_WARNING | G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_ERROR)) &&
        g_thread_self() == UmiTestDiagnosticThread &&
        g_atomic_int_compare_and_exchange(&UmiTestDiagnosticWritten, 0, 1)) {
        GtkWidget *root = g_weak_ref_get(&UmiTestDiagnosticRoot);
        fprintf(stderr, "GTK diagnostic context: runtime=%u.%u.%u; widget limit=1024; depth limit=32\n",
            gtk_get_major_version(), gtk_get_minor_version(), gtk_get_micro_version());
        if (root != NULL) {
            unsigned remaining = 1024U;
            UmiTestDiagnosticTree(root, 0U, &remaining);
            g_object_unref(root);
        } else {
            fputs("No retained test root is available.\n", stderr);
        }
        fflush(stderr);
    }
    /* Preserve the original severity and fatal-warning policy. Capturing
     * evidence does not make the diagnostic expected or the test successful. */
    return g_log_writer_default(level, fields, count, NULL);
}

static void UmiTestWidgetDiagnosticsInstall(void)
{
    g_weak_ref_init(&UmiTestDiagnosticRoot, NULL);
    UmiTestDiagnosticThread = g_thread_self();
    g_log_set_writer_func(UmiTestDiagnosticWriter, NULL, NULL);
}

static void UmiTestWidgetDiagnosticsRoot(GtkWidget *root)
{
    g_weak_ref_set(&UmiTestDiagnosticRoot, root);
}
#endif
