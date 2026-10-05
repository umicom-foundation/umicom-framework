/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/navigation_text_internal.h
 * PURPOSE: Keep navigation history, bookmarks and line jumps consistent across LF, CRLF and CR drafts.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DOCUMENT_NAVIGATION_TEXT_INTERNAL_H
#define UMICOM_DOCUMENT_NAVIGATION_TEXT_INTERNAL_H
/* These helpers keep the public one-based byte-column convention. Language
 * protocol ranges use a separate exact UTF-16 conversion before entering this
 * navigation history. Columns past a shorter edited line clamp to its end. */
static UmiStatus NavigationTextOffset(const char *text, size_t length, uint64_t line, uint64_t column,
                                      size_t *out_offset)
{
    if (line == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    uint64_t current = 1U;
    size_t offset = 0U;
    while (offset < length && current < line)
    {
        if (text[offset] == '\r')
        {
            ++offset;
            if (offset < length && text[offset] == '\n')
                ++offset;
            ++current;
        }
        else if (text[offset++] == '\n')
            ++current;
    }
    if (current != line)
        return UMI_STATUS_NOT_FOUND;
    size_t start = offset, end = start;
    while (end < length && text[end] != '\r' && text[end] != '\n')
        ++end;
    uint64_t delta = column == 0U ? 0U : column - 1U;
    offset += delta < (uint64_t)(end - start) ? (size_t)delta : end - start;
    /* A byte column may land within UTF-8. Restore the start of that scalar
     * so native editors never receive a caret in a continuation byte. */
    while (offset > start && offset < length && ((unsigned char)text[offset] & 0xc0U) == 0x80U)
        --offset;
    *out_offset = offset;
    return UMI_STATUS_OK;
}
static void NavigationTextLocation(const char *text, size_t length, size_t offset, uint64_t *line,
                                   uint64_t *column)
{
    if (offset > length)
        offset = length;
    while (offset > 0U && offset < length && ((unsigned char)text[offset] & 0xc0U) == 0x80U)
        --offset;
    /* The middle of CRLF is not an editor position. Match the preceding line's
     * end, just as an interior UTF-8 byte belongs to its preceding scalar. */
    if (offset > 0U && offset < length && text[offset] == '\n' && text[offset - 1U] == '\r')
        --offset;
    uint64_t current = 1U;
    size_t start = 0U;
    for (size_t byte = 0U; byte < offset; ++byte)
    {
        if (text[byte] == '\r')
        {
            if (byte + 1U < offset && text[byte + 1U] == '\n')
                ++byte;
            ++current;
            start = byte + 1U;
        }
        else if (text[byte] == '\n')
        {
            ++current;
            start = byte + 1U;
        }
    }
    *line = current;
    *column = (uint64_t)(offset - start) + 1U;
}
#endif
