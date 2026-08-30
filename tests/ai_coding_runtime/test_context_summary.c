/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_runtime/test_context_summary.c
 *
 * PURPOSE:
 *   Verify AI coding runtime context summary behavior.
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
#include "umicom/ai_coding_runtime/context_summary.h"

int main(void)
{

    UmiAiCodingMaterializedContext context = {0};
    char output[4096];
    context.file_count = 1U;
    context.estimated_tokens = 12U;
    (void)strcpy(context.files[0].path, "src/main.c");
    (void)strcpy(context.files[0].language_id, "c");
    context.files[0].length = 10U;
    context.files[0].hash = 42U;
    context.files[0].active = 1;
    assert(umi_ai_coding_context_summary(
        &context, output, sizeof(output)) == UMI_STATUS_OK);
    assert(strstr(output, "src/main.c") != NULL);
    assert(strstr(output, "* ") != NULL);

    return 0;
}
