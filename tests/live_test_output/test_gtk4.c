/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/live_test_output/test_gtk4.c
 * PURPOSE: Check test output controls, shared display repair and independent widget lifetime.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/testing/ctest_output_gtk4.h"
#include "umicom/ui/gtk4/output_view.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(x))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x);                                          \
            return 1;                                                                                        \
        }                                                                                                    \
    } while (0)
static GtkWidget *Find(GtkWidget *root, const char *name)
{
    const char *tag = g_object_get_data(G_OBJECT(root), "umicom-automation-id");
    if (tag != NULL && strcmp(tag, name) == 0)
        return root;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child != NULL;
         child = gtk_widget_get_next_sibling(child))
    {
        GtkWidget *found = Find(child, name);
        if (found != NULL)
            return found;
    }
    return NULL;
}
static char *Text(GtkWidget *widget)
{
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget));
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(buffer, &start, &end);
    return gtk_text_buffer_get_text(buffer, &start, &end, TRUE);
}
typedef struct ClipboardReply
{
    gboolean done;
    char *text;
    GError *error;
} ClipboardReply;
static void ClipboardRead(GObject *source, GAsyncResult *result, gpointer data)
{
    ClipboardReply *reply = data;
    reply->text = gdk_clipboard_read_text_finish(GDK_CLIPBOARD(source), result, &reply->error);
    reply->done = TRUE;
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    (void)g_setenv("GTK_A11Y", "test", TRUE);
    if (!gtk_init_check())
        return 77;
    const char *name = argv[1];
    GtkWidget *panel = UmiCtestOutputGtk4Create();
    g_object_ref_sink(panel);
    GtkWidget *text = Find(panel, "tests.output.text"), *follow = Find(panel, "tests.output.follow");
    GtkWidget *refresh = Find(panel, "tests.output.refresh"), *copy = Find(panel, "tests.output.copy");
    GtkWidget *label = Find(panel, "tests.output.status");
    CHECK(GTK_IS_TEXT_VIEW(text) && GTK_IS_CHECK_BUTTON(follow) && GTK_IS_BUTTON(refresh) &&
          GTK_IS_BUTTON(copy) && GTK_IS_LABEL(label));
    UmiCtestOutputSnapshot *snapshot = g_new0(UmiCtestOutputSnapshot, 1);
    CHECK(UmiCtestOutputGtk4Update(panel, snapshot) == UMI_STATUS_OK);
    snapshot->task_id = 1U;
    snapshot->revision = 1U;
    snapshot->invocation = 1U;
    snapshot->attempt = 1U;
    strcpy(snapshot->name, "notes.test");
    strcpy(snapshot->test_id, "notes.test");
    snapshot->result_status = UMI_STATUS_BUSY;
    snapshot->tail.length = 5U;
    snapshot->tail.total_bytes = 5U;
    strcpy(snapshot->bytes, "first");
    CHECK(UmiCtestOutputGtk4Update(panel, snapshot) == UMI_STATUS_OK);
    char *displayed = Text(text);
    CHECK(strcmp(displayed, "first") == 0);
    g_free(displayed);
    if (strcmp(name, "pause") == 0 || strcmp(name, "refresh") == 0 || strcmp(name, "resume") == 0 ||
        strcmp(name, "copy") == 0)
    {
        gtk_check_button_set_active(GTK_CHECK_BUTTON(follow), FALSE);
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text));
        GtkTextIter start, end;
        gtk_text_buffer_get_bounds(buffer, &start, &end);
        gtk_text_buffer_select_range(buffer, &start, &end);
        snapshot->revision++;
        snapshot->tail.length = 6U;
        snapshot->tail.total_bytes = 6U;
        strcpy(snapshot->bytes, "second");
        CHECK(UmiCtestOutputGtk4Update(panel, snapshot) == UMI_STATUS_OK);
        displayed = Text(text);
        CHECK(strcmp(displayed, "first") == 0);
        g_free(displayed);
        CHECK(gtk_text_buffer_get_has_selection(buffer));
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(label)), "newer output available") != NULL);
        if (strcmp(name, "refresh") == 0 || strcmp(name, "resume") == 0)
        {
            if (strcmp(name, "refresh") == 0)
                g_signal_emit_by_name(refresh, "clicked");
            else
                gtk_check_button_set_active(GTK_CHECK_BUTTON(follow), TRUE);
            displayed = Text(text);
            CHECK(strcmp(displayed, "second") == 0);
            g_free(displayed);
            CHECK(gtk_check_button_get_active(GTK_CHECK_BUTTON(follow)) == (strcmp(name, "resume") == 0));
        }
        else if (strcmp(name, "copy") == 0)
        {
            ClipboardReply reply = {0};
            g_signal_emit_by_name(copy, "clicked");
            gdk_clipboard_read_text_async(gtk_widget_get_clipboard(panel), NULL, ClipboardRead, &reply);
            for (unsigned i = 0U; !reply.done && i < 5000U; ++i)
            {
                while (g_main_context_iteration(NULL, FALSE))
                {
                }
                g_usleep(1000U);
            }
            CHECK(reply.done && reply.error == NULL && reply.text != NULL &&
                  strcmp(reply.text, "first") == 0);
            g_free(reply.text);
            g_clear_error(&reply.error);
        }
    }
    else if (strcmp(name, "binary") == 0 || strcmp(name, "split-utf8") == 0)
    {
        snapshot->revision++;
        if (strcmp(name, "binary") == 0)
        {
            const char raw[] = {'x', '\0', 'y', '\xff', '\0'};
            memcpy(snapshot->bytes, raw, sizeof(raw));
            snapshot->tail.length = 4U;
            snapshot->tail.total_bytes = 4U;
        }
        else
        {
            strcpy(snapshot->bytes, "caf\xc3");
            snapshot->tail.length = 4U;
            snapshot->tail.total_bytes = 4U;
        }
        CHECK(UmiCtestOutputGtk4Update(panel, snapshot) == UMI_STATUS_OK);
        displayed = Text(text);
        CHECK(g_utf8_validate(displayed, -1, NULL));
        if (strcmp(name, "binary") == 0)
            CHECK(strcmp(displayed, "x\xe2\x90\x80y\xef\xbf\xbd") == 0);
        else
            CHECK(strcmp(displayed, "caf\xef\xbf\xbd") == 0);
        g_free(displayed);
        if (strcmp(name, "split-utf8") == 0)
        {
            snapshot->revision++;
            strcpy(snapshot->bytes, "caf\xc3\xa9");
            snapshot->tail.length = 5U;
            snapshot->tail.total_bytes = 5U;
            CHECK(UmiCtestOutputGtk4Update(panel, snapshot) == UMI_STATUS_OK);
            displayed = Text(text);
            CHECK(strcmp(displayed, "caf\xc3\xa9") == 0);
            g_free(displayed);
        }
    }
    else if (strcmp(name, "invalid") == 0)
    {
        snapshot->tail.length = sizeof(snapshot->bytes);
        CHECK(UmiCtestOutputGtk4Update(panel, snapshot) == UMI_STATUS_INVALID_ARGUMENT);
        snapshot->tail.length = 5U;
        snapshot->bytes[5] = 'x';
        CHECK(UmiCtestOutputGtk4Update(panel, snapshot) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiCtestOutputGtk4Update(NULL, snapshot) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiCtestOutputGtk4Update(panel, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        displayed = Text(text);
        CHECK(strcmp(displayed, "first") == 0);
        g_free(displayed);
    }
    else if (strcmp(name, "stale") == 0)
    {
        snapshot->revision = 2U;
        CHECK(UmiCtestOutputGtk4Update(panel, snapshot) == UMI_STATUS_OK);
        snapshot->revision = 1U;
        CHECK(UmiCtestOutputGtk4Update(panel, snapshot) == UMI_STATUS_INVALID_STATE);
        snapshot->task_id = 2U;
        CHECK(UmiCtestOutputGtk4Update(panel, snapshot) == UMI_STATUS_OK);
        snapshot->task_id = 1U;
        snapshot->revision = 10U;
        CHECK(UmiCtestOutputGtk4Update(panel, snapshot) == UMI_STATUS_INVALID_STATE);
    }
    else if (strcmp(name, "status") == 0)
    {
        snapshot->revision++;
        snapshot->attempt_complete = true;
        snapshot->result_status = UMI_STATUS_IO_ERROR;
        snapshot->tail.truncated = true;
        snapshot->tail.total_bytes = 100000U;
        CHECK(UmiCtestOutputGtk4Update(panel, snapshot) == UMI_STATUS_OK);
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(label)), "earlier bytes omitted") != NULL);
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(label)), umi_status_text(UMI_STATUS_IO_ERROR)) != NULL);
    }
    else if (strcmp(name, "retained") == 0)
    {
        g_object_ref(refresh);
        g_object_ref(follow);
        g_object_ref(copy);
        g_object_unref(panel);
        panel = NULL;
        g_signal_emit_by_name(refresh, "clicked");
        g_signal_emit_by_name(copy, "clicked");
        gtk_check_button_set_active(GTK_CHECK_BUTTON(follow), FALSE);
        g_object_unref(refresh);
        g_object_unref(follow);
        g_object_unref(copy);
    }
    else if (strcmp(name, "unchanged") == 0)
    {
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text));
        GtkTextIter start, end;
        gtk_text_buffer_get_bounds(buffer, &start, &end);
        gtk_text_buffer_select_range(buffer, &start, &end);
        CHECK(UmiCtestOutputGtk4Update(panel, snapshot) == UMI_STATUS_OK);
        CHECK(gtk_text_buffer_get_has_selection(buffer));
    }
    else if (strcmp(name, "attempt") == 0)
    {
        snapshot->revision++;
        snapshot->invocation++;
        snapshot->attempt++;
        strcpy(snapshot->name, "second.test");
        strcpy(snapshot->test_id, "second");
        strcpy(snapshot->bytes, "next");
        snapshot->tail.length = 4U;
        snapshot->tail.total_bytes = 4U;
        CHECK(UmiCtestOutputGtk4Update(panel, snapshot) == UMI_STATUS_OK);
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(label)), "second.test") != NULL);
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(label)), "attempt 2") != NULL);
        displayed = Text(text);
        CHECK(strcmp(displayed, "next") == 0);
        g_free(displayed);
    }
    else if (strcmp(name, "skipped") == 0)
    {
        snapshot->revision++;
        snapshot->attempt_complete = true;
        snapshot->result_state = UMI_TEST_STATE_SKIPPED;
        snapshot->result_status = UMI_STATUS_OK;
        CHECK(UmiCtestOutputGtk4Update(panel, snapshot) == UMI_STATUS_OK);
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(label)), "skipped; no pass") != NULL);
    }
    else if (strcmp(name, "capture-error") == 0)
    {
        snapshot->revision++;
        snapshot->capture_status = UMI_STATUS_IO_ERROR;
        CHECK(UmiCtestOutputGtk4Update(panel, snapshot) == UMI_STATUS_OK);
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(label)), "output capture failed") != NULL);
    }
    else if (strcmp(name, "saturated") == 0)
    {
        snapshot->tail.counters_saturated = true;
        strcpy(snapshot->bytes, "other");
        CHECK(UmiCtestOutputGtk4Update(panel, snapshot) == UMI_STATUS_OK);
        displayed = Text(text);
        CHECK(strcmp(displayed, "other") == 0);
        g_free(displayed);
    }
    else if (strcmp(name, "generic-invalid") == 0)
    {
        UmiOutputViewSnapshot *invalid = g_new0(UmiOutputViewSnapshot, 1);
        memset(invalid->context, 'x', sizeof(invalid->context));
        CHECK(UmiOutputViewGtk4Update(panel, invalid) == UMI_STATUS_INVALID_ARGUMENT);
        memset(invalid->context, 0, sizeof(invalid->context));
        memset(invalid->status, 'x', sizeof(invalid->status));
        CHECK(UmiOutputViewGtk4Update(panel, invalid) == UMI_STATUS_INVALID_ARGUMENT);
        g_free(invalid);
        CHECK(UmiOutputViewGtk4Create(NULL, "ready") == NULL);
        CHECK(UmiOutputViewGtk4Create("test", NULL) == NULL);
    }
    else
        return 2;
    if (panel != NULL)
        g_object_unref(panel);
    g_free(snapshot);
    return 0;
}
