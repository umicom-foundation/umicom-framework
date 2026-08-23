/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_runtime/test_validation_plan.c
 *
 * PURPOSE:
 *   Verify AI coding runtime validation plan behavior.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/ai_coding_runtime/validation_plan.h"

int main(void)
{

    UmiAiCodingValidationPlan plan;
    UmiAiCodingValidationStep step = {0};
    UmiAiCodingValidationStep output;
    umi_ai_coding_validation_plan_init(&plan);
    (void)strcpy(step.id, "build");
    (void)strcpy(step.label, "Build");
    (void)strcpy(step.executable, "cmake");
    step.kind = UMI_AI_CODING_VALIDATION_BUILD;
    step.enabled = 1;
    step.required = 1;
    assert(umi_ai_coding_validation_plan_add(&plan, &step) == UMI_STATUS_OK);
    assert(umi_ai_coding_validation_plan_at(&plan, 0U, &output) ==
           UMI_STATUS_OK);
    assert(strcmp(output.id, "build") == 0);

    return 0;
}
