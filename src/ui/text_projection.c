/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/text_projection.c
 * PURPOSE: Keep presentation order separate from immutable source row identities.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/text_projection.h"
#include <stdlib.h>
#include <string.h>

struct UmiUiTextProjection
{
    char **rows;
    char *text;
    size_t *indices, *scratch, *merge;
    size_t source_count, visible_count;
};

void UmiUiTextProjectionDestroy(UmiUiTextProjection *projection)
{
    if (projection == NULL)
        return;
    free(projection->rows);
    free(projection->text);
    free(projection->indices);
    free(projection->scratch);
    free(projection->merge);
    free(projection);
}

UmiStatus UmiUiTextProjectionCreate(const char *const *rows, size_t count,
                                    UmiUiTextProjection **out_projection)
{
    if (out_projection == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_projection = NULL;
    if (count > UMI_UI_TEXT_PROJECTION_ROWS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (count != 0U && rows == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t bytes = 0U;
    for (size_t i = 0U; i < count; ++i)
    {
        if (rows[i] == NULL)
            return UMI_STATUS_INVALID_ARGUMENT;
        size_t length = 0U;
        while (length < UMI_UI_TEXT_PROJECTION_ROW_BYTES && rows[i][length] != '\0')
            ++length;
        if (length == UMI_UI_TEXT_PROJECTION_ROW_BYTES || length + 1U > UMI_UI_TEXT_PROJECTION_BYTES - bytes)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        bytes += length + 1U;
    }
    UmiUiTextProjection *projection = calloc(1U, sizeof(*projection));
    if (projection == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    if (count != 0U)
    {
        projection->rows = calloc(count, sizeof(*projection->rows));
        projection->text = malloc(bytes);
        projection->indices = malloc(count * sizeof(*projection->indices));
        projection->scratch = malloc(count * sizeof(*projection->scratch));
        projection->merge = malloc(count * sizeof(*projection->merge));
        if (projection->rows == NULL || projection->text == NULL || projection->indices == NULL ||
            projection->scratch == NULL || projection->merge == NULL)
        {
            UmiUiTextProjectionDestroy(projection);
            return UMI_STATUS_OUT_OF_MEMORY;
        }
        size_t offset = 0U;
        for (size_t i = 0U; i < count; ++i)
        {
            size_t length = strlen(rows[i]) + 1U;
            projection->rows[i] = projection->text + offset;
            memcpy(projection->rows[i], rows[i], length);
            projection->indices[i] = i;
            offset += length;
        }
    }
    projection->source_count = count;
    projection->visible_count = count;
    *out_projection = projection;
    return UMI_STATUS_OK;
}

/* Merge source indices rather than caller records. Taking the left item on an
 * equal comparison keeps duplicates stable without relying on qsort globals
 * or platform-specific context parameters. The bounded row count also bounds
 * all additions and allocation sizes used by this merge. */
static void ProjectionSort(UmiUiTextProjection *projection, size_t count,
                           const UmiUiSortFilterSnapshot *filter)
{
    for (size_t width = 1U; width < count; width *= 2U)
    {
        for (size_t start = 0U; start < count; start += 2U * width)
        {
            size_t middle = start + width < count ? start + width : count;
            size_t end = middle + width < count ? middle + width : count;
            size_t left = start, right = middle;
            for (size_t out = start; out < end; ++out)
            {
                bool take_left = right == end;
                if (left < middle && right < end)
                    take_left = umi_ui_sort_filter_model_compare_text(
                                    filter, projection->rows[projection->scratch[left]],
                                    projection->rows[projection->scratch[right]]) <= 0;
                projection->merge[out] =
                    left < middle && take_left ? projection->scratch[left++] : projection->scratch[right++];
            }
        }
        memcpy(projection->scratch, projection->merge, count * sizeof(*projection->scratch));
    }
}

UmiStatus UmiUiTextProjectionApply(UmiUiTextProjection *projection, const UmiUiSortFilterSnapshot *filter,
                                   bool sort)
{
    if (projection == NULL || filter == NULL || memchr(filter->query, '\0', sizeof(filter->query)) == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t count = 0U;
    for (size_t i = 0U; i < projection->source_count; ++i)
        if (umi_ui_sort_filter_model_matches(filter, projection->rows[i]))
            projection->scratch[count++] = i;
    if (sort)
        ProjectionSort(projection, count, filter);
    /* Publish only the finished mapping. Text and source positions never move. */
    size_t *previous = projection->indices;
    projection->indices = projection->scratch;
    projection->scratch = previous;
    projection->visible_count = count;
    return UMI_STATUS_OK;
}
size_t UmiUiTextProjectionCount(const UmiUiTextProjection *projection)
{
    return projection != NULL ? projection->visible_count : 0U;
}
size_t UmiUiTextProjectionSourceCount(const UmiUiTextProjection *projection)
{
    return projection != NULL ? projection->source_count : 0U;
}
const char *UmiUiTextProjectionText(const UmiUiTextProjection *projection, size_t source_index)
{
    return projection != NULL && source_index < projection->source_count ? projection->rows[source_index]
                                                                         : NULL;
}
UmiStatus UmiUiTextProjectionToSource(const UmiUiTextProjection *projection, size_t view_index,
                                      size_t *out_source_index)
{
    if (projection == NULL || out_source_index == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (view_index >= projection->visible_count)
        return UMI_STATUS_NOT_FOUND;
    *out_source_index = projection->indices[view_index];
    return UMI_STATUS_OK;
}
UmiStatus UmiUiTextProjectionToView(const UmiUiTextProjection *projection, size_t source_index,
                                    size_t *out_view_index)
{
    if (projection == NULL || out_view_index == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < projection->visible_count; ++i)
        if (projection->indices[i] == source_index)
        {
            *out_view_index = i;
            return UMI_STATUS_OK;
        }
    return UMI_STATUS_NOT_FOUND;
}
