/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/teacher/test_teacher_context.c
 *
 * PURPOSE:
 *   Implement the test teacher context behavior for
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
#include "umicom/teacher/teacher_context.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiTeacherTeacherContext q; umi_teacher_teacher_context_init(&q); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_teacher_teacher_context_append(&q,"a")!=UMI_STATUS_OK) return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_teacher_teacher_context_append(&q,"b")!=UMI_STATUS_OK) return 2; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(strcmp(umi_teacher_teacher_context_current(&q),"a")!=0) return 3; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_teacher_teacher_context_next(&q)!=UMI_STATUS_OK || strcmp(umi_teacher_teacher_context_current(&q),"b")!=0) return 4; return 0; }
