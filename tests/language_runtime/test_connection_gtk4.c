/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_connection_gtk4.c
 * PURPOSE: Check the native connection panel through actual widgets, worker completion and owner lifetime changes.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/language_connection.h"
#include <glib/gstdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#define CHECK(value)                                                                                         \
    do                                                                                                       \
    {                                                                                                        \
        if (!(value))                                                                                        \
        {                                                                                                    \
            fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value);                                      \
            failed = 1;                                                                                      \
            goto cleanup;                                                                                    \
        }                                                                                                    \
    } while (0)

/* The rendered-child walk omitted controls owned by collapsed expanders. The shared bounded logical-tree lookup replaces it; retain the earlier traversal for review. */
#if 0
static GtkWidget *Find(GtkWidget *widget, const char *id)
{
    const char *tag = g_object_get_data(G_OBJECT(widget), "umicom-automation-id");
    if (tag != NULL && strcmp(tag, id) == 0)
        return widget;
    for (GtkWidget *child = gtk_widget_get_first_child(widget); child != NULL;
         child = gtk_widget_get_next_sibling(child))
    {
        GtkWidget *found = Find(child, id);
        if (found != NULL)
            return found;
    }
    return NULL;
}
#endif
/* Use the Framework logical tree so a collapsed panel can be inspected
 * without changing the user's layout or overlooking an ambiguous identifier. */
