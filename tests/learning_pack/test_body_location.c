/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/learning_pack/test_body_location.c
 *
 * PURPOSE:
 *   Verify search-location identity, UTF-8 boundaries and transactional character offsets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../../adapters/gtk4/learning_catalogue_private.h"
#include <stdio.h>
#include <string.h>

#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); result = 1; goto cleanup; } } while (0)
static int Check(const char *name)
{
    int result = 0, known = 1;
    LearningCatalogueJob *job = g_try_new0(LearningCatalogueJob, 1U);
    CHECK(job != NULL);
    job->selects_text_hit = 1;
    job->snapshot.has_document = 1;
    job->snapshot.pack.revision = 7U;
    g_strlcpy(job->snapshot.pack.id, "pack", sizeof(job->snapshot.pack.id));
    g_strlcpy(job->snapshot.selection.resource.id, "guide", sizeof(job->snapshot.selection.resource.id));
    memset(job->snapshot.selection.resource.sha256, 'a', 64U);
    job->snapshot.selection.resource.sha256[64] = '\0';
    job->text_hit.pack = job->snapshot.pack;
    job->text_hit.resource = job->snapshot.selection.resource;
    job->text_hit.byte_start = 7U; job->text_hit.byte_end = 16U;
    job->display = g_strdup("prefix ownership tail");
    job->display_is_source = 1;
    job->text_first_character = 83; job->text_last_character = 97;
    UmiStatus expected = UMI_STATUS_OK;
    gint first = 7, last = 16;
    if (strcmp(name, "null") == 0) {
        CHECK(LearningCataloguePrepareBodySelection(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        goto cleanup;
    } else if (strcmp(name, "no-hit") == 0) {
        job->selects_text_hit = 0; job->snapshot.has_document = 0; first = 83; last = 97;
    } else if (strcmp(name, "projection") == 0 || strcmp(name, "source-projection") == 0) {
        const char html[] = "<p>prefix ownership tail</p>";
        CHECK(UmiBrowserReadingDocumentCreate(html, sizeof(html)-1U, &job->reading_document) == UMI_STATUS_OK);
        g_free(job->display); job->display = g_strdup(UmiBrowserReadingDocumentText(job->reading_document));
        job->display_is_source = 0;
        job->text_hit.reading_projection = strcmp(name, "projection") == 0;
        if (!job->text_hit.reading_projection) expected = UMI_STATUS_INVALID_STATE;
    } else if (strcmp(name, "unicode-prefix") == 0 || strcmp(name, "unicode-match") == 0 ||
        strcmp(name, "start-split") == 0 || strcmp(name, "end-split") == 0) {
        g_free(job->display); job->display = g_strdup("\xc3\xa9 alpha \xce\xb2" "eta");
        job->text_hit.byte_start = 3U; job->text_hit.byte_end = 8U; first = 2; last = 7;
        if (strcmp(name, "unicode-match") == 0) {
            job->text_hit.byte_start = 9U; job->text_hit.byte_end = 14U; first = 8; last = 12;
        } else if (strcmp(name, "start-split") == 0) {
            job->text_hit.byte_start = 1U; expected = UMI_STATUS_INVALID_ARGUMENT;
        } else if (strcmp(name, "end-split") == 0) {
            job->text_hit.byte_end = 10U; expected = UMI_STATUS_INVALID_ARGUMENT;
        }
    } else if (strcmp(name, "end") == 0) {
        job->text_hit.byte_start = 17U; job->text_hit.byte_end = 21U; first = 17; last = 21;
    } else if (strcmp(name, "empty-range") == 0) {
        job->text_hit.byte_end = job->text_hit.byte_start; expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(name, "reversed") == 0) {
        job->text_hit.byte_start = 18U; expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(name, "end-past") == 0) {
        job->text_hit.byte_end = 22U; expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(name, "start-past") == 0) {
        job->text_hit.byte_start = SIZE_MAX-1U; job->text_hit.byte_end = SIZE_MAX;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(name, "oversize") == 0) {
        g_free(job->display); job->display = g_malloc(UMI_LEARNING_TEXT_DOCUMENT_MAX+2U);
        memset(job->display, 'a', UMI_LEARNING_TEXT_DOCUMENT_MAX+1U);
        job->display[UMI_LEARNING_TEXT_DOCUMENT_MAX+1U] = '\0';
        expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(name, "invalid-utf8") == 0) {
        job->display[0] = (char)0xff; expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(name, "missing-document") == 0) {
        job->snapshot.has_document = 0; expected = UMI_STATUS_INVALID_STATE;
    } else if (strcmp(name, "revision") == 0) {
        ++job->snapshot.pack.revision; expected = UMI_STATUS_INVALID_STATE;
    } else if (strcmp(name, "pack") == 0) {
        job->snapshot.pack.id[0] = 'x'; expected = UMI_STATUS_INVALID_STATE;
    } else if (strcmp(name, "resource") == 0) {
        job->snapshot.selection.resource.id[0] = 'x'; expected = UMI_STATUS_INVALID_STATE;
    } else if (strcmp(name, "digest") == 0) {
        job->snapshot.selection.resource.sha256[0] = 'b'; expected = UMI_STATUS_INVALID_STATE;
    } else if (strcmp(name, "projection-flag") == 0) {
        job->text_hit.reading_projection = 2; expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(name, "projection-missing") == 0) {
        job->text_hit.reading_projection = 1; expected = UMI_STATUS_INVALID_STATE;
    } else if (strcmp(name, "source-flag") == 0) {
        job->display_is_source = 0; expected = UMI_STATUS_INVALID_STATE;
    } else if (strcmp(name, "missing-display") == 0) {
        g_clear_pointer(&job->display, g_free); expected = UMI_STATUS_INVALID_STATE;
    } else if (strcmp(name, "source") != 0) known = 0;
    CHECK(known);
    CHECK(LearningCataloguePrepareBodySelection(job) == expected);
    CHECK(job->text_first_character == (expected == UMI_STATUS_OK ? first : 83));
    CHECK(job->text_last_character == (expected == UMI_STATUS_OK ? last : 97));
cleanup:
    if (job != NULL) {
        UmiBrowserReadingDocumentDestroy(job->reading_document);
        g_free(job->display);
        g_free(job);
    }
    return result;
}
int main(int argc, char **argv)
{
    return argc == 2 ? Check(argv[1]) : 2;
}
