/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/instruction_page.c
 * PURPOSE: Publish complete instruction pages and compare their descriptive frame context without partial output.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "instruction_fields_internal.h"
#include <stdlib.h>
#include <string.h>
/* A whole temporary page makes failure atomic for caller-owned output storage. */
UmiStatus UmiDebugInstructionPageDecode(const char *json, uint32_t requested,
                                        UmiDebugInstructionPage *out)
{
    if (json == NULL || out == NULL || requested == 0U ||
        requested > UMI_DEBUG_INSTRUCTION_PAGE_LIMIT)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiDebugInstructionPage *page = calloc(1U, sizeof *page);
    if (page == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = DebugInstructionArrayDecode(json, page->items,
                                                   UMI_DEBUG_INSTRUCTION_PAGE_LIMIT, &page->count);
    if (status == UMI_STATUS_OK && page->count != requested)
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK)
        *out = *page;
    free(page);
    return status;
}
/* Compare only meaningful fields, never structure padding or local pointer identities. */
int UmiDebugInstructionContextEqual(const UmiDebugInstructionContext *left,
                                    const UmiDebugInstructionContext *right)
{
    if (left == NULL || right == NULL ||
        memchr(left->connection.session_id, 0, sizeof left->connection.session_id) == NULL ||
        memchr(right->connection.session_id, 0, sizeof right->connection.session_id) == NULL ||
        memchr(left->reference, 0, sizeof left->reference) == NULL ||
        memchr(right->reference, 0, sizeof right->reference) == NULL)
        return 0;
    return left->connection.generation == right->connection.generation &&
           strcmp(left->connection.session_id, right->connection.session_id) == 0 &&
           left->revision == right->revision && left->thread_id == right->thread_id &&
           left->frame_id == right->frame_id && strcmp(left->reference, right->reference) == 0;
}

/* Keep user input exact and bounded before serializing it as a protocol integer. */
UmiStatus UmiDebugInstructionOffsetRead(const char *text, int64_t *out)
{
    if (text == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    int negative = *text == '-';
    const char *digits = text + negative;
    if (*digits == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    uint64_t value = 0U;
    size_t count = 0U;
    for (; *digits != '\0'; ++digits)
    {
        if (++count > 16U || *digits < '0' || *digits > '9')
            return UMI_STATUS_INVALID_ARGUMENT;
        unsigned digit = (unsigned)(*digits - '0');
        if (value > ((uint64_t)UMI_DEBUG_INSTRUCTION_OFFSET_LIMIT - digit) / 10U)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        value = value * 10U + digit;
    }
    *out = negative ? -(int64_t)value : (int64_t)value;
    return UMI_STATUS_OK;
}
