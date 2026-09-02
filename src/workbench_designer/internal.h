/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/internal.h
 *
 * PURPOSE:
 *   Provide bounded string, identifier, comparison and document-index helpers
 *   shared only by workbench designer implementation units.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_INTERNAL_H
#define UMICOM_WORKBENCH_DESIGNER_INTERNAL_H

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/workbench_designer/types.h"
#include "umicom/workbench_layout/document.h"

/**
 * Provide the workbench designer copy text operation used by this module and its client
 * applications.
 */
static inline UmiStatus umi_workbench_designer_copy_text(
    char *destination,
    size_t capacity,
    const char *source)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

/**
 * Provide the workbench designer clear text operation used by this module and its client
 * applications.
 */
static inline void umi_workbench_designer_clear_text(
    char *destination,
    size_t capacity)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination != NULL && capacity > 0U) destination[0] = '\0';
}

/**
 * Provide the workbench designer text equal operation used by this module and its client
 * applications.
 */
static inline bool umi_workbench_designer_text_equal(
    const char *left,
    const char *right)
{
    return left != NULL && right != NULL && strcmp(left, right) == 0;
}

/**
 * Provide the workbench designer text contains case insensitive operation used by this
 * module and its client applications.
 */
static inline bool umi_workbench_designer_text_contains_case_insensitive(
    const char *text,
    const char *query)
{
    size_t text_length;
    size_t query_length;
    size_t index;
    size_t query_index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL || query == NULL) return false;
    query_length = strlen(query);
    /* Apply this branch only when its contract condition is satisfied. */
    if (query_length == 0U) return true;
    text_length = strlen(text);
    /* Apply this branch only when its contract condition is satisfied. */
    if (query_length > text_length) return false;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index + query_length <= text_length; ++index) {
        /* Visit each bounded item once so every record receives the same rule. */
        for (query_index = 0U; query_index < query_length; ++query_index) {
            const unsigned char left = (unsigned char)text[index + query_index];
            const unsigned char right = (unsigned char)query[query_index];
            /* Apply this branch only when its contract condition is satisfied. */
            if (tolower(left) != tolower(right)) break;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (query_index == query_length) return true;
    }
    return false;
}

/**
 * Provide the workbench designer to layout rect operation used by this module and its
 * client applications.
 */
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

/**
 * Provide the workbench designer from layout rect operation used by this module and its
 * client applications.
 */
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

/**
 * Check that workbench designer document index satisfies its contract before another
 * service relies on it.
 */
static inline bool umi_workbench_designer_document_index_valid(
    const UmiWorkbenchLayoutDocument *document,
    size_t index)
{
    return document != NULL && index < document->node_count;
}

/**
 * Provide the workbench designer parent index operation used by this module and its client
 * applications.
 */
static inline size_t umi_workbench_designer_parent_index(
    const UmiWorkbenchLayoutDocument *document,
    size_t child_index)
{
    size_t index;
    size_t child_position;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_workbench_designer_document_index_valid(document, child_index)) {
        return UMI_WORKBENCH_LAYOUT_INDEX_NONE;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < document->node_count; ++index) {
        const UmiWorkbenchLayoutNode *node = &document->nodes[index];
        /* Visit each bounded item once so every record receives the same rule. */
        for (child_position = 0U;
             child_position < node->child_count;
             ++child_position) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (node->child_indices[child_position] == child_index) return index;
        }
    }
    return UMI_WORKBENCH_LAYOUT_INDEX_NONE;
}

#endif
