/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_runtime/validation_runner.h
 *
 * PURPOSE:
 *   Execute validation steps through the existing UmiDeveloperExecutor so AI
 *   generated changes use the same native/remote/sandbox execution boundary as
 *   every other developer operation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_RUNTIME_VALIDATION_RUNNER_H
#define UMICOM_AI_CODING_RUNTIME_VALIDATION_RUNNER_H

#include "umicom/ai_coding_runtime/validation_plan.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_ai_coding_validation_run(
    const UmiAiCodingValidationPlan *plan,
    const UmiDeveloperExecutor *executor,
    UmiAiCodingValidationReport *out_report);

#ifdef __cplusplus
}
#endif
#endif
