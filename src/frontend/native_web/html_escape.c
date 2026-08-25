/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/native_web/html_escape.c
 *
 * PURPOSE:
 *   Escape HTML text and attribute values without third-party templating dependencies.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/frontend/native_web/html_escape.h"

static UmiStatus append_entity(UmiNativeWebMarkupBuffer *buffer, char ch, bool attribute)
{
    switch (ch) {
        case '&': return umi_native_web_markup_buffer_append(buffer, "&amp;");
        case '<': return umi_native_web_markup_buffer_append(buffer, "&lt;");
        case '>': return umi_native_web_markup_buffer_append(buffer, "&gt;");
        case '"': return attribute ? umi_native_web_markup_buffer_append(buffer, "&quot;") : umi_native_web_markup_buffer_append_n(buffer, &ch, 1U);
        case '\'': return attribute ? umi_native_web_markup_buffer_append(buffer, "&#39;") : umi_native_web_markup_buffer_append_n(buffer, &ch, 1U);
        default: return umi_native_web_markup_buffer_append_n(buffer, &ch, 1U);
    }
}

static UmiStatus escape(UmiNativeWebMarkupBuffer *buffer, const char *text, bool attribute)
{
    const char *p;
    UmiStatus status;
    if (buffer == NULL || text == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (p = text; *p != '\0'; ++p) {
        status = append_entity(buffer, *p, attribute);
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_native_web_html_escape_text(UmiNativeWebMarkupBuffer *buffer, const char *text)
{
    return escape(buffer, text, false);
}

UmiStatus umi_native_web_html_escape_attribute(UmiNativeWebMarkupBuffer *buffer, const char *text)
{
    return escape(buffer, text, true);
}

