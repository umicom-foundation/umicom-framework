/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_browser_document_preview.c
 *
 * PURPOSE:
 *   Verify that HTML documentation can be projected into a safe readable
 *   preview without exposing script or style content.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/browser/document_preview.h"

/* Exercise title, visible text, entity and hidden-content behaviour together. */
int main(void)
{
    static const char HTML[] =
        "<!doctype html><html><head><title>Getting Started</title>"
        "<style>hidden style</style></head><body><h1>First lesson</h1>"
        "<p>Write &amp; build C safely.</p><script>hidden script</script>"
        "</body></html>";
    UmiBrowserDocumentPreview preview;

    assert(umi_browser_document_preview_from_html(
        HTML, sizeof(HTML) - 1U, &preview) == UMI_STATUS_OK);
    assert(strcmp(preview.title, "Getting Started") == 0);
    assert(strstr(preview.text, "First lesson") != NULL);
    assert(strstr(preview.text, "Write & build C safely.") != NULL);
    assert(strstr(preview.text, "hidden style") == NULL);
    assert(strstr(preview.text, "hidden script") == NULL);
    assert(preview.truncated == 0);
    return 0;
}
