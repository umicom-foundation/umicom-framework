/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/editor_buffer_replace_gtk4.inc
 *
 * PURPOSE:
 *   Restore a rejected native draft using ordinary GTK edits, without starting
 *   an irreversible action inside an existing user action or clearing history.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/

/* The caller suppresses its model feedback while restoring accepted text.
 * Do not call set_text here: GTK wraps it in an irreversible action, which is
 * forbidden before an outer end-user-action signal has finished dispatching.
 * Reacquire the iterator after deletion; mutating a buffer invalidates it.
 * This helper does not begin/end a group or toggle the buffer's undo policy.
 */
static void EditorReplaceTextReversibly(GtkTextBuffer *buffer,
    const char *text, int length)
{
    GtkTextIter start;
    GtkTextIter end;
    gtk_text_buffer_get_bounds(buffer, &start, &end);
    gtk_text_buffer_delete(buffer, &start, &end);
    gtk_text_buffer_get_start_iter(buffer, &start);
    gtk_text_buffer_insert(buffer, &start, text, length);
}
