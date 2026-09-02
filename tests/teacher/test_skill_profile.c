/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/teacher/test_skill_profile.c
 *
 * PURPOSE:
 *   Implement the test skill profile behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/teacher/skill_profile.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiTeacherSkillProfile p; umi_teacher_skill_profile_init(&p); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_teacher_skill_profile_set(&p,"pointers",80U,70U)!=UMI_STATUS_OK) return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_teacher_skill_profile_set(&p,"arrays",60U,80U)!=UMI_STATUS_OK) return 2; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_teacher_skill_profile_average_mastery(&p)!=70U) return 3; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_teacher_skill_profile_find(&p,"pointers")==0) return 4; return 0; }
