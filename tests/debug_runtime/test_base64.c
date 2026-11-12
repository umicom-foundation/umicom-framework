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
/* Required fixture setup and checks also execute in optimized builds. */
#include "../support/assertions.h"
#include <assert.h>
#include <string.h>
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
    /* Prefix bytes must never become a successful memory observation. */
    const char *invalid[] = {"A", "AQ", "AQI", "AQ==tail", "AQ=",
        "AQ===", "AR==", "AQJ=", "=QID", "AQ!D"};
    for (size_t i = 0U; i < sizeof(invalid) / sizeof(invalid[0]); ++i) {
        unsigned char before[sizeof(bytes)];
        memset(bytes, 0xA5, sizeof(bytes));
        memcpy(before, bytes, sizeof(bytes));
        count = 47U;
        assert(umi_debug_runtime_base64_decode(invalid[i], bytes,
            sizeof(bytes), &count) == UMI_STATUS_PARSE_ERROR);
        assert(count == 47U && memcmp(before, bytes, sizeof(bytes)) == 0);
    }
    count = 47U;
    assert(umi_debug_runtime_base64_decode("AQIDBA==", bytes, 3U,
        &count) == UMI_STATUS_CAPACITY_EXCEEDED);
    assert(count == 47U && bytes[0] == 0xA5U && bytes[2] == 0xA5U);
    assert(umi_debug_runtime_base64_decode(" A Q I D B A = = ", bytes,
        sizeof(bytes), &count) == UMI_STATUS_OK);
    assert(count == 4U && bytes[0] == 1U && bytes[3] == 4U);
    return 0;
}
