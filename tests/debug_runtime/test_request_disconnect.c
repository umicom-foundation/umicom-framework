/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_runtime/test_request_disconnect.c
 *
 * PURPOSE:
 *   Verify DAP disconnect is emitted through the persistent framed transport.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "request_test_support.h"
#include "umicom/debug_runtime/requests/disconnect.h"

int main(void)
{
    DebugRequestTestFixture f;
    char written[UMI_DEBUG_RUNTIME_FRAME_CAPACITY];
    uint64_t sequence = 0U;

    assert(debug_request_test_fixture_create(&f) == UMI_STATUS_OK);
    assert(umi_debug_runtime_request_disconnect(f.adapter, 0, 1, 0, &sequence) == UMI_STATUS_OK);
    assert(sequence != 0U);
    assert(debug_request_test_fixture_written(
        &f, written, sizeof(written)) == UMI_STATUS_OK);
    assert(strstr(written, "disconnect") != NULL);

    debug_request_test_fixture_destroy(&f);
    return 0;
}
