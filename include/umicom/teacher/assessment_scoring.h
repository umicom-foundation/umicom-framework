/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/assessment_scoring.h
 *
 * PURPOSE:
 *   Normalize assessment component scores into one result.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable learning capability. Umicom Studio, Desk and
 *   future applications are thin consumers and do not reimplement pedagogy,
 *   progression, assessment or AI Teacher orchestration.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEACHER_ASSESSMENT_SCORING_H
#define UMICOM_TEACHER_ASSESSMENT_SCORING_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/teacher/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the teacher assessment scoring data shared with callers of this public
 * contract.
 */
typedef struct UmiTeacherAssessmentScoring {
    uint32_t correctness_weight;
    uint32_t quality_weight;
    uint32_t efficiency_weight;
    uint32_t minimum_score;
} UmiTeacherAssessmentScoring;

/**
 * Initialise teacher assessment scoring from caller-provided values so later operations
 * receive a known state.
 */
void umi_teacher_assessment_scoring_init(UmiTeacherAssessmentScoring *rubric);
/**
 * Provide the teacher assessment scoring configure operation used by this module and its
 * client applications.
 */
UmiStatus umi_teacher_assessment_scoring_configure(UmiTeacherAssessmentScoring *rubric, uint32_t correctness_weight, uint32_t quality_weight, uint32_t efficiency_weight, uint32_t minimum_score);
/**
 * Provide the teacher assessment scoring compute operation used by this module and its
 * client applications.
 */
uint32_t umi_teacher_assessment_scoring_compute(const UmiTeacherAssessmentScoring *rubric, uint32_t correctness, uint32_t quality, uint32_t efficiency);
/**
 * Provide the teacher assessment scoring passes operation used by this module and its
 * client applications.
 */
int umi_teacher_assessment_scoring_passes(const UmiTeacherAssessmentScoring *rubric, uint32_t score);

#ifdef __cplusplus
}
#endif

#endif
