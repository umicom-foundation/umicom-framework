/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_cancellation.c
 *
 * PURPOSE:
 *   Verify cancellation request, reset and generation behaviour used by
 *   processes, watchers and searches.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/platform/cancellation.h"

#include <assert.h>
#include <stddef.h>

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiCancellationToken *token = NULL;
    uint64_t generation;
    assert(umi_cancellation_token_create(&token) == UMI_STATUS_OK);
    assert(!umi_cancellation_token_is_requested(token));
    generation = umi_cancellation_token_generation(token);
    umi_cancellation_token_request(token);
    assert(umi_cancellation_token_is_requested(token));
    assert(umi_cancellation_token_generation(token) > generation);
    generation = umi_cancellation_token_generation(token);
    umi_cancellation_token_reset(token);
    assert(!umi_cancellation_token_is_requested(token));
    assert(umi_cancellation_token_generation(token) > generation);
    umi_cancellation_token_destroy(token);
    return 0;
}
