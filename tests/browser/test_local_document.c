/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/browser/test_local_document.c
 *
 * PURPOSE:
 *   Verify bounded local previews retain prior output when admission or file reads fail.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/browser/document_preview.h"
#include "../build_log/fixture.h"

static void Store(const char *path, const void *bytes, size_t size)
{
    UmiOutputFile *file = NULL;
    CHECK(UmiOutputFileCreate(path, &file) == UMI_STATUS_OK);
    CHECK(UmiOutputFileWrite(file, bytes, size) == UMI_STATUS_OK);
    CHECK(UmiOutputFileClose(file) == UMI_STATUS_OK);
    UmiOutputFileDestroy(file);
}

int main(void)
{
    static const char html[] = "<title>Learning</title><p>Read <b>Umicom</b>.</p><script>hidden()</script>";
    static const unsigned char binary[] = {'<', 'p', '>', 'A', 0U, 'B'};
    char root[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY];
    UmiBrowserDocumentPreview *preview = malloc(sizeof(*preview));
    UmiBrowserDocumentPreview *before = malloc(sizeof(*before));
    CHECK(preview != NULL && before != NULL);
    FixtureDirectory(root);
    FixturePath(path, root, "lesson.html");
    Store(path, html, sizeof(html) - 1U);
    CHECK(umi_browser_document_preview_read_file(path, sizeof(html) - 1U, preview) == UMI_STATUS_OK);
    CHECK(strcmp(preview->title, "Learning") == 0);
    CHECK(strstr(preview->text, "Read Umicom.") != NULL);
    CHECK(strstr(preview->text, "hidden()") == NULL);
    CHECK(preview->source_bytes == sizeof(html) - 1U);
    memcpy(before, preview, sizeof(*before));

    /* The exact input budget succeeds; one byte less rejects before publishing. */
    CHECK(umi_browser_document_preview_read_file(path, sizeof(html) - 2U, preview) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(before, preview, sizeof(*before)) == 0);
    CHECK(umi_browser_document_preview_read_file(path, 0U, preview) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_browser_document_preview_read_file(NULL, 32U, preview) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_browser_document_preview_read_file(path, 32U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_browser_document_preview_read_file("lesson.html", 256U, preview) != UMI_STATUS_OK);
    CHECK(memcmp(before, preview, sizeof(*before)) == 0);

    FixturePath(path, root, "missing.html");
    CHECK(umi_browser_document_preview_read_file(path, 256U, preview) != UMI_STATUS_OK);
    CHECK(memcmp(before, preview, sizeof(*before)) == 0);
    FixturePath(path, root, "binary.html");
    Store(path, binary, sizeof(binary));
    CHECK(umi_browser_document_preview_read_file(path, sizeof(binary), preview) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(before, preview, sizeof(*before)) == 0);
    FixturePath(path, root, "empty.html");
    Store(path, "", 0U);
    CHECK(umi_browser_document_preview_read_file(path, 1U, preview) == UMI_STATUS_OK);
    CHECK(preview->source_bytes == 0U && preview->text_bytes == 0U);
    CHECK(preview->title[0] == '\0' && preview->text[0] == '\0');
    free(before);
    free(preview);
    return 0;
}
