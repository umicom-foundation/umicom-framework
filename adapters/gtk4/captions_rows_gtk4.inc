/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/captions_rows_gtk4.inc
 * PURPOSE: Expose immutable caption row edits without letting stale forms overwrite a newer document.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Read at most one cue's characters before allocating its UTF-8 text. The byte
 * limit remains authoritative; a multibyte character is never cut in half. */
static char *RowText(GtkWidget *view)
{
    GtkTextBuffer *buffer = Buffer(view);
    if (gtk_text_buffer_get_char_count(buffer) >= (int)UMI_CAPTION_TEXT_CAPACITY) return NULL;
    GtkTextIter begin, end;
    gtk_text_buffer_get_bounds(buffer, &begin, &end);
    char *text = gtk_text_buffer_get_text(buffer, &begin, &end, FALSE);
    if (strlen(text) >= UMI_CAPTION_TEXT_CAPACITY) { g_free(text); return NULL; }
    return text;
}
static void LoadRow(CaptionPanel *panel, size_t index)
{
    UmiCaptionCue cue;
    UmiStatus status = UmiCaptionTrackCue(panel->track, index, &cue);
    if (status != UMI_STATUS_OK) { Message(panel, umi_status_text(status)); return; }
    char *before = Source(panel);
    if (before == NULL) { Message(panel, "The caption draft exceeds its document limit."); return; }
    panel->rowLoading = true;
    panel->rowLoaded = false;
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(panel->rowBegin), cue.begin_ms);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(panel->rowEnd), cue.end_ms);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(panel->rowSplit),
        cue.begin_ms + (cue.end_ms - cue.begin_ms) / 2U);
    Display(panel->rowText, cue.text);
    Display(panel->rowSecondText, "");
    /* Host notifications may edit the main text while the form is filled.
     * Grant a loaded identity only when it still describes that exact draft. */
    char *after = Source(panel);
    panel->rowLoaded = panel->track != NULL && after != NULL && strcmp(before, after) == 0;
    panel->loadedRow = index;
    panel->rowLoading = false;
    g_free(before); g_free(after);
    Message(panel, panel->rowLoaded ? "Caption loaded. Apply a row action to change the prepared document."
        : "The document changed while loading. Prepare it and load the caption again.");
}
static void EditRow(CaptionPanel *panel, const char *action)
{
    if (panel->track == NULL) { Message(panel, "Prepare the current captions first."); return; }
    size_t index = (size_t)gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(panel->rowNumber)) - 1U;
    if (strcmp(action, "row-load") == 0) { LoadRow(panel, index); return; }
    bool insert = strcmp(action, "row-insert") == 0;
    if (!insert && (!panel->rowLoaded || index != panel->loadedRow)) {
        Message(panel, "Load this caption again. The row number or prepared document changed.");
        return;
    }
    UmiCaptionTrack *next = NULL;
    UmiStatus status = UMI_STATUS_INVALID_ARGUMENT;
    if (strcmp(action, "row-remove") == 0)
        status = UmiCaptionTrackRemove(panel->track, index, &next);
    else if (strcmp(action, "row-merge") == 0)
        status = UmiCaptionTrackMerge(panel->track, index, &next);
    else {
        char *text = RowText(panel->rowText);
        if (text == NULL) { Message(panel, "Caption text exceeds 2048 UTF-8 bytes."); return; }
        if (strcmp(action, "row-split") == 0) {
            char *second = RowText(panel->rowSecondText);
            status = second == NULL ? UMI_STATUS_CAPACITY_EXCEEDED :
                UmiCaptionTrackSplit(panel->track, index,
                    (uint32_t)gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(panel->rowSplit)),
                    text, second, &next);
            g_free(second);
        } else if (insert || strcmp(action, "row-replace") == 0) {
            UmiCaptionCue cue = {0};
            cue.begin_ms = (uint32_t)gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(panel->rowBegin));
            cue.end_ms = (uint32_t)gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(panel->rowEnd));
            memcpy(cue.text, text, strlen(text) + 1U);
            status = insert ? UmiCaptionTrackInsert(panel->track, index, &cue, &next) :
                UmiCaptionTrackReplace(panel->track, index, &cue, &next);
        }
        g_free(text);
    }
    /* Publish owns the undo transition only after the complete new track has
     * validated and serialised. Refused edits keep both the draft and undo. */
    if (status == UMI_STATUS_OK) status = Publish(panel, &next);
    UmiCaptionTrackDestroy(next);
    if (status == UMI_STATUS_BUSY) {
        Message(panel, "The document changed during publication. Keep the current draft and prepare it again.");
    } else if (status != UMI_STATUS_OK) {
        char *text = g_strdup_printf("Caption edit: %s. Keep starts in order, use an interior split time and plain text; the prepared document was kept.",
            umi_status_text(status));
        Message(panel, text);
        g_free(text);
    }
}
