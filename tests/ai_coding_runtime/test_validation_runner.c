/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_runtime/test_validation_runner.c
 *
 * PURPOSE:
 *   Verify AI coding runtime validation runner behavior.
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
#include "umicom/ai_coding_runtime/validation_runner.h"

static UmiStatus fake_execute(
    void *user_data,
    const UmiDeveloperOperationSnapshot *operation,
    UmiDeveloperExecutionResult *out_result)
{
    (void)user_data;
    assert(operation != NULL);
    assert(out_result != NULL);
    (void)memset(out_result, 0, sizeof(*out_result));
    out_result->struct_size = (uint32_t)sizeof(*out_result);
    out_result->api_version = UMI_DEVELOPER_EXECUTION_API_VERSION;
    out_result->launched = 1;
    out_result->exit_code = 0;
    (void)strcpy(out_result->output, "ok");
    return UMI_STATUS_OK;
}

int main(void)
{

    UmiAiCodingValidationPlan plan;
    UmiAiCodingValidationStep step = {0};
    UmiDeveloperExecutor executor;
    UmiAiCodingValidationReport report;

    umi_ai_coding_validation_plan_init(&plan);
    (void)strcpy(step.id, "build");
    (void)strcpy(step.label, "Build");
    (void)strcpy(step.executable, "fake");
    step.kind = UMI_AI_CODING_VALIDATION_BUILD;
    step.required = 1;
    step.enabled = 1;
    assert(umi_ai_coding_validation_plan_add(&plan, &step) == UMI_STATUS_OK);

    umi_developer_executor_init(&executor, fake_execute, NULL);
    assert(umi_ai_coding_validation_run(
        &plan, &executor, &report) == UMI_STATUS_OK);
    assert(report.passed);
    assert(report.passed_count == 1U);

    return 0;
}
