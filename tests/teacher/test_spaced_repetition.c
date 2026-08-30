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
int main(void) { UmiTeacherSpacedRepetition s; umi_teacher_spaced_repetition_init(&s); if(umi_teacher_spaced_repetition_record(&s,5U)!=UMI_STATUS_OK) return 1; if(umi_teacher_spaced_repetition_record(&s,5U)!=UMI_STATUS_OK) return 2; if(umi_teacher_spaced_repetition_next_interval(&s)!=6U) return 3; return 0; }
