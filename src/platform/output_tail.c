/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/output_tail.c
 * PURPOSE: Centralise allocation-free output retention without changing raw process bytes.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/output_tail.h"
#include <string.h>

/* Compare address distances without forming an end pointer that could wrap.
 * This rejects aliasing before eviction can overwrite a still-borrowed chunk. */
static bool overlap(const void *left, size_t left_size, const void *right, size_t right_size)
{
    if (left_size == 0U || right_size == 0U)
        return false;
    uintptr_t a = (uintptr_t)left, b = (uintptr_t)right;
    return a <= b ? b - a < left_size : a - b < right_size;
}

UmiStatus UmiOutputTailAppend(char *buffer, size_t capacity, UmiOutputTailState *state, const char *bytes,
                              size_t length)
{
    if (buffer == NULL || state == NULL || capacity < 2U || (length != 0U && bytes == NULL))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (overlap(buffer, capacity, state, sizeof(*state)) || overlap(bytes, length, buffer, capacity) ||
        overlap(bytes, length, state, sizeof(*state)))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (state->length >= capacity || state->total_bytes < state->length || buffer[state->length] != '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    if (length == 0U)
        return UMI_STATUS_OK;

    const size_t limit = capacity - 1U;
    if (length > UINT64_MAX - state->total_bytes)
    {
        state->total_bytes = UINT64_MAX;
        state->counters_saturated = true;
    }
    else
        state->total_bytes += (uint64_t)length;

    if (length >= limit)
    {
        state->truncated = state->truncated || state->length != 0U || length > limit;
        memcpy(buffer, bytes + length - limit, limit);
        state->length = limit;
    }
    else
    {
        size_t available = limit - state->length;
        if (length > available)
        {
            size_t discarded = length - available;
            memmove(buffer, buffer + discarded, state->length - discarded);
            state->length -= discarded;
            state->truncated = true;
        }
        memcpy(buffer + state->length, bytes, length);
        state->length += length;
    }
    buffer[state->length] = '\0';
    return UMI_STATUS_OK;
}