static GtkWidget *Find(GtkWidget *widget, const char *id)
{
    return umi_gtk4_automation_find_tagged_widget(widget, id);
}
static int WaitIdle(GtkWidget *panel)
{
    gint64 deadline = g_get_monotonic_time() + 10 * G_TIME_SPAN_SECOND;
    while (g_object_get_data(G_OBJECT(panel), "umicom-language-check-pending") != NULL &&
           g_get_monotonic_time() < deadline)
    {
        g_main_context_iteration(NULL, FALSE);
        g_usleep(1000U);
    }
    return g_object_get_data(G_OBJECT(panel), "umicom-language-check-pending") == NULL;
}
static int WaitMapped(GtkWidget *panel)
{
    gint64 deadline = g_get_monotonic_time() + 5 * G_TIME_SPAN_SECOND;
    while (!gtk_widget_get_mapped(panel) && g_get_monotonic_time() < deadline)
    {
        g_main_context_iteration(NULL, FALSE);
        g_usleep(1000U);
    }
    return gtk_widget_get_mapped(panel);
}
static void CloseOnResult(GObject *object, GParamSpec *property, gpointer context)
{
    (void)property;
    if (strstr(gtk_label_get_text(GTK_LABEL(object)), "Handshake succeeded") != NULL)
/* The fixture retains the host window, so GTK destruction alone can retain its child. Detach the panel before destroying the host; retain the former teardown for review. */
#if 0
        gtk_window_destroy(GTK_WINDOW(context));
#endif
    {
        gtk_window_set_child(GTK_WINDOW(context), NULL);
        gtk_window_destroy(GTK_WINDOW(context));
    }
}
static int ProgramMain(int argc, char **argv)
{
    if (argc != 3)
        return 2;
    if (!gtk_init_check())
        return 77;
    const char *mode = argv[1];
    int failed = 0;
    GtkWidget *panel = NULL, *window = gtk_window_new();
    g_object_ref_sink(window);
    char *folder = g_dir_make_tmp("umicom-language-XXXXXX", NULL);
    CHECK(folder != NULL);
/* Check the copied project tool-folder draft through the new constructor while retaining coverage for the original panel API. The previous implementation is retained for engineering review. */
#if 0
    CHECK(UmiGtk4LanguageConnectionPanelCreate(folder, &panel) == UMI_STATUS_OK);
#endif
    if (strncmp(mode, "tools-", 6U) == 0)
    {
        char *toolDraft = g_path_get_dirname(argv[2]);
        CHECK(toolDraft != NULL);
        UmiStatus createStatus = UmiGtk4LanguageConnectionPanelCreateWithToolDirectory(folder, toolDraft, &panel);
        /* The form owns its initial text; later changes to caller storage cannot
         * alter what the user reviews or what a worker will capture. */
        memset(toolDraft, 'x', strlen(toolDraft));
        g_free(toolDraft);
        CHECK(createStatus == UMI_STATUS_OK);
    }
    else
        CHECK(UmiGtk4LanguageConnectionPanelCreate(folder, &panel) == UMI_STATUS_OK);
    g_object_ref_sink(panel);
    gtk_window_set_child(GTK_WINDOW(window), panel);
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 500);
    gtk_window_present(GTK_WINDOW(window));
    CHECK(WaitMapped(panel));
    GtkWidget *executable = Find(panel, "language.connection.executable");
    GtkWidget *arguments = Find(panel, "language.connection.arguments");
    GtkWidget *directory = Find(panel, "language.connection.directory");
    GtkWidget *check = Find(panel, "language.connection.check");
    GtkWidget *cancel = Find(panel, "language.connection.cancel");
    GtkWidget *message = Find(panel, "language.connection.result");
    CHECK(executable != NULL && arguments != NULL && directory != NULL && check != NULL && cancel != NULL &&
          message != NULL);
    CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(directory)), folder) == 0);
    GtkWidget *tools = Find(panel, "language.connection.tools");
    CHECK(tools != NULL);
    if (strncmp(mode, "tools-", 6U) == 0)
    {
        char *expected = g_path_get_dirname(argv[2]);
        int copied = strcmp(gtk_editable_get_text(GTK_EDITABLE(tools)), expected) == 0;
        g_free(expected);
        CHECK(copied);
    }
    else
        CHECK(gtk_editable_get_text(GTK_EDITABLE(tools))[0] == '\0');
    CHECK(!gtk_widget_get_sensitive(cancel) &&
          g_object_get_data(G_OBJECT(panel), "umicom-language-check-pending") == NULL);
    if (strcmp(mode, "initial") == 0)
    {
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(message)), "No connection") != NULL);
        goto cleanup;
    }
    if (strcmp(mode, "invalid") == 0)
    {
        g_signal_emit_by_name(check, "clicked");
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(message)), "absolute") != NULL && WaitIdle(panel));
        goto cleanup;
    }
    gtk_editable_set_text(GTK_EDITABLE(executable), argv[2]);
    gtk_editable_set_text(GTK_EDITABLE(arguments), strcmp(mode, "error") == 0    ? "lsp-error"
                                                   : strcmp(mode, "cancel") == 0 ? "lsp-timeout"
                                                                                 : "lsp");
    if (strcmp(mode, "tools-name") == 0)
        gtk_editable_set_text(GTK_EDITABLE(executable), "umicom-language-process-fixture");
    if (strcmp(mode, "tools-invalid") == 0)
    {
        gtk_editable_set_text(GTK_EDITABLE(tools), "relative/tools");
        g_signal_emit_by_name(check, "clicked");
        CHECK(WaitIdle(panel) && gtk_widget_get_sensitive(check));
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(message)), "absolute") != NULL);
        goto cleanup;
    }
    if (strcmp(mode, "retained") == 0)
    {
/* Detach host-owned content before checking that a retained panel is unrooted. Keep the earlier teardown for review. */
#if 0
        gtk_window_destroy(GTK_WINDOW(window));
#endif
        gtk_window_set_child(GTK_WINDOW(window), NULL);
        gtk_window_destroy(GTK_WINDOW(window));
        g_signal_emit_by_name(check, "clicked");
        CHECK(WaitIdle(panel) && strstr(gtk_label_get_text(GTK_LABEL(message)), "No connection") != NULL);
        goto cleanup;
    }
    if (strcmp(mode, "notify-close") == 0)
        g_signal_connect(message, "notify::label", G_CALLBACK(CloseOnResult), window);
    g_signal_emit_by_name(check, "clicked");
    CHECK(!gtk_widget_get_sensitive(check));
    if (strcmp(mode, "cancel") == 0)
        g_signal_emit_by_name(cancel, "clicked");
    if (strcmp(mode, "tools-edit") == 0 || strcmp(mode, "tools-edit-back") == 0)
    {
        char *prior = g_strdup(gtk_editable_get_text(GTK_EDITABLE(tools)));
        gtk_editable_set_text(GTK_EDITABLE(tools), "");
        if (strcmp(mode, "tools-edit-back") == 0)
            gtk_editable_set_text(GTK_EDITABLE(tools), prior);
        g_free(prior);
    }
    if (strcmp(mode, "edit") == 0 || strcmp(mode, "edit-back") == 0)
    {
        gtk_editable_set_text(GTK_EDITABLE(arguments), "different");
        if (strcmp(mode, "edit-back") == 0)
            gtk_editable_set_text(GTK_EDITABLE(arguments), "lsp");
    }
    if (strcmp(mode, "hide") == 0 || strcmp(mode, "remap") == 0)
    {
        gtk_widget_set_visible(panel, FALSE);
        if (strcmp(mode, "remap") == 0)
            gtk_widget_set_visible(panel, TRUE);
    }
