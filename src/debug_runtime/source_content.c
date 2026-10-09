/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/source_content.c
 * PURPOSE: Read debugger source responses as bounded UTF-8 text rather than executable content.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "response_fields_internal.h"
#include "umicom/debug_runtime/source_catalog.h"
#include <stdlib.h>
#include <string.h>
UmiStatus UmiDebugSourceContentDecode(const char *json, UmiDebugSourceContent *out)
{
    if (json == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiLanguageRuntimeJsonDocument *document = malloc(sizeof *document);
    UmiDebugSourceContent *content = calloc(1U, sizeof *content);
    if (document == NULL || content == NULL)
    {
        free(document);
        free(content);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    UmiStatus status = umi_language_runtime_json_parse(json, document);
    int body = -1;
    if (status == UMI_STATUS_OK)
        status = DebugResponseJsonResponse(document, "source", 0, &body);
    if (status == UMI_STATUS_OK)
        status = DebugResponseJsonString(document, body, "content", 1, content->text,
                                         sizeof content->text);
    if (status == UMI_STATUS_OK)
        status = DebugResponseJsonString(document, body, "mimeType", 0, content->mime_type,
                                         sizeof content->mime_type);
    /* MIME is metadata only. A caller must never turn a debugger response into
     * HTML execution or overwrite a local document just because its path matches. */
    if (status == UMI_STATUS_OK)
    {
        content->bytes = strlen(content->text);
        *out = *content;
    }
    free(content);
    free(document);
    return status;
}
