/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_runtime/test_request_set_data_breakpoints.c
 *
 * PURPOSE:
 *   Verify DAP setDataBreakpoints is emitted through the persistent framed transport.
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
#include "umicom/debug_runtime/requests/set_data_breakpoints.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    DebugRequestTestFixture f;
    char written[UMI_DEBUG_RUNTIME_FRAME_CAPACITY];
    uint64_t sequence = 0U;

    assert(debug_request_test_fixture_create(&f) == UMI_STATUS_OK);
    assert(umi_debug_runtime_request_set_data_breakpoints(f.adapter, NULL, 0U, &sequence) == UMI_STATUS_OK);
    assert(sequence != 0U);
    assert(debug_request_test_fixture_written(
        &f, written, sizeof(written)) == UMI_STATUS_OK);
    assert(strstr(written, "setDataBreakpoints") != NULL);

    debug_request_test_fixture_destroy(&f);
    return 0;
}