/* The owner-close fixture must release GTK parentage even while its test references keep widgets alive. Detaching reproduces owner teardown and retains the same root/lifetime assertions. The previous implementation is retained for engineering review. */
#if 0
    if (strcmp(mode, "close") == 0)
        gtk_window_destroy(GTK_WINDOW(window));
#endif
    if (strcmp(mode, "close") == 0)
    {
        /* The fixture retains both window and panel. Detach before destruction
         * so this case actually exercises an unrooted, cancelled panel. */
        gtk_window_set_child(GTK_WINDOW(window), NULL);
        gtk_window_destroy(GTK_WINDOW(window));
    }
    CHECK(WaitIdle(panel));
    if (strcmp(mode, "hide") == 0)
    {
        gtk_widget_set_visible(panel, TRUE);
        CHECK(WaitMapped(panel) && gtk_widget_get_sensitive(check));
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(message)), "discarded") != NULL);
    }
    else if (strcmp(mode, "close") == 0 || strcmp(mode, "notify-close") == 0)
    {
        CHECK(gtk_widget_get_root(panel) == NULL);
        g_signal_emit_by_name(check, "clicked");
        CHECK(WaitIdle(panel));
    }
    else
    {
        CHECK(gtk_widget_get_sensitive(check) && !gtk_widget_get_sensitive(cancel));
        const char *result = gtk_label_get_text(GTK_LABEL(message));
        if (strcmp(mode, "cancel") == 0)
            CHECK(strstr(result, "cancelled") != NULL);
/* Tool-folder edits invalidate the captured worker even if the original value is restored before completion. The previous implementation is retained for engineering review. */
#if 0
        else if (strcmp(mode, "edit") == 0 || strcmp(mode, "edit-back") == 0 || strcmp(mode, "remap") == 0)
#endif
        else if (strcmp(mode, "edit") == 0 || strcmp(mode, "edit-back") == 0 || strcmp(mode, "remap") == 0 ||
                 strcmp(mode, "tools-edit") == 0 || strcmp(mode, "tools-edit-back") == 0)
            CHECK(strstr(result, "changed") != NULL || strstr(result, "discarded") != NULL);
        else if (strcmp(mode, "error") == 0)
            CHECK(strstr(result, "Connection check:") != NULL);
/* The same successful handshake assertions cover simple-name selection and an explicit absolute adapter. The previous implementation is retained for engineering review. */
#if 0
        else if (strcmp(mode, "success") == 0 || strcmp(mode, "repeat") == 0)
#endif
        else if (strcmp(mode, "success") == 0 || strcmp(mode, "repeat") == 0 ||
                 strcmp(mode, "tools-name") == 0 || strcmp(mode, "tools-absolute") == 0)
        {
            CHECK(strstr(result, "Handshake succeeded") != NULL && strstr(result, "Completion: yes") != NULL);
            if (strcmp(mode, "repeat") == 0)
            {
                g_signal_emit_by_name(check, "clicked");
                CHECK(WaitIdle(panel));
                CHECK(strstr(gtk_label_get_text(GTK_LABEL(message)), "Handshake succeeded") != NULL);
            }
        }
        else
            CHECK(0);
    }
cleanup:
    gtk_window_destroy(GTK_WINDOW(window));
    if (panel != NULL)
    {
        /* Closing requests cancellation. Wait for the actual completion before
         * releasing test-held controls; a fixed sleep is not evidence. */
        if (!WaitIdle(panel))
            failed = 1;
        g_object_unref(panel);
    }
    g_object_unref(window);
    if (folder != NULL)
    {
        (void)g_rmdir(folder);
        g_free(folder);
    }
    return failed;
}
#include "../native_process/utf8_entry.inc"
