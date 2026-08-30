/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_runtime/test_validation_feedback.c
 *
 * PURPOSE:
 *   Verify AI coding runtime validation feedback behavior.
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
#include "umicom/ai_coding_runtime/validation_feedback.h"

int main(void)
{

    UmiAiCodingValidationReport report = {0};
    char output[4096];
    report.result_count = 1U;
    report.failed_count = 1U;
    report.required_failed_count = 1U;
    (void)strcpy(report.results[0].step_id, "build");
    report.results[0].kind = UMI_AI_CODING_VALIDATION_BUILD;
    report.results[0].exit_code = 1;
    report.results[0].required = 1;
    (void)strcpy(report.results[0].output, "compiler error");
    assert(umi_ai_coding_validation_feedback(
        &report, output, sizeof(output)) == UMI_STATUS_OK);
    assert(strstr(output, "compiler error") != NULL);

    return 0;
}
