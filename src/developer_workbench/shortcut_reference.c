/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_workbench/shortcut_reference.c
 *
 * PURPOSE:
 *   Validate and format supplied keyboard bindings without invoking commands or depending on a GUI toolkit.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/developer_workbench/shortcut_reference.h"
#include "umicom/document/text_encoding.h"
#include <string.h>

typedef struct ReferenceWriter { char *out; size_t used; } ReferenceWriter;
static void ReferenceBytes(ReferenceWriter *writer, const char *text, size_t bytes)
{
    if (writer->out != NULL) memcpy(writer->out + writer->used, text, bytes);
    writer->used += bytes;
}
static void ReferenceLiteral(ReferenceWriter *writer, const char *text)
{
    ReferenceBytes(writer, text, strlen(text));
}
static int ReferenceValid(const char *text, size_t capacity)
{
    const char *end = memchr(text, '\0', capacity);
    return end != NULL && end != text &&
        umi_document_utf8_validate((const unsigned char *)text, (size_t)(end - text), NULL);
}
static void ReferenceField(ReferenceWriter *writer, const char *text)
{
    const char hex[] = "0123456789ABCDEF";
    for (const unsigned char *p = (const unsigned char *)text; *p != 0U; ++p) {
        if (*p < 0x20U || *p == 0x7fU || *p == '\\') {
            const char escaped[] = {'\\', 'x', hex[*p >> 4U], hex[*p & 15U]};
            ReferenceBytes(writer, escaped, sizeof escaped);
        } else ReferenceBytes(writer, (const char *)p, 1U);
    }
}
static void ReferenceWrite(ReferenceWriter *writer,
    const UmiShortcutReferenceEntry *entries, size_t count)
{
    ReferenceLiteral(writer, "Keyboard shortcuts\n\n"
        "Bindings installed by this application. Availability is captured when this reference opens.\n"
        "Unavailable commands may need a document, project, selection or idle task.\n\n");
    if (count == 0U) ReferenceLiteral(writer, "No keyboard bindings were supplied.\n");
    for (size_t i = 0U; i < count; ++i) {
        ReferenceField(writer, entries[i].shortcut);
        ReferenceLiteral(writer, "  ");
        ReferenceField(writer, entries[i].caption);
        ReferenceLiteral(writer, entries[i].available ? " [available]\n  " : " [unavailable]\n  ");
        ReferenceField(writer, entries[i].command);
        ReferenceLiteral(writer, "\n\n");
    }
}
UmiStatus UmiShortcutReferenceFormat(const UmiShortcutReferenceEntry *entries,
    size_t count, char *out, size_t capacity, size_t *required)
{
    if (required == NULL || (entries == NULL && count != 0U) ||
        (out == NULL && capacity != 0U)) return UMI_STATUS_INVALID_ARGUMENT;
    if (count > UMI_SHORTCUT_REFERENCE_LIMIT) return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t i = 0U; i < count; ++i) {
        if (!ReferenceValid(entries[i].command, sizeof entries[i].command) ||
            !ReferenceValid(entries[i].caption, sizeof entries[i].caption) ||
            !ReferenceValid(entries[i].shortcut, sizeof entries[i].shortcut) ||
            (entries[i].available != 0 && entries[i].available != 1))
            return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Row and field bounds keep even a fully escaped report below one MiB.
     * Count before writing: refusal cannot leave a partially published page. */
    ReferenceWriter writer = {NULL, 0U};
    ReferenceWrite(&writer, entries, count);
    if (out == NULL) { *required = writer.used; return UMI_STATUS_OK; }
    if (capacity <= writer.used) { *required = writer.used; return UMI_STATUS_CAPACITY_EXCEEDED; }
    writer.out = out; writer.used = 0U;
    ReferenceWrite(&writer, entries, count);
    out[writer.used] = '\0';
    *required = writer.used;
    return UMI_STATUS_OK;
}
