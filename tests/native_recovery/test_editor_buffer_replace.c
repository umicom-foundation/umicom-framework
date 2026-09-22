/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/native_recovery/test_editor_buffer_replace.c
 * PURPOSE: Run the production replacement helper inside a real GTK user action.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include "../../adapters/gtk4/editor_buffer_replace_gtk4.inc"

#define CHECK(value) do { if (!(value)) { \
    fprintf(stderr, "Line %d: %s\n", __LINE__, #value); failed = 1; goto cleanup; \
} } while (0)

typedef struct RecoveryFixture {
    const char *accepted;
    int calls;
} RecoveryFixture;

static void RestoreRejectedAction(GtkTextBuffer *buffer, gpointer data)
{
    RecoveryFixture *fixture = data;
    GtkTextIter insert;
    GtkTextIter bound;
    ++fixture->calls;
    EditorReplaceTextReversibly(buffer, fixture->accepted, -1);
    gtk_text_buffer_get_iter_at_offset(buffer, &insert, 6);
    gtk_text_buffer_get_iter_at_offset(buffer, &bound, 3);
    gtk_text_buffer_select_range(buffer, &insert, &bound);
}

static int TextEquals(GtkTextBuffer *buffer, const char *expected)
{
    GtkTextIter start;
    GtkTextIter end;
    gtk_text_buffer_get_bounds(buffer, &start, &end);
    char *text = gtk_text_buffer_get_text(buffer, &start, &end, TRUE);
    const int same = text != NULL && strcmp(text, expected) == 0;
    g_free(text);
    return same;
}

int main(void)
{
    const char *initial = "/* \xCE\xBB \xF0\x9F\x9A\x80 Umicom Notes */";
    const char *accepted = "/* \xCE\xBB \xF0\x9F\x9A\x80 Umicom Notes */!";
    GtkTextBuffer *buffer = NULL;
    GtkTextIter insert;
    GtkTextIter bound;
    RecoveryFixture fixture = {accepted, 0};
    gulong handler = 0U;
    int failed = 0;
    if (!gtk_init_check()) {
        fprintf(stderr, "Native recovery requires a usable GTK4 display; NOT ACCEPTED\n");
        return 1;
    }
    buffer = gtk_text_buffer_new(NULL);
    CHECK(buffer != NULL);
    gtk_text_buffer_set_text(buffer, initial, -1);
    gtk_text_buffer_set_enable_undo(buffer, TRUE);
    gtk_text_buffer_get_end_iter(buffer, &insert);
    gtk_text_buffer_begin_user_action(buffer);
    gtk_text_buffer_insert(buffer, &insert, "!", 1);
    gtk_text_buffer_end_user_action(buffer);
    CHECK(TextEquals(buffer, accepted) && gtk_text_buffer_get_can_undo(buffer));
    handler = g_signal_connect(buffer, "end-user-action", G_CALLBACK(RestoreRejectedAction), &fixture);
    gtk_text_buffer_begin_user_action(buffer);
    gtk_text_buffer_get_iter_at_offset(buffer, &insert, 3);
    gtk_text_buffer_get_iter_at_offset(buffer, &bound, 6);
    gtk_text_buffer_delete(buffer, &insert, &bound);
    /* Match the production before-default end-user-action callback. The helper
     * must not use set_text or clear the earlier accepted edit's undo history. */
    gtk_text_buffer_end_user_action(buffer);
    g_signal_handler_disconnect(buffer, handler);
    handler = 0U;
    CHECK(fixture.calls == 1 && TextEquals(buffer, accepted));
    gtk_text_buffer_get_iter_at_mark(buffer, &insert, gtk_text_buffer_get_insert(buffer));
    gtk_text_buffer_get_iter_at_mark(buffer, &bound, gtk_text_buffer_get_selection_bound(buffer));
    CHECK(gtk_text_iter_get_offset(&insert) == 6 && gtk_text_iter_get_offset(&bound) == 3);
    CHECK(gtk_text_buffer_get_can_undo(buffer));
    for (unsigned attempt = 0U; attempt < 4U && !TextEquals(buffer, initial); ++attempt) {
        CHECK(gtk_text_buffer_get_can_undo(buffer));
        gtk_text_buffer_undo(buffer);
    }
    CHECK(TextEquals(buffer, initial));
    puts("PASS: grouped rollback, Unicode selection and previous undo retained");
cleanup:
    if (buffer != NULL && handler != 0U) g_signal_handler_disconnect(buffer, handler);
    g_clear_object(&buffer);
    return failed;
}
