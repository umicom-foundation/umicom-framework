/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/output_tail.h
 * PURPOSE: Retain bounded raw output for compiler, test and other worker processes.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_PLATFORM_OUTPUT_TAIL_H
#define UMICOM_PLATFORM_OUTPUT_TAIL_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C"
{
#endif

    /* The caller owns storage and serialises writers/readers. Zero-initialise this
 * state and buffer[0] before first use. capacity includes a spare terminator.
 * length counts raw bytes, including NUL; text frontends repair display copies.
 * Reuse this policy for new process consumers instead of growing output without
 * a limit. No allocation, callback, locking or character conversion occurs here. */
    typedef struct UmiOutputTailState
    {
        size_t length;
        uint64_t total_bytes;
        bool truncated;
        bool counters_saturated;
    } UmiOutputTailState;

    /* Keep the newest capacity-1 bytes and set buffer[state->length] to NUL.
 * State, destination and nonempty input must occupy separate memory regions.
 * Empty input permits NULL bytes. Invalid inputs leave state/storage unchanged.
 * The caller supplies valid readable/writable regions of the stated sizes.
 * Saturated byte counts remain UINT64_MAX; truncation never implies success. */
    UmiStatus UmiOutputTailAppend(char *buffer, size_t capacity, UmiOutputTailState *state, const char *bytes,
                                  size_t length);

#ifdef __cplusplus
}
#endif
#endif
