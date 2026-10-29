/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/learning_catalogue_body_navigation.c
 *
 * PURPOSE:
 *   Validate copied learning-match positions and select them in the verified native reader.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "learning_catalogue_private.h"
#include <string.h>

UmiStatus LearningCataloguePrepareBodySelection(LearningCatalogueJob *job)
{
    if (job == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!job->selects_text_hit) return UMI_STATUS_OK;
    const UmiLearningTextHit *hit = &job->text_hit;
    const UmiLearningReaderSnapshot *reader = &job->snapshot;
    if (!reader->has_document || reader->pack.revision != hit->pack.revision ||
        strcmp(reader->pack.id, hit->pack.id) != 0 ||
        strcmp(reader->selection.resource.id, hit->resource.id) != 0 ||
        strcmp(reader->selection.resource.sha256, hit->resource.sha256) != 0)
        return UMI_STATUS_INVALID_STATE;
    if (hit->reading_projection != 0 && hit->reading_projection != 1)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* HTML offsets describe the inert reading projection, never markup bytes.
     * Refuse a fallback presentation: it may have different text or offsets.
     * Reader opening already checked the resource digest against the manifest. */
    if (job->display == NULL ||
        (hit->reading_projection ? job->reading_document == NULL :
            job->reading_document != NULL || !job->display_is_source))
        return UMI_STATUS_INVALID_STATE;
    size_t size = strlen(job->display);
    if (size > UMI_LEARNING_TEXT_DOCUMENT_MAX || hit->byte_start >= hit->byte_end ||
        hit->byte_end > size) return UMI_STATUS_INVALID_ARGUMENT;
    if (!g_utf8_validate(job->display, (gssize)size, NULL) ||
        !g_utf8_validate(job->display, (gssize)hit->byte_start, NULL) ||
        !g_utf8_validate(job->display, (gssize)hit->byte_end, NULL))
        return UMI_STATUS_INVALID_ARGUMENT;
    /* GTK positions count characters; the search service counts UTF-8 bytes.
     * Validate the complete prefixes before conversion, including multibyte
     * characters before the match. Publish neither output on a refusal. */
    glong first = g_utf8_strlen(job->display, (gssize)hit->byte_start);
    glong last = g_utf8_strlen(job->display, (gssize)hit->byte_end);
    if (first < 0 || last < first || last > G_MAXINT)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    job->text_first_character = (gint)first;
    job->text_last_character = (gint)last;
    return UMI_STATUS_OK;
}

UmiStatus LearningCataloguePublishBodySelection(LearningCatalogue *owner,
    const LearningCatalogueJob *job)
{
    if (owner == NULL || job == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (owner->state.retired || !owner->state.busy || !job->selects_text_hit)
        return UMI_STATUS_INVALID_STATE;
    const char *expected = owner->reading_document != NULL ?
        UmiBrowserReadingDocumentText(owner->reading_document) : owner->reading_source;
    if (expected == NULL) return UMI_STATUS_INVALID_STATE;
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(owner->content));
    GtkTextIter first, last;
    gtk_text_buffer_get_bounds(buffer, &first, &last);
    char *visible = gtk_text_buffer_get_text(buffer, &first, &last, TRUE);
    int unchanged = visible != NULL && strcmp(visible, expected) == 0;
    g_free(visible);
    if (!unchanged) return UMI_STATUS_INVALID_STATE;
    /* The dispatcher retains the root and keeps busy set throughout publication.
     * Selection notifications may retire the panel, so do not scroll afterwards
     * unless the owner still accepts presentation work. */
    gtk_text_buffer_get_iter_at_offset(buffer, &first, job->text_first_character);
    gtk_text_buffer_get_iter_at_offset(buffer, &last, job->text_last_character);
    gtk_text_buffer_select_range(buffer, &last, &first);
    if (!owner->state.retired)
        gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(owner->content), &first, 0.1, FALSE, 0.0, 0.0);
    return UMI_STATUS_OK;
}
