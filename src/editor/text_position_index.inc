/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/text_position_index.inc
 * PURPOSE: Share Unicode scanning while indexing immutable source for repeated edit ranges.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/editor/text_position_index.h"
#include <stdlib.h>
#include <string.h>
typedef struct PositionCheckpoint
{
    size_t offset;
    UmiEditorTextPosition position;
} PositionCheckpoint;
struct UmiEditorTextPositionIndex
{
    char *bytes;
    size_t length, count, capacity;
    PositionCheckpoint *points;
    uint64_t revision, save_revision;
    int dirty;
};
/* The same scalar decoder as the original view scanner prevents the indexed
 * and one-off APIs from disagreeing about valid Unicode or line boundaries. */
static UmiStatus PositionAdvance(const char *bytes, size_t length, PositionCheckpoint *point)
{
    unsigned char first = (unsigned char)bytes[point->offset];
    if (first == '\r' || first == '\n')
    {
        point->offset +=
            first == '\r' && point->offset + 1U < length && bytes[point->offset + 1U] == '\n' ? 2U : 1U;
        ++point->position.line;
        point->position.utf16_column = 0U;
        return UMI_STATUS_OK;
    }
    size_t width;
    uint64_t units;
    UmiStatus status =
        PositionScalar((const unsigned char *)bytes + point->offset, length - point->offset, &width, &units);
    if (status == UMI_STATUS_OK)
    {
        point->offset += width;
        point->position.utf16_column += units;
    }
    return status;
}
UmiStatus UmiEditorTextPositionIndexCreate(const UmiEditorTextBufferView *view,
                                           const UmiCancellationToken *cancel,
                                           UmiEditorTextPositionIndex **out_index)
{
    if (out_index == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_index = NULL;
    if (view == NULL || view->struct_size != sizeof(*view) ||
        view->api_version != UMI_EDITOR_TEXT_BUFFER_API_VERSION ||
        (view->bytes == NULL && view->byte_count != 0U) || view->byte_count > view->capacity)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (view->byte_count > 16U * 1024U * 1024U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (umi_cancellation_token_is_requested(cancel))
        return UMI_STATUS_CANCELLED;
    UmiEditorTextPositionIndex *index = calloc(1U, sizeof(*index));
    if (index == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    index->length = view->byte_count;
    /* Carry the source revision with the owned view. A host can compare this
     * capture with its document before using coordinates for a live edit. */
    index->revision = view->revision;
    index->save_revision = view->save_revision;
    index->dirty = view->dirty;
    index->capacity = index->length / 128U + 2U;
    index->bytes = malloc(index->length + 1U);
    index->points = calloc(index->capacity, sizeof(*index->points));
    if (index->bytes == NULL || index->points == NULL)
    {
        UmiEditorTextPositionIndexDestroy(index);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    if (index->length != 0U)
        memcpy(index->bytes, view->bytes, index->length);
    index->bytes[index->length] = '\0';
    PositionCheckpoint point = {0};
    index->points[index->count++] = point;
    UmiStatus status = UMI_STATUS_OK;
    while (point.offset < index->length && status == UMI_STATUS_OK)
    {
        if (point.offset - index->points[index->count - 1U].offset >= 128U)
        {
            if (umi_cancellation_token_is_requested(cancel))
            {
                status = UMI_STATUS_CANCELLED;
                break;
            }
            if (index->count == index->capacity)
            {
                status = UMI_STATUS_INTERNAL_ERROR;
                break;
            }
            index->points[index->count++] = point;
        }
        status = PositionAdvance(index->bytes, index->length, &point);
    }
    if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status != UMI_STATUS_OK)
        UmiEditorTextPositionIndexDestroy(index);
    else
        *out_index = index;
    return status;
}
void UmiEditorTextPositionIndexDestroy(UmiEditorTextPositionIndex *index)
{
    if (index != NULL)
    {
        free(index->bytes);
        free(index->points);
        free(index);
    }
}
static int PositionCheckpointCompare(PositionCheckpoint point, int by_offset, size_t offset,
                                     UmiEditorTextPosition position)
{
    if (by_offset)
        return point.offset == offset ? 0 : (point.offset < offset ? -1 : 1);
    return PositionCompare(point.position, position);
}
static UmiStatus PositionIndexScan(const UmiEditorTextPositionIndex *index, int by_offset, size_t offset,
                                   UmiEditorTextPosition position, PositionCheckpoint *out)
{
    if (index == NULL || (by_offset && offset > index->length))
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t low = 0U, high = index->count;
    while (high - low > 1U)
    {
        size_t middle = low + (high - low) / 2U;
        if (PositionCheckpointCompare(index->points[middle], by_offset, offset, position) <= 0)
            low = middle;
        else
            high = middle;
    }
    PositionCheckpoint point = index->points[low];
    for (;;)
    {
        int order = PositionCheckpointCompare(point, by_offset, offset, position);
        if (order == 0)
        {
            *out = point;
            return UMI_STATUS_OK;
        }
        if (order > 0 || point.offset == index->length)
            return UMI_STATUS_INVALID_ARGUMENT;
        UmiStatus status = PositionAdvance(index->bytes, index->length, &point);
        if (status != UMI_STATUS_OK)
            return status;
    }
}
UmiStatus UmiEditorTextPositionIndexResolve(const UmiEditorTextPositionIndex *index,
                                            UmiEditorTextPosition position, size_t *out_byte_offset)
{
    if (out_byte_offset == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    PositionCheckpoint point;
    UmiStatus status = PositionIndexScan(index, 0, 0U, position, &point);
    if (status == UMI_STATUS_OK)
        *out_byte_offset = point.offset;
    return status;
}
UmiStatus UmiEditorTextPositionIndexAt(const UmiEditorTextPositionIndex *index, size_t byte_offset,
                                       UmiEditorTextPosition *out_position)
{
    if (out_position == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    PositionCheckpoint point;
    UmiStatus status = PositionIndexScan(index, 1, byte_offset, (UmiEditorTextPosition){0}, &point);
    if (status == UMI_STATUS_OK)
        *out_position = point.position;
    return status;
}
UmiStatus UmiEditorTextPositionIndexView(const UmiEditorTextPositionIndex *index,
                                         UmiEditorTextBufferView *out_view)
{
    if (index == NULL || out_view == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiEditorTextBufferView view = {0};
    view.struct_size = (uint32_t)sizeof(view);
    view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
    view.bytes = index->bytes;
    view.byte_count = index->length;
    view.capacity = index->length;
    view.revision = index->revision;
    view.save_revision = index->save_revision;
    view.dirty = index->dirty;
    *out_view = view;
    return UMI_STATUS_OK;
}
