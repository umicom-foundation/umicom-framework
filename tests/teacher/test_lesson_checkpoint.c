/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/teacher/test_lesson_checkpoint.c
 *
 * PURPOSE:
 *   Implement the test lesson checkpoint behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/teacher/lesson_checkpoint.h"
int main(void) { UmiTeacherLessonCheckpoint s; umi_teacher_lesson_checkpoint_init(&s); if (umi_teacher_lesson_checkpoint_record(&s,82U,1)!=UMI_STATUS_OK) return 1; umi_teacher_lesson_checkpoint_set_units(&s,4U,4U); if (umi_teacher_lesson_checkpoint_completion(&s)!=100U) return 2; if (!umi_teacher_lesson_checkpoint_complete(&s)) return 3; return 0; }
