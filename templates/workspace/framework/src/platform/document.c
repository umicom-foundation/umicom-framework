/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: templates/workspace/framework/src/platform/document.c
 *
 * PURPOSE:
 *   Implement the document behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/platform/document.h"
#include "umicom/platform/filesystem.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Initialise document from caller-provided values so later operations receive a known
 * state.
 */
void umi_document_init(UmiDocument *document)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (document != 0) memset(document, 0, sizeof(*document));
}
/* Release or reset state held by document so the same storage can be reused safely. */
void umi_document_dispose(UmiDocument *document)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (document == 0) return;
    free(document->text);
    memset(document, 0, sizeof(*document));
}
/* Provide the document open operation used by this module and its client applications. */
UmiStatus umi_document_open(UmiDocument *document, const char *path)
{
    char *text = 0;
    size_t length = 0U;
    UmiStatus status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (document == 0 || path == 0) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_fs_read_text(path, &text, &length);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    umi_document_dispose(document);
    (void)snprintf(document->path, sizeof(document->path), "%s", path);
    document->text = text;
    document->length = length;
    document->revision = 1U;
    document->dirty = 0;
    return UMI_STATUS_OK;
}
/* Provide the document set text operation used by this module and its client applications. */
UmiStatus umi_document_set_text(UmiDocument *document, const char *text)
{
    size_t length;
    char *copy;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (document == 0 || text == 0) return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(text);
    copy = malloc(length + 1U);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (copy == 0) return UMI_STATUS_OUT_OF_MEMORY;
    memcpy(copy, text, length + 1U);
    free(document->text);
    document->text = copy;
    document->length = length;
    document->revision++;
    document->dirty = 1;
    return UMI_STATUS_OK;
}
/* Add document only after its inputs and available capacity have been checked. */
UmiStatus umi_document_append(UmiDocument *document, const char *text)
{
    size_t extra;
    char *resized;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (document == 0 || text == 0) return UMI_STATUS_INVALID_ARGUMENT;
    extra = strlen(text);
    resized = realloc(document->text, document->length + extra + 1U);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (resized == 0) return UMI_STATUS_OUT_OF_MEMORY;
    document->text = resized;
    memcpy(document->text + document->length, text, extra + 1U);
    document->length += extra;
    document->revision++;
    document->dirty = 1;
    return UMI_STATUS_OK;
}
/*
 * Write document in its stable representation and report capacity or input failures to the
 * caller.
 */
UmiStatus umi_document_save(UmiDocument *document)
{
    UmiStatus status;
    /* Apply this branch only when its contract condition is satisfied. */
    if (document == 0 || document->path[0] == '\0' || document->text == 0)
        return UMI_STATUS_INVALID_STATE;
    status = umi_fs_write_text(document->path, document->text);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) document->dirty = 0;
    return status;
}
