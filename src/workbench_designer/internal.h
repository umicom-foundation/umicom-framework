/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/internal.h
 *
 * PURPOSE:
 *   Provide bounded string, identifier, comparison and document-index helpers
 *   shared only by workbench designer implementation units.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_INTERNAL_H
#define UMICOM_WORKBENCH_DESIGNER_INTERNAL_H

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/workbench_designer/types.h"
#include "umicom/workbench_layout/document.h"

static inline UmiStatus umi_workbench_designer_copy_text(
    char *destination,
    size_t capacity,
    const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(source);
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

static inline void umi_workbench_designer_clear_text(
    char *destination,
    size_t capacity)
{
    if (destination != NULL && capacity > 0U) destination[0] = '\0';
}

static inline bool umi_workbench_designer_text_equal(
    const char *left,
    const char *right)
{
    return left != NULL && right != NULL && strcmp(left, right) == 0;
}

static inline bool umi_workbench_designer_text_contains_case_insensitive(
    const char *text,
    const char *query)
{
    size_t text_length;
    size_t query_length;
    size_t index;
    size_t query_index;
    if (text == NULL || query == NULL) return false;
    query_length = strlen(query);
    if (query_length == 0U) return true;
    text_length = strlen(text);
    if (query_length > text_length) return false;
    for (index = 0U; index + query_length <= text_length; ++index) {
        for (query_index = 0U; query_index < query_length; ++query_index) {
            const unsigned char left = (unsigned char)text[index + query_index];
            const unsigned char right = (unsigned char)query[query_index];
            if (tolower(left) != tolower(right)) break;
        }
        if (query_index == query_length) return true;
    }
    return false;
}

static inline UmiWorkbenchLayoutRect umi_workbench_designer_to_layout_rect(
    UmiWorkbenchDesignerRect rect)
{
    UmiWorkbenchLayoutRect result;
    result.x = (int32_t)rect.x;
    result.y = (int32_t)rect.y;
    result.width = (int32_t)rect.width;
    result.height = (int32_t)rect.height;
    return result;
}

static inline UmiWorkbenchDesignerRect umi_workbench_designer_from_layout_rect(
    UmiWorkbenchLayoutRect rect)
{
    UmiWorkbenchDesignerRect result;
    result.x = (double)rect.x;
    result.y = (double)rect.y;
    result.width = (double)rect.width;
    result.height = (double)rect.height;
    return result;
}

static inline bool umi_workbench_designer_document_index_valid(
    const UmiWorkbenchLayoutDocument *document,
    size_t index)
{
    return document != NULL && index < document->node_count;
}

static inline size_t umi_workbench_designer_parent_index(
    const UmiWorkbenchLayoutDocument *document,
    size_t child_index)
{
    size_t index;
    size_t child_position;
    if (!umi_workbench_designer_document_index_valid(document, child_index)) {
        return UMI_WORKBENCH_LAYOUT_INDEX_NONE;
    }
    for (index = 0U; index < document->node_count; ++index) {
        const UmiWorkbenchLayoutNode *node = &document->nodes[index];
        for (child_position = 0U;
             child_position < node->child_count;
             ++child_position) {
            if (node->child_indices[child_position] == child_index) return index;
        }
    }
    return UMI_WORKBENCH_LAYOUT_INDEX_NONE;
}

#endif
