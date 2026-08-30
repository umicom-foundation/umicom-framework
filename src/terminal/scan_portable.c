/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/scan_portable.c
 *
 * PURPOSE:
 *   Implement the scan portable behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Portable C terminal transcript byte scanner. */
#include "scan_internal.h"

uint64_t umi_terminal_scan_count_byte_portable(const unsigned char *bytes,
                                               size_t byte_count,
                                               unsigned char value)
{
    size_t index;
    uint64_t count = 0U;
    for (index = 0U; index < byte_count; ++index) {
        count += bytes[index] == value ? 1U : 0U;
    }
    return count;
}
