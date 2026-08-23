/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_runtime/test_cancellation.c
 *
 * PURPOSE:
 *   Verify the reusable AI coding runtime cancellation contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/ai_coding_runtime/cancellation.h"

int main(void)
{

    UmiAiCodingCancellation token;
    umi_ai_coding_cancellation_init(&token);
    assert(!umi_ai_coding_cancellation_is_requested(&token));
    umi_ai_coding_cancellation_request(&token);
    assert(umi_ai_coding_cancellation_is_requested(&token));

    return 0;
}
