/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/teacher/test_spaced_repetition.c
 *
 * PURPOSE:
 *   Implement the test spaced repetition behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/teacher/spaced_repetition.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiTeacherSpacedRepetition s; umi_teacher_spaced_repetition_init(&s); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_teacher_spaced_repetition_record(&s,5U)!=UMI_STATUS_OK) return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_teacher_spaced_repetition_record(&s,5U)!=UMI_STATUS_OK) return 2; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_teacher_spaced_repetition_next_interval(&s)!=6U) return 3; return 0; }
