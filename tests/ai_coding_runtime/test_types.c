/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_runtime/test_types.c
 *
 * PURPOSE:
 *   Verify the reusable AI coding runtime types contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/ai_coding_runtime/types.h"

int main(void)
{

    assert(strcmp(
        umi_ai_coding_runtime_state_text(UMI_AI_CODING_RUNTIME_VALIDATING),
        "validating") == 0);
    assert(strcmp(
        umi_ai_coding_validation_kind_text(UMI_AI_CODING_VALIDATION_BUILD),
        "build") == 0);
    assert(strcmp(
        umi_ai_coding_event_kind_text(UMI_AI_CODING_EVENT_PATCH),
        "patch") == 0);

    return 0;
}
