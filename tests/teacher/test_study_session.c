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
int main(void) { UmiTeacherStudySession s; umi_teacher_study_session_init(&s,"session"); if(umi_teacher_study_session_record_activity(&s,1,80U)!=UMI_STATUS_OK) return 1; umi_teacher_study_session_record_question(&s); umi_teacher_study_session_add_minutes(&s,25U); if(umi_teacher_study_session_completion(&s)!=100U) return 2; if(!umi_teacher_study_session_ready_to_close(&s)) return 3; return 0; }
