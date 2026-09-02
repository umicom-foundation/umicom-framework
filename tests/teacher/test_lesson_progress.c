/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/teacher/test_lesson_progress.c
 *
 * PURPOSE:
 *   Implement the test lesson progress behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/teacher/lesson_progress.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiTeacherLessonProgress p; umi_teacher_lesson_progress_init(&p); umi_teacher_lesson_progress_update(&p,3U,3U,2U,2U,80U); /* Apply this operation only while the related capability or state is available. */ if(umi_teacher_lesson_progress_completion(&p)!=100U) return 1; /* Apply this operation only while the related capability or state is available. */ if(!umi_teacher_lesson_progress_ready_for_assessment(&p,75U)) return 2; return 0; }
