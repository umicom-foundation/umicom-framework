/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ai_embedding.c
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
#include "umicom/umicom.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    const float a_values[3] = {1.0F, 0.0F, 0.0F};
    const float b_values[3] = {1.0F, 0.0F, 0.0F};
    UmiAiEmbedding a;
    UmiAiEmbedding b;
    assert(umi_ai_embedding_set(&a, a_values, 3U) == UMI_STATUS_OK);
    assert(umi_ai_embedding_set(&b, b_values, 3U) == UMI_STATUS_OK);
    assert(umi_ai_embedding_cosine(&a, &b) > 0.99);
    return 0;
}
