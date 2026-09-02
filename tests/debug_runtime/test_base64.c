/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_runtime/test_base64.c
 *
 * PURPOSE:
 *   Verify bounded DAP memory base64 decoding.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/debug_runtime/base64.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    unsigned char bytes[8];
    size_t count = 0U;

    assert(umi_debug_runtime_base64_decode(
        "AQIDBA==", bytes, sizeof(bytes), &count) == UMI_STATUS_OK);
    assert(count == 4U);
    assert(bytes[0] == 1U);
    assert(bytes[3] == 4U);
    return 0;
}
