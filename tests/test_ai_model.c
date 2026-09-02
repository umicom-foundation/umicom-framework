/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ai_model.c
 *
 * PURPOSE:
 *   Validate the AI and Helix foundation introduced by Batch 10.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This focused executable keeps one contract easy to diagnose when the larger test suite reports a failure.
 */



#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "umicom/umicom.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiAiModel model;
    assert(umi_ai_model_init(&model) == UMI_STATUS_OK);
    (void)snprintf(model.model_id, sizeof(model.model_id), "%s", "qwen-local");
    model.context_tokens = 32768U;
    model.max_output_tokens = 4096U;
    assert(strcmp(model.model_id, "qwen-local") == 0);
    assert(model.context_tokens == 32768U);
    return 0;
}
