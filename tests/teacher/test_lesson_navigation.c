/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/teacher/test_lesson_navigation.c
 *
 * PURPOSE:
 *   Implement the test lesson navigation behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <string.h>
#include "umicom/teacher/lesson_navigation.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiTeacherLessonNavigation q; umi_teacher_lesson_navigation_init(&q); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_teacher_lesson_navigation_append(&q,"a")!=UMI_STATUS_OK) return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_teacher_lesson_navigation_append(&q,"b")!=UMI_STATUS_OK) return 2; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(strcmp(umi_teacher_lesson_navigation_current(&q),"a")!=0) return 3; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_teacher_lesson_navigation_next(&q)!=UMI_STATUS_OK || strcmp(umi_teacher_lesson_navigation_current(&q),"b")!=0) return 4; return 0; }
