/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/teacher/test_progress_snapshot.c
 *
 * PURPOSE:
 *   Implement the test progress snapshot behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/teacher/progress_snapshot.h"
int main(void) { UmiTeacherProgressSnapshot s; umi_teacher_progress_snapshot_init(&s); if (umi_teacher_progress_snapshot_record(&s,82U,1)!=UMI_STATUS_OK) return 1; umi_teacher_progress_snapshot_set_units(&s,4U,4U); if (umi_teacher_progress_snapshot_completion(&s)!=100U) return 2; if (!umi_teacher_progress_snapshot_complete(&s)) return 3; return 0; }
