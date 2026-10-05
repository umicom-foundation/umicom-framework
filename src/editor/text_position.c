/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/text_position.c
 * PURPOSE: Use one Unicode and line-ending policy for editor and protocol coordinates.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/editor/text_position.h"

/* Decode a complete Unicode scalar, refusing overlong encodings, surrogate
 * values and code points outside Unicode. No locale or platform code page
 * participates, so the same source has the same coordinates on each host. */
static UmiStatus PositionScalar(const unsigned char *bytes, size_t remaining, size_t *width, uint64_t *units)
{
    unsigned char first = bytes[0];
    uint32_t scalar, minimum;
    size_t count;
    if (first < 0x80U)
    {
        *width = 1U;
        *units = 1U;
        return UMI_STATUS_OK;
    }
    if (first >= 0xc2U && first <= 0xdfU)
    {
        count = 2U;
        scalar = first & 0x1fU;
        minimum = 0x80U;
    }
    else if (first >= 0xe0U && first <= 0xefU)
    {
        count = 3U;
        scalar = first & 0x0fU;
        minimum = 0x800U;
    }
    else if (first >= 0xf0U && first <= 0xf4U)
    {
        count = 4U;
        scalar = first & 0x07U;
        minimum = 0x10000U;
    }
    else
        return UMI_STATUS_PARSE_ERROR;
    if (remaining < count)
        return UMI_STATUS_PARSE_ERROR;
    for (size_t i = 1U; i < count; ++i)
    {
        if ((bytes[i] & 0xc0U) != 0x80U)
            return UMI_STATUS_PARSE_ERROR;
        scalar = (scalar << 6U) | (uint32_t)(bytes[i] & 0x3fU);
    }
    if (scalar < minimum || scalar > 0x10ffffU || (scalar >= 0xd800U && scalar <= 0xdfffU))
        return UMI_STATUS_PARSE_ERROR;
    *width = count;
    *units = scalar > 0xffffU ? 2U : 1U;
    return UMI_STATUS_OK;
}
static int PositionCompare(UmiEditorTextPosition left, UmiEditorTextPosition right)
{
    if (left.line != right.line)
        return left.line < right.line ? -1 : 1;
    if (left.utf16_column != right.utf16_column)
        return left.utf16_column < right.utf16_column ? -1 : 1;
    return 0;
}
/* One scanner serves both directions. Testing the requested boundary before
 * advancing also makes an empty buffer and an end-of-line position explicit. */
static UmiStatus PositionScan(const UmiEditorTextBufferView *view, int by_offset, size_t target_offset,
                              UmiEditorTextPosition target_position, size_t *resolved_offset,
                              UmiEditorTextPosition *resolved_position)
{
    if (view == NULL || view->struct_size != sizeof(*view) ||
        view->api_version != UMI_EDITOR_TEXT_BUFFER_API_VERSION ||
        (view->bytes == NULL && view->byte_count != 0U) || view->byte_count > view->capacity ||
        (by_offset && target_offset > view->byte_count))
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t offset = 0U;
    UmiEditorTextPosition position = {0U, 0U};
    for (;;)
    {
        int order = by_offset ? (offset < target_offset   ? -1
                                 : offset > target_offset ? 1
                                                          : 0)
                              : PositionCompare(position, target_position);
        if (order == 0)
        {
            *resolved_offset = offset;
            *resolved_position = position;
            return UMI_STATUS_OK;
        }
        if (order > 0 || offset == view->byte_count)
            return UMI_STATUS_INVALID_ARGUMENT;
        unsigned char first = (unsigned char)view->bytes[offset];
        if (first == (unsigned char)'\r' || first == (unsigned char)'\n')
        {
            size_t width = first == (unsigned char)'\r' && offset + 1U < view->byte_count &&
                                   view->bytes[offset + 1U] == '\n'
                               ? 2U
                               : 1U;
            if (position.line == UINT64_MAX)
                return UMI_STATUS_CAPACITY_EXCEEDED;
            ++position.line;
            position.utf16_column = 0U;
            offset += width;
        }
        else
        {
            size_t width;
            uint64_t units;
            UmiStatus status = PositionScalar((const unsigned char *)view->bytes + offset,
                                              view->byte_count - offset, &width, &units);
            if (status != UMI_STATUS_OK)
                return status;
            if (position.utf16_column > UINT64_MAX - units)
                return UMI_STATUS_CAPACITY_EXCEEDED;
            position.utf16_column += units;
            offset += width;
        }
    }
}
UmiStatus UmiEditorTextViewResolvePosition(const UmiEditorTextBufferView *view,
                                           UmiEditorTextPosition position, size_t *out_byte_offset)
{
    if (out_byte_offset == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t offset;
    UmiEditorTextPosition resolved;
    UmiStatus status = PositionScan(view, 0, 0U, position, &offset, &resolved);
    if (status == UMI_STATUS_OK)
        *out_byte_offset = offset;
    return status;
}
UmiStatus UmiEditorTextViewPositionAt(const UmiEditorTextBufferView *view, size_t byte_offset,
                                      UmiEditorTextPosition *out_position)
{
    if (out_position == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t offset;
    UmiEditorTextPosition resolved;
    UmiStatus status =
        PositionScan(view, 1, byte_offset, (UmiEditorTextPosition){0U, 0U}, &offset, &resolved);
    if (status == UMI_STATUS_OK)
        *out_position = resolved;
    return status;
}

/* Owned indexed queries reuse the scalar and coordinate rules above. */
#include "text_position_index.inc"
