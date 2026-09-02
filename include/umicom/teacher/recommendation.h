/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/recommendation.h
 *
 * PURPOSE:
 *   Rank candidate lessons/exercises by gap, relevance and readiness.
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
#ifndef UMICOM_TEACHER_RECOMMENDATION_H
#define UMICOM_TEACHER_RECOMMENDATION_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/teacher/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the teacher recommendation score data shared with callers of this public
 * contract.
 */
typedef struct UmiTeacherRecommendationScore { uint32_t skill_gap; uint32_t relevance; uint32_t readiness; uint32_t fatigue_penalty; } UmiTeacherRecommendationScore;
/**
 * Initialise teacher recommendation from caller-provided values so later operations
 * receive a known state.
 */
void umi_teacher_recommendation_init(UmiTeacherRecommendationScore *score);
/**
 * Provide the teacher recommendation compute operation used by this module and its client
 * applications.
 */
uint32_t umi_teacher_recommendation_compute(const UmiTeacherRecommendationScore *score);
/**
 * Provide the teacher recommendation prefer operation used by this module and its client
 * applications.
 */
int umi_teacher_recommendation_prefer(const UmiTeacherRecommendationScore *left, const UmiTeacherRecommendationScore *right);

#ifdef __cplusplus
}
#endif

#endif
