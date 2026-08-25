/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/exercise_scoring.h
 *
 * PURPOSE:
 *   Compute weighted exercise scores from correctness, quality and efficiency.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable learning capability. Umicom Studio, Desk and
 *   future applications are thin consumers and do not reimplement pedagogy,
 *   progression, assessment or AI Teacher orchestration.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEACHER_EXERCISE_SCORING_H
#define UMICOM_TEACHER_EXERCISE_SCORING_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/teacher/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTeacherExerciseScoring {
    uint32_t correctness_weight;
    uint32_t quality_weight;
    uint32_t efficiency_weight;
    uint32_t minimum_score;
} UmiTeacherExerciseScoring;

void umi_teacher_exercise_scoring_init(UmiTeacherExerciseScoring *rubric);
UmiStatus umi_teacher_exercise_scoring_configure(UmiTeacherExerciseScoring *rubric, uint32_t correctness_weight, uint32_t quality_weight, uint32_t efficiency_weight, uint32_t minimum_score);
uint32_t umi_teacher_exercise_scoring_compute(const UmiTeacherExerciseScoring *rubric, uint32_t correctness, uint32_t quality, uint32_t efficiency);
int umi_teacher_exercise_scoring_passes(const UmiTeacherExerciseScoring *rubric, uint32_t score);

#ifdef __cplusplus
}
#endif

#endif
