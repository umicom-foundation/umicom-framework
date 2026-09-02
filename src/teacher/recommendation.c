/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/teacher/recommendation.c
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
#include "umicom/teacher/recommendation.h"
#include <string.h>
/*
 * Initialise teacher recommendation from caller-provided values so later operations
 * receive a known state.
 */
void umi_teacher_recommendation_init(UmiTeacherRecommendationScore *score) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(score!=NULL) memset(score,0,sizeof(*score));
    }
/*
 * Provide the teacher recommendation compute operation used by this module and its client
 * applications.
 */
uint32_t umi_teacher_recommendation_compute(const UmiTeacherRecommendationScore *score) { uint64_t raw;
    uint32_t value;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(score==NULL) return 0U;
    raw=(uint64_t)umi_teacher_clamp_score(score->skill_gap)*45U+(uint64_t)umi_teacher_clamp_score(score->relevance)*35U+(uint64_t)umi_teacher_clamp_score(score->readiness)*20U;
    value=(uint32_t)(raw/100U);
    return score->fatigue_penalty>=value?0U:value-score->fatigue_penalty;
    }
/*
 * Provide the teacher recommendation prefer operation used by this module and its client
 * applications.
 */
int umi_teacher_recommendation_prefer(const UmiTeacherRecommendationScore *left,const UmiTeacherRecommendationScore *right) { return umi_teacher_recommendation_compute(left)>umi_teacher_recommendation_compute(right);
    }
