/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/native_web/html_escape.c
 *
 * PURPOSE:
 *   Escape HTML text and attribute values without third-party templating dependencies.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/frontend/native_web/html_escape.h"

/* Provide the append entity operation used by this module and its client applications. */
static UmiStatus append_entity(UmiNativeWebMarkupBuffer *buffer, char ch, bool attribute)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (ch) {
        case '&': return umi_native_web_markup_buffer_append(buffer, "&amp;");
        case '<': return umi_native_web_markup_buffer_append(buffer, "&lt;");
        case '>': return umi_native_web_markup_buffer_append(buffer, "&gt;");
        case '"': return attribute ? umi_native_web_markup_buffer_append(buffer, "&quot;") : umi_native_web_markup_buffer_append_n(buffer, &ch, 1U);
        case '\'': return attribute ? umi_native_web_markup_buffer_append(buffer, "&#39;") : umi_native_web_markup_buffer_append_n(buffer, &ch, 1U);
        default: return umi_native_web_markup_buffer_append_n(buffer, &ch, 1U);
    }
}

/* Provide the escape operation used by this module and its client applications. */
static UmiStatus escape(UmiNativeWebMarkupBuffer *buffer, const char *text, bool attribute)
{
    const char *p;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (buffer == NULL || text == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (p = text; *p != '\0'; ++p) {
        status = append_entity(buffer, *p, attribute);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the native web html escape text operation used by this module and its client
 * applications.
 */
UmiStatus umi_native_web_html_escape_text(UmiNativeWebMarkupBuffer *buffer, const char *text)
{
    return escape(buffer, text, false);
}

/*
 * Provide the native web html escape attribute operation used by this module and its
 * client applications.
 */
UmiStatus umi_native_web_html_escape_attribute(UmiNativeWebMarkupBuffer *buffer, const char *text)
{
    return escape(buffer, text, true);
}

