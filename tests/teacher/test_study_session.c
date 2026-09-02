/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/teacher/test_study_session.c
 *
 * PURPOSE:
 *   Implement the test study session behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/teacher/study_session.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiTeacherStudySession s; umi_teacher_study_session_init(&s,"session"); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_teacher_study_session_record_activity(&s,1,80U)!=UMI_STATUS_OK) return 1; umi_teacher_study_session_record_question(&s); umi_teacher_study_session_add_minutes(&s,25U); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_teacher_study_session_completion(&s)!=100U) return 2; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_teacher_study_session_ready_to_close(&s)) return 3; return 0; }
