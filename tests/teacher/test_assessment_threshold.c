/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/teacher/test_assessment_threshold.c
 *
 * PURPOSE:
 *   Implement the test assessment threshold behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/teacher/assessment_threshold.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiTeacherAssessmentThreshold p; /* Preserve the original failure result so the caller can respond to the correct cause. */ if (umi_teacher_assessment_threshold_configure(&p,70U,1U,2U,UMI_TEACHER_LEVEL_ADVANCED,UINT32_MAX)!=UMI_STATUS_OK) return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if (!umi_teacher_assessment_threshold_allows(&p,80U,1U,0U,UMI_TEACHER_LEVEL_BEGINNER,UMI_TEACHER_LANGUAGE_C23)) return 2; /* Preserve the original failure result so the caller can respond to the correct cause. */ if (umi_teacher_assessment_threshold_allows(&p,50U,1U,0U,UMI_TEACHER_LEVEL_BEGINNER,UMI_TEACHER_LANGUAGE_C23)) return 3; /* Preserve the original failure result so the caller can respond to the correct cause. */ if (umi_teacher_assessment_threshold_deficit(&p,50U)!=20U) return 4; return 0; }
