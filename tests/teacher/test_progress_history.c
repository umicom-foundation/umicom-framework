/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/teacher/test_progress_history.c
 *
 * PURPOSE:
 *   Implement the test progress history behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/teacher/progress_history.h"
int main(void) { UmiTeacherProgressHistory h; const UmiTeacherProgressHistoryEntry *e; umi_teacher_progress_history_init(&h); umi_teacher_progress_history_record(&h,1U,70U,50U,80U); umi_teacher_progress_history_record(&h,2U,80U,70U,90U); e=umi_teacher_progress_history_latest(&h); if(e==0||e->revision!=2U||e->mastery!=80U) return 1; return 0; }
