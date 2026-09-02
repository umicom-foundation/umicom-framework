/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/teacher/test_mastery_model.c
 *
 * PURPOSE:
 *   Implement the test mastery model behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/teacher/mastery_model.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiTeacherMasteryModel m; umi_teacher_mastery_model_init(&m); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_teacher_mastery_model_observe(&m,80U,40U)!=UMI_STATUS_OK) return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_teacher_mastery_model_observe(&m,100U,40U)!=UMI_STATUS_OK) return 2; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_teacher_mastery_model_value(&m)!=90U) return 3; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_teacher_mastery_model_meets(&m,85U,70U)) return 4; return 0; }
