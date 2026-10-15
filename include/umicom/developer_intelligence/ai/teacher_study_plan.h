/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_intelligence/ai/teacher_study_plan.h
 *
 * PURPOSE:
 *   Evaluate AI Teacher Study Plan readiness and policy using bounded copied signals.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_INTELLIGENCE_AI_TEACHER_STUDY_PLAN_H
#define UMICOM_DEVELOPER_INTELLIGENCE_AI_TEACHER_STUDY_PLAN_H

#include <stddef.h>
#include "umicom/developer_intelligence/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus UmiDeveloperIntelligenceEvaluateAiTeacherStudyPlan(
    const UmiDeveloperIntelligenceInput *input,
    UmiDeveloperIntelligenceSnapshot *outSnapshot);


/** Evaluate a complete group using this policy without publishing partial rows.
 * Zero count is a no-op and permits NULL arrays. Nonempty groups require arrays
 * with count readable inputs and capacity writable outputs. At most 4096 rows
 * and 16 MiB of temporary output are accepted; allocation can fail.
 * Any failure preserves all outputs. Non-finite signals and overlong labels
 * are refused. Inputs may borrow output storage until publication. The caller
 * serializes access; this calculation does not submit orders or execute jobs.
 */
UmiStatus UmiDeveloperIntelligenceEvaluateAiTeacherStudyPlanBatch(const UmiDeveloperIntelligenceInput *inputs,
    size_t count, UmiDeveloperIntelligenceSnapshot *outputs, size_t capacity);

#ifdef __cplusplus
}
#endif
#endif
