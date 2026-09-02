/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/teacher/test_recommendation.c
 *
 * PURPOSE:
 *   Implement the test recommendation behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/teacher/recommendation.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiTeacherRecommendationScore a={80U,90U,75U,0U},b={20U,50U,50U,0U}; /* Apply this branch only when its contract condition is satisfied. */ if(umi_teacher_recommendation_compute(&a)<70U) return 1; /* Apply this branch only when its contract condition is satisfied. */ if(!umi_teacher_recommendation_prefer(&a,&b)) return 2; return 0; }
