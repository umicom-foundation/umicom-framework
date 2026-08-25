/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/difficulty_model.h
 *
 * PURPOSE:
 *   Calculate normalized activity difficulty.
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
#ifndef UMICOM_TEACHER_DIFFICULTY_MODEL_H
#define UMICOM_TEACHER_DIFFICULTY_MODEL_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/teacher/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTeacherDifficultyModel {
    uint32_t correctness_weight;
    uint32_t quality_weight;
    uint32_t efficiency_weight;
    uint32_t minimum_score;
} UmiTeacherDifficultyModel;

void umi_teacher_difficulty_model_init(UmiTeacherDifficultyModel *rubric);
UmiStatus umi_teacher_difficulty_model_configure(UmiTeacherDifficultyModel *rubric, uint32_t correctness_weight, uint32_t quality_weight, uint32_t efficiency_weight, uint32_t minimum_score);
uint32_t umi_teacher_difficulty_model_compute(const UmiTeacherDifficultyModel *rubric, uint32_t correctness, uint32_t quality, uint32_t efficiency);
int umi_teacher_difficulty_model_passes(const UmiTeacherDifficultyModel *rubric, uint32_t score);

#ifdef __cplusplus
}
#endif

#endif
