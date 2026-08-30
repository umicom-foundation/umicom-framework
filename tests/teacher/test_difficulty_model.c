/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/teacher/test_difficulty_model.c
 *
 * PURPOSE:
 *   Implement the test difficulty model behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/teacher/difficulty_model.h"
int main(void) { UmiTeacherDifficultyModel r; uint32_t score; umi_teacher_difficulty_model_init(&r); score=umi_teacher_difficulty_model_compute(&r,100U,80U,60U); if(score!=89U) return 1; if(!umi_teacher_difficulty_model_passes(&r,score)) return 2; if(umi_teacher_difficulty_model_configure(&r,50U,20U,20U,70U)!=UMI_STATUS_INVALID_ARGUMENT) return 3; return 0; }
