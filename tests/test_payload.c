/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_payload.c
 *
 * PURPOSE:
 *   Verify owned text and binary payload creation, cloning and cleanup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/umicom.h"

#include <assert.h>
#include <string.h>

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiPayload payload;
    UmiPayload clone;
    assert(umi_payload_create_text("hello", &payload) == UMI_STATUS_OK);
    assert(payload.size == 5U);
    assert(strcmp(umi_payload_text(&payload), "hello") == 0);
    assert(umi_payload_clone(&payload, &clone) == UMI_STATUS_OK);
    assert(clone.data != payload.data);
    assert(memcmp(clone.data, payload.data, payload.size) == 0);
    umi_payload_dispose(&clone);
    umi_payload_dispose(&payload);
    return 0;
}
