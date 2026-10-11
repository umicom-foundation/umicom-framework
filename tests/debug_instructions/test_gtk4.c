/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_instructions/test_gtk4.c
 * PURPOSE: Check instruction paging, edited requests and retained-control lifetimes without a real debugger.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/debug_instructions.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                   \
    do                                                                                             \
    {                                                                                              \
        if (!(x))                                                                                  \
        {                                                                                          \
            fprintf(stderr, "%d: %s\n", __LINE__, #x);                                             \
            failed = 1;                                                                            \
            goto done;                                                                             \
        }                                                                                          \
    } while (0)
typedef struct Host
{
    GtkWidget *panel;
    GtkWindow *window;
    unsigned captures, reads, destroyed;
    const char *mode;
    bool deny;
    int64_t bytes, offset;
    uint32_t count;
    int symbols;
} Host;
static UmiStatus Capture(void *data, UmiDebugInstructionContext *out)
{
    Host *host = data;
    ++host->captures;
    if (host->deny)
        return UMI_STATUS_PERMISSION_DENIED;
    memset(out, 0, sizeof *out);
    strcpy(out->connection.session_id, "test");
    out->connection.generation = 1U;
    out->revision = 2U;
    out->thread_id = 1U;
    out->frame_id = 0U;
    strcpy(out->reference, "0x1000");
    if (strcmp(host->mode, "invalid-context") == 0)
        memset(out->reference, 'x', sizeof out->reference);
    if (strcmp(host->mode, "close-capture") == 0)
        gtk_window_destroy(host->window);
    if (strcmp(host->mode, "edit-capture") == 0)
    {
        GtkWidget *offset =
            umi_gtk4_automation_find_tagged_widget(host->panel, "debug.instructions.offset");
        gtk_editable_set_text(GTK_EDITABLE(offset), "10");
    }
    return UMI_STATUS_OK;
}
static UmiStatus Read(void *data, const UmiDebugInstructionContext *context, int64_t bytes,
                      int64_t offset, uint32_t count, int symbols, UmiDebugInstructionPage *out)
{
    Host *host = data;
    ++host->reads;
    host->bytes = bytes;
    host->offset = offset;
    host->count = count;
    host->symbols = symbols;
    if (host->deny || strcmp(host->mode, "read-denied") == 0)
        return UMI_STATUS_PERMISSION_DENIED;
    if (context->frame_id != 0U || strcmp(context->reference, "0x1000") != 0)
        return UMI_STATUS_INVALID_STATE;
    memset(out, 0, sizeof *out);
    out->count = count;
    for (size_t i = 0U; i < count; ++i)
    {
        UmiDebugInstructionRecord *row = &out->items[i];
        strcpy(row->address, "0x1000");
        strcpy(row->text, i == 1U ? "unavailable" : "<literal instruction>");
        if (i == 1U)
            strcpy(row->presentation_hint, "invalid");
    }
    if (strcmp(host->mode, "invalid-page") == 0)
        out->count = UMI_DEBUG_INSTRUCTION_PAGE_LIMIT + 1U;
    if (strcmp(host->mode, "close-read") == 0)
        gtk_window_destroy(host->window);
    if (strcmp(host->mode, "edit-read") == 0)
    {
        GtkWidget *field =
            umi_gtk4_automation_find_tagged_widget(host->panel, "debug.instructions.bytes");
        gtk_editable_set_text(GTK_EDITABLE(field), "4");
    }
    return UMI_STATUS_OK;
}
static void Destroy(gpointer data) { ++((Host *)data)->destroyed; }
static void Drain(void)
{
    for (unsigned i = 0U; i < 30U; ++i)
    {
        while (g_main_context_iteration(NULL, FALSE))
        {
        }
        g_usleep(1000U);
    }
}
static char *Content(GtkWidget *panel)
{
    GtkWidget *view = umi_gtk4_automation_find_tagged_widget(panel, "debug.instructions.output");
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
    GtkTextIter first, last;
    gtk_text_buffer_get_bounds(buffer, &first, &last);
    return gtk_text_buffer_get_text(buffer, &first, &last, FALSE);
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *mode = argv[1],
               *cases[] = {"normal",       "paging",          "edited",       "invalid-offset",
                           "offset-limit", "denied",          "read-denied",  "hidden",
                           "retained",     "close-capture",   "close-read",   "edit-capture",
                           "edit-read",    "invalid-context", "invalid-page", "failed-refresh"};
    bool known = false;
    for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i)
        if (strcmp(mode, cases[i]) == 0)
            known = true;
    if (!known)
        return 2;
    if (!gtk_init_check())
        return 77;
    int failed = 0;
    Host host = {0};
    host.mode = mode;
    host.deny = strcmp(mode, "denied") == 0;
    char *content = NULL;
    host.panel = g_object_ref_sink(UmiGtk4DebugInstructionsCreate(Capture, Read, &host, Destroy));
    host.window = GTK_WINDOW(g_object_ref_sink(gtk_window_new()));
    gtk_window_set_child(host.window, host.panel);
    gtk_window_present(host.window);
    Drain();
    GtkWidget *refresh =
        umi_gtk4_automation_find_tagged_widget(host.panel, "debug.instructions.refresh");
    GtkWidget *read = umi_gtk4_automation_find_tagged_widget(host.panel, "debug.instructions.read");
    GtkWidget *earlier =
        umi_gtk4_automation_find_tagged_widget(host.panel, "debug.instructions.earlier");
    GtkWidget *later =
        umi_gtk4_automation_find_tagged_widget(host.panel, "debug.instructions.later");
    GtkWidget *offset =
        umi_gtk4_automation_find_tagged_widget(host.panel, "debug.instructions.offset");
    CHECK(refresh && read && earlier && later && offset && !gtk_widget_get_sensitive(read));
    if (strcmp(mode, "hidden") == 0)
        gtk_widget_set_visible(host.panel, FALSE);
    if (strcmp(mode, "retained") == 0)
        gtk_window_destroy(host.window);
    g_signal_emit_by_name(refresh, "clicked");
    bool invisible = strcmp(mode, "hidden") == 0 || strcmp(mode, "retained") == 0;
    CHECK(host.captures == (invisible ? 0U : 1U));
    bool capture_failed = invisible || host.deny || strcmp(mode, "invalid-context") == 0 ||
                          strcmp(mode, "close-capture") == 0 || strcmp(mode, "edit-capture") == 0;
    CHECK(host.reads == (capture_failed ? 0U : 1U));
    content = Content(host.panel);
    bool query_failed = capture_failed || strcmp(mode, "read-denied") == 0 ||
                        strcmp(mode, "invalid-page") == 0 || strcmp(mode, "close-read") == 0 ||
                        strcmp(mode, "edit-read") == 0;
    if (query_failed)
    {
        CHECK(content[0] == '\0' && !gtk_widget_get_sensitive(read));
        goto done;
    }
    CHECK(strstr(content, "<literal instruction>") != NULL &&
          strstr(content, "[unavailable]") != NULL);
    CHECK(host.count == 32U && host.bytes == 0 && host.offset == 0 && host.symbols == 1);
    if (strcmp(mode, "paging") == 0)
    {
        g_signal_emit_by_name(later, "clicked");
        CHECK(host.reads == 2U && host.offset == 32);
        g_signal_emit_by_name(earlier, "clicked");
        CHECK(host.reads == 3U && host.offset == 0);
        g_signal_emit_by_name(earlier, "clicked");
        CHECK(host.reads == 4U && host.offset == -32 && host.captures == 1U);
    }
    if (strcmp(mode, "edited") == 0)
    {
        gtk_editable_set_text(GTK_EDITABLE(offset), "-2");
        g_free(content);
        content = Content(host.panel);
        CHECK(content[0] == '\0' && host.reads == 1U);
        g_signal_emit_by_name(read, "clicked");
        CHECK(host.offset == -2 && host.reads == 2U);
    }
    if (strcmp(mode, "invalid-offset") == 0 || strcmp(mode, "offset-limit") == 0)
    {
        gtk_editable_set_text(GTK_EDITABLE(offset),
                              strcmp(mode, "offset-limit") == 0 ? "9007199254740991" : "1e2");
        g_signal_emit_by_name(strcmp(mode, "offset-limit") == 0 ? later : read, "clicked");
        CHECK(host.reads == 1U && !gtk_widget_get_sensitive(read));
        g_free(content);
        content = Content(host.panel);
        CHECK(content[0] == '\0');
    }
    if (strcmp(mode, "failed-refresh") == 0)
    {
        host.deny = true;
        g_signal_emit_by_name(refresh, "clicked");
        CHECK(host.captures == 2U && host.reads == 1U && !gtk_widget_get_sensitive(read));
        g_free(content);
        content = Content(host.panel);
        CHECK(content[0] == '\0');
    }
done:
    g_free(content);
    gtk_window_destroy(host.window);
    g_object_unref(host.window);
    g_object_unref(host.panel);
    if (host.destroyed != 1U)
    {
        fprintf(stderr, "destructor count %u\n", host.destroyed);
        failed = 1;
    }
    return failed;
}
