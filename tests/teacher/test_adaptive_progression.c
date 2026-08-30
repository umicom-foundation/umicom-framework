/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/teacher/test_adaptive_progression.c
 *
 * PURPOSE:
 *   Implement the test adaptive progression behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/teacher/adaptive_progression.h"
int main(void) { UmiTeacherAdaptiveProgression m; umi_teacher_adaptive_progression_init(&m); if(umi_teacher_adaptive_progression_decide(&m,85U,70U)!=UMI_TEACHER_PROGRESS_ADVANCE) return 1; if(umi_teacher_adaptive_progression_decide(&m,30U,70U)!=UMI_TEACHER_PROGRESS_REMEDIATE) return 2; if(umi_teacher_adaptive_progression_decide(&m,60U,20U)!=UMI_TEACHER_PROGRESS_HOLD) return 3; return 0; }
