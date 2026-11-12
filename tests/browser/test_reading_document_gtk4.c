/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/browser/test_reading_document_gtk4.c
 *
 * PURPOSE:
 *   Exercise native reading admission, typography, Unicode ranges and callback ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "../support/assertions.h"
#include <assert.h>
#include <math.h>
#include <string.h>
#include "umicom/ui/gtk4/reading_document.h"
/* White-box corruptions exercise the frontend boundary independently of the
 * current parser. Production callers only receive immutable opaque models. */
#include "../../src/browser/reading_document_internal.h"

typedef struct CallbackState {
    UmiBrowserReadingDocument *document;
    UmiUiAppearanceProfile *profile;
    GtkTextBuffer *buffer;
    unsigned int calls, cancel_at;
    UmiStatus nested;
    int release_model, change_profile, edit, restore, remove;
} CallbackState;
static int Continue(void *data)
{
    CallbackState *state = data;
    ++state->calls;
    if (state->calls == 1U && state->release_model) {
        UmiBrowserReadingDocumentDestroy(state->document);
        state->document = NULL;
    }
    if (state->calls == 1U && state->change_profile)
        g_strlcpy(state->profile->interface_font, "Unrelated Font", sizeof(state->profile->interface_font));
    if (state->calls == 1U && state->nested == UMI_STATUS_INTERNAL_ERROR)
        state->nested = UmiReadingDocumentGtk4Style(state->document, state->buffer,
            state->profile, 100U, NULL, NULL);
    return state->cancel_at == 0U || state->calls < state->cancel_at;
}
static void Applied(GtkTextBuffer *buffer, GtkTextTag *tag,
    GtkTextIter *first, GtkTextIter *last, gpointer data)
{
    (void)tag; (void)first; (void)last;
    CallbackState *state = data;
    if (state->remove == 2) {
        state->remove = 0;
        gtk_text_tag_table_remove(gtk_text_buffer_get_tag_table(buffer), tag);
        return;
    }
    if (!state->edit) return;
    state->edit = 0;
    char *saved = NULL;
    if (state->restore) {
        GtkTextIter begin, end;
        gtk_text_buffer_get_bounds(buffer, &begin, &end);
        saved = gtk_text_buffer_get_text(buffer, &begin, &end, TRUE);
    }
    gtk_text_buffer_set_text(buffer, "changed during notification", -1);
    if (saved != NULL) { gtk_text_buffer_set_text(buffer, saved, -1); g_free(saved); }
}
static void Added(GtkTextTagTable *table, GtkTextTag *tag, gpointer data)
{
    CallbackState *state = data;
    if (state->remove == 1) { state->remove = 0; gtk_text_tag_table_remove(table, tag); }
}
static void VerifyText(GtkTextBuffer *buffer, const char *expected)
{
    GtkTextIter first, last;
    gtk_text_buffer_get_bounds(buffer, &first, &last);
    char *text = gtk_text_buffer_get_text(buffer, &first, &last, TRUE);
    assert(strcmp(text, expected) == 0); g_free(text);
}
static GtkTextTag *Tag(GtkTextBuffer *buffer, const char *name)
{
    GtkTextTag *tag = gtk_text_tag_table_lookup(gtk_text_buffer_get_tag_table(buffer), name);
    assert(tag != NULL); return tag;
}
static void VerifyRanges(GtkTextBuffer *buffer, const UmiBrowserReadingDocument *document)
{
    const char *text = UmiBrowserReadingDocumentText(document);
    UmiBrowserReadingSummary summary;
    assert(UmiBrowserReadingDocumentSummary(document, &summary) == UMI_STATUS_OK);
    for (size_t i = 0U; i < summary.spans + summary.links; ++i) {
        char name[64]; size_t begin, length;
        if (i < summary.spans) {
            UmiBrowserReadingSpan span;
            assert(UmiBrowserReadingDocumentSpanAt(document, i, &span) == UMI_STATUS_OK);
            begin = span.byte_start; length = span.byte_length;
            if (span.kind == UMI_BROWSER_READING_HEADING)
                g_snprintf(name, sizeof(name), "umicom-reading-heading-%u", span.heading_level);
            else g_strlcpy(name, span.kind == UMI_BROWSER_READING_CODE ? "umicom-reading-code" :
                span.kind == UMI_BROWSER_READING_STRONG ? "umicom-reading-strong" : "umicom-reading-emphasis", sizeof(name));
        } else {
            UmiBrowserReadingLink link;
            assert(UmiBrowserReadingDocumentLinkAt(document, i-summary.spans, &link) == UMI_STATUS_OK);
            begin = link.byte_start; length = link.byte_length;
            g_strlcpy(name, "umicom-reading-link", sizeof(name));
        }
        if (length != 0U) {
            GtkTextIter first, last;
            gtk_text_buffer_get_iter_at_offset(buffer, &first,
                (gint)g_utf8_pointer_to_offset(text, text+begin));
            gtk_text_buffer_get_iter_at_offset(buffer, &last,
                (gint)g_utf8_pointer_to_offset(text, text+begin+length)-1);
            assert(gtk_text_iter_has_tag(&first, Tag(buffer, name)));
            assert(gtk_text_iter_has_tag(&last, Tag(buffer, name)));
        }
    }
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    if (!gtk_init_check()) return 77;
    const char *name = argv[1];
    static const char *const known[] = {"structure", "fonts", "zoom-min", "zoom-max", "selection", "unrelated-tag", "restyle", "empty", "text-mismatch", "null-document", "null-profile", "null-buffer", "null-output", "zoom-low", "zoom-high", "invalid-profile", "invalid-utf8", "split-start", "split-end", "range-overflow", "span-limit", "link-limit", "invalid-kind", "heading-low", "heading-high", "nonheading-level", "cancel-before", "cancel-after", "cancel-retry", "reentrant", "model-release", "profile-capture", "text-edit", "text-edit-restore", "tag-remove", "private-owner", "create-invalid", "create-zoom", "create-profile", "text-capacity", "embedded-nul", "link-split", "tag-remove-late"};
    int recognized = 0;
    for (size_t i = 0U; i < sizeof(known)/sizeof(known[0]); ++i)
        if (strcmp(name, known[i]) == 0) recognized = 1;
    if (!recognized) return 2;
    const char *html = "<h1>\xc3\x85\xf0\x9f\x98\x80</h1><p><strong>Bold</strong> "
        "<em>soft</em> <code>x</code> <a href='local.html'>link</a></p>";
    if (strcmp(name, "empty") == 0) html = "";
    UmiBrowserReadingDocument *document = NULL;
    assert(UmiBrowserReadingDocumentCreate(html, strlen(html), &document) == UMI_STATUS_OK);
    UmiUiAppearanceProfile profile;
    assert(umi_ui_appearance_profile_init(&profile, "reader-test", "Reader test",
        UMI_UI_THEME_MODE_SYSTEM, UMI_UI_DENSITY_COMFORTABLE) == UMI_STATUS_OK);
    g_strlcpy(profile.interface_font, "Segoe UI, Sans", sizeof(profile.interface_font));
    g_strlcpy(profile.editor_font, "Cascadia Code, Monospace", sizeof(profile.editor_font));
    profile.interface_font_size = 12.0; profile.editor_font_size = 13.0; profile.font_scale = 1.0;
    GtkTextBuffer *buffer = gtk_text_buffer_new(NULL);
    gtk_text_buffer_set_text(buffer, UmiBrowserReadingDocumentText(document), -1);
    GtkTextBuffer *candidate = buffer;
    UmiStatus expected = UMI_STATUS_OK, status;
    unsigned int zoom = strcmp(name, "zoom-min") == 0 ? 50U : strcmp(name, "zoom-max") == 0 ? 200U : 100U;
    int create = strcmp(name, "private-owner") == 0 || strcmp(name, "null-output") == 0 || strncmp(name, "create-", 7U) == 0;
    int admission = 0;
    if (strcmp(name, "create-invalid") == 0) {
        document->text[0] = (char)0xff; expected = UMI_STATUS_INVALID_ARGUMENT; admission = 1;
    } else if (strcmp(name, "create-zoom") == 0) {
        zoom = 201U; expected = UMI_STATUS_INVALID_ARGUMENT; admission = 1;
    } else if (strcmp(name, "create-profile") == 0) {
        profile.editor_font_size = NAN; expected = UMI_STATUS_INVALID_STATE; admission = 1;
    } else if (strcmp(name, "text-capacity") == 0) {
        document->summary.text_bytes = UMI_BROWSER_READING_TEXT_MAX + 1U;
        expected = UMI_STATUS_CAPACITY_EXCEEDED; admission = 1;
    } else if (strcmp(name, "embedded-nul") == 0) {
        document->text[0] = '\0'; expected = UMI_STATUS_INVALID_ARGUMENT; admission = 1;
    } else if (strcmp(name, "link-split") == 0) {
        document->links[0].byte_start = 1U; document->links[0].byte_length = 1U;
        expected = UMI_STATUS_INVALID_ARGUMENT; admission = 1;
    } else if (strcmp(name, "text-mismatch") == 0) {
        gtk_text_buffer_set_text(buffer, "other text", -1); expected = UMI_STATUS_INVALID_STATE; admission = 1;
    } else if (strcmp(name, "invalid-profile") == 0) {
        profile.font_scale = NAN; expected = UMI_STATUS_INVALID_STATE; admission = 1;
    } else if (strcmp(name, "invalid-utf8") == 0) {
        document->text[0] = (char)0xff; expected = UMI_STATUS_INVALID_ARGUMENT; admission = 1;
    } else if (strcmp(name, "split-start") == 0) {
        document->spans[0].byte_start = 1U; expected = UMI_STATUS_INVALID_ARGUMENT; admission = 1;
    } else if (strcmp(name, "split-end") == 0) {
        document->spans[0].byte_length = 1U; expected = UMI_STATUS_INVALID_ARGUMENT; admission = 1;
    } else if (strcmp(name, "range-overflow") == 0) {
        document->spans[0].byte_length = (size_t)-1; expected = UMI_STATUS_INVALID_ARGUMENT; admission = 1;
    } else if (strcmp(name, "span-limit") == 0) {
        document->summary.spans = UMI_BROWSER_READING_SPANS_MAX+1U; expected = UMI_STATUS_CAPACITY_EXCEEDED; admission = 1;
    } else if (strcmp(name, "link-limit") == 0) {
        document->summary.links = UMI_BROWSER_READING_LINKS_MAX+1U; expected = UMI_STATUS_CAPACITY_EXCEEDED; admission = 1;
    } else if (strcmp(name, "invalid-kind") == 0) {
        document->spans[0].kind = (UmiBrowserReadingKind)99; expected = UMI_STATUS_INVALID_ARGUMENT; admission = 1;
    } else if (strcmp(name, "heading-low") == 0 || strcmp(name, "heading-high") == 0) {
        document->spans[0].heading_level = strcmp(name, "heading-low") == 0 ? 0U : 7U;
        expected = UMI_STATUS_INVALID_ARGUMENT; admission = 1;
    } else if (strcmp(name, "nonheading-level") == 0) {
        document->spans[1].heading_level = 2U; expected = UMI_STATUS_INVALID_ARGUMENT; admission = 1;
    } else if (strcmp(name, "zoom-low") == 0 || strcmp(name, "zoom-high") == 0) {
        zoom = strcmp(name, "zoom-low") == 0 ? 49U : 201U; expected = UMI_STATUS_INVALID_ARGUMENT; admission = 1;
    } else if (strncmp(name, "null-", 5U) == 0) {
        expected = UMI_STATUS_INVALID_ARGUMENT; admission = 1;
    }
    GtkTextIter first, last;
    gtk_text_buffer_get_iter_at_offset(buffer, &first, 1);
    gtk_text_buffer_get_iter_at_offset(buffer, &last, 2);
    if (strcmp(name, "selection") == 0) gtk_text_buffer_select_range(buffer, &first, &last);
    GtkTextTag *unrelated = NULL;
    if (strcmp(name, "unrelated-tag") == 0) {
        unrelated = gtk_text_buffer_create_tag(buffer, "host-annotation", "weight", PANGO_WEIGHT_BOLD, NULL);
        gtk_text_buffer_apply_tag(buffer, unrelated, &first, &last);
    }
    CallbackState callback = {document, &profile, buffer, 0U, 0U, UMI_STATUS_OK, 0, 0, 0, 0, 0};
    if (strncmp(name, "cancel-", 7U) == 0) {
        callback.cancel_at = strcmp(name, "cancel-before") == 0 ? 1U : 4U;
        expected = UMI_STATUS_CANCELLED;
    }
    if (strcmp(name, "reentrant") == 0) callback.nested = UMI_STATUS_INTERNAL_ERROR;
    callback.release_model = strcmp(name, "model-release") == 0;
    callback.change_profile = strcmp(name, "profile-capture") == 0;
    callback.edit = strcmp(name, "text-edit") == 0 || strcmp(name, "text-edit-restore") == 0;
    callback.restore = strcmp(name, "text-edit-restore") == 0;
    callback.remove = strcmp(name, "tag-remove") == 0 ? 1 : strcmp(name, "tag-remove-late") == 0 ? 2 : 0;
    if (callback.edit || callback.remove) expected = UMI_STATUS_INVALID_STATE;
    gulong applied = g_signal_connect_after(buffer, "apply-tag", G_CALLBACK(Applied), &callback);
    gulong added = g_signal_connect(gtk_text_buffer_get_tag_table(buffer), "tag-added", G_CALLBACK(Added), &callback);
    if (create) status = UmiReadingDocumentGtk4CreateBuffer(document, &profile, zoom,
        strcmp(name, "null-output") == 0 ? NULL : &candidate);
    else status = UmiReadingDocumentGtk4Style(
        strcmp(name, "null-document") == 0 ? NULL : document,
        strcmp(name, "null-buffer") == 0 ? NULL : buffer,
        strcmp(name, "null-profile") == 0 ? NULL : &profile, zoom, Continue, &callback);
    assert(status == expected);
    if (create && status != UMI_STATUS_OK) assert(candidate == buffer);
    if (callback.release_model) document = NULL;
    if (admission) assert(gtk_text_tag_table_get_size(gtk_text_buffer_get_tag_table(buffer)) == 0);
    if (strcmp(name, "cancel-before") == 0)
        assert(gtk_text_tag_table_get_size(gtk_text_buffer_get_tag_table(buffer)) == 0);
    if (strcmp(name, "reentrant") == 0) assert(callback.nested == UMI_STATUS_BUSY);
    if (strcmp(name, "cancel-retry") == 0 || strcmp(name, "restyle") == 0) {
        assert(UmiReadingDocumentGtk4Style(document, buffer, &profile, 100U, NULL, NULL) == UMI_STATUS_OK);
        VerifyRanges(buffer, document);
    }
    if (status == UMI_STATUS_OK) {
        GtkTextBuffer *styled = create ? candidate : buffer;
        if (document != NULL) { VerifyText(styled, UmiBrowserReadingDocumentText(document)); VerifyRanges(styled, document); }
        char *family = NULL; double points = 0.0;
        g_object_get(Tag(styled, "umicom-reading-body"), "family", &family, "size-points", &points, NULL);
        assert(strcmp(family, "Segoe UI, Sans") == 0); g_free(family);
        assert(fabs(points - 12.0*(double)zoom/100.0) < 0.01);
        if (strcmp(name, "empty") != 0) {
            g_object_get(Tag(styled, "umicom-reading-code"), "family", &family, "size-points", &points, NULL);
            assert(strcmp(family, "Cascadia Code, Monospace") == 0); g_free(family);
            assert(fabs(points - 13.0*(double)zoom/100.0) < 0.01);
        }
    }
    if (strcmp(name, "selection") == 0) {
        assert(gtk_text_buffer_get_selection_bounds(buffer, &first, &last));
        assert(gtk_text_iter_get_offset(&first) == 1 && gtk_text_iter_get_offset(&last) == 2);
    }
    if (unrelated != NULL) {
        gtk_text_buffer_get_iter_at_offset(buffer, &first, 1);
        assert(gtk_text_iter_has_tag(&first, unrelated));
    }
    if (create && status == UMI_STATUS_OK) {
        assert(candidate != buffer);
        UmiBrowserReadingDocumentDestroy(document); document = NULL;
        assert(gtk_text_buffer_get_char_count(candidate) > 0);
        g_object_unref(candidate);
    }
    g_signal_handler_disconnect(buffer, applied);
    g_signal_handler_disconnect(gtk_text_buffer_get_tag_table(buffer), added);
    UmiBrowserReadingDocumentDestroy(document);
    g_object_unref(buffer);
    return 0;
}
