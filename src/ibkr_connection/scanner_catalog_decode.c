/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/scanner_catalog_decode.c
 * PURPOSE: Accept catalogue text atomically while leaving XML inert.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include <stdlib.h>
#include <string.h>
/* Reuse the existing UTF-8 validator for each line segment. XML whitespace is
 * allowed here, while other control characters remain rejected. Temporary NUL
 * separators are restored before the owned document becomes visible. */
static bool CatalogText(char *text, size_t length)
{
    if (!length)
        return false;
    size_t start = 0;
    for (size_t i = 0; i <= length; ++i)
    {
        if (i < length && text[i] != '\n' && text[i] != '\r' && text[i] != '\t')
            continue;
        char saved = text[i];
        text[i] = 0;
        bool valid = UmiIbkrText(text + start, i - start + 1U, true);
        text[i] = saved;
        if (!valid)
            return false;
        start = i + 1U;
    }
    return true;
}
UmiStatus UmiIbkrScannerCatalogFrame(UmiIbkrConnection *c, const unsigned char *body, size_t length,
                                     uint64_t now)
{
    /* The legacy callback contains message 19, version 1 and one XML field.
     * Do not split XML into the normal small control-field array. */
    static const unsigned char prefix[] = {'1', '9', 0, '1', 0};
    if (!UmiIbkrScannerCatalogPending(c, now))
        return UMI_STATUS_OK;
    if (length <= sizeof prefix + 1U || length > UMI_IBKR_SCANNER_CATALOG_BYTES + 6U ||
        memcmp(body, prefix, sizeof prefix) || body[length - 1U] != 0 ||
        memchr(body + sizeof prefix, 0, length - sizeof prefix - 1U))
        return UMI_STATUS_PARSE_ERROR;
    size_t bytes = length - sizeof prefix - 1U;
    char *text = malloc(bytes + 1U);
    if (!text)
        return UMI_STATUS_OUT_OF_MEMORY;
    memcpy(text, body + sizeof prefix, bytes + 1U);
    if (!CatalogText(text, bytes))
    {
        free(text);
        return UMI_STATUS_PARSE_ERROR;
    }
    c->scannerCatalog.xml = text;
    UmiIbkrScannerCatalogSnapshot *s = &c->scannerCatalog.snapshot;
    s->byteCount = bytes;
    s->receivedAtMilliseconds = now;
    s->complete = true;
    s->stale = false;
    strcpy(s->message, "Scanner catalogue received as plain text. Broker availability still applies.");
    return UMI_STATUS_OK;
}
