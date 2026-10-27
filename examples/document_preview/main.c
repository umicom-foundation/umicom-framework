/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/document_preview/main.c
 *
 * PURPOSE:
 *   Demonstrate bounded local HTML reading through the installed Framework SDK.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/browser/document_preview.h"
#include <stdio.h>
#include <stdlib.h>

/* A command-line example owns its output object and lends it to Framework.
 * Keeping this on the heap avoids a large automatic variable on small stacks. */
int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "Usage: umicom-document-preview <absolute-html-path>\n");
        return 2;
    }
    UmiBrowserDocumentPreview *preview = malloc(sizeof(*preview));
    if (preview == NULL) {
        fprintf(stderr, "The preview could not be allocated.\n");
        return 1;
    }
    /* The limit applies to source bytes, including tags. The readable result
     * has its own fixed capacity and explicitly reports when it is shortened. */
    UmiStatus status = umi_browser_document_preview_read_file(
        argv[1], 1024U * 1024U, preview);
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "Document could not be previewed: %s\n", umi_status_text(status));
        free(preview);
        return 1;
    }
    int result = printf("Title: %s\nSource bytes: %zu\nShortened: %s\n\n%s\n",
        preview->title[0] != '\0' ? preview->title : "(untitled)",
        preview->source_bytes, preview->truncated ? "yes" : "no", preview->text);
    free(preview);
    if (result < 0 || fflush(stdout) != 0) {
        fprintf(stderr, "The preview output could not be written.\n");
        return 1;
    }
    return 0;
}
