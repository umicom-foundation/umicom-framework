/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/text_scan_portable.c
 *
 * PURPOSE:
 *   Implement the text scan portable behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework portable editor scanner.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/

#include "text_scan_internal.h"

uint64_t umi_editor_text_scan_count_byte_portable(const unsigned char *bytes,
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

uint64_t umi_editor_text_scan_count_non_ascii_portable(
    const unsigned char *bytes,
    size_t byte_count)
{
    size_t index;
    uint64_t count = 0U;
    for (index = 0U; index < byte_count; ++index) {
        count += bytes[index] >= 0x80U ? 1U : 0U;
    }
    return count;
}
