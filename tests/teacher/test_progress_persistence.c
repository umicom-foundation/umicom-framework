/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/teacher/test_progress_persistence.c
 *
 * PURPOSE:
 *   Implement the test progress persistence behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/teacher/progress_persistence.h"
int main(void) { UmiTeacherProgressPersistence s; umi_teacher_progress_persistence_init(&s); if (umi_teacher_progress_persistence_record(&s,82U,1)!=UMI_STATUS_OK) return 1; umi_teacher_progress_persistence_set_units(&s,4U,4U); if (umi_teacher_progress_persistence_completion(&s)!=100U) return 2; if (!umi_teacher_progress_persistence_complete(&s)) return 3; return 0; }
