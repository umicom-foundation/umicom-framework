/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/assessment_rubric.h
 *
 * PURPOSE:
 *   Compute rubric-weighted assessment outcomes.
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
#ifndef UMICOM_TEACHER_ASSESSMENT_RUBRIC_H
#define UMICOM_TEACHER_ASSESSMENT_RUBRIC_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/teacher/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTeacherAssessmentRubric {
    uint32_t correctness_weight;
    uint32_t quality_weight;
    uint32_t efficiency_weight;
    uint32_t minimum_score;
} UmiTeacherAssessmentRubric;

void umi_teacher_assessment_rubric_init(UmiTeacherAssessmentRubric *rubric);
UmiStatus umi_teacher_assessment_rubric_configure(UmiTeacherAssessmentRubric *rubric, uint32_t correctness_weight, uint32_t quality_weight, uint32_t efficiency_weight, uint32_t minimum_score);
uint32_t umi_teacher_assessment_rubric_compute(const UmiTeacherAssessmentRubric *rubric, uint32_t correctness, uint32_t quality, uint32_t efficiency);
int umi_teacher_assessment_rubric_passes(const UmiTeacherAssessmentRubric *rubric, uint32_t score);

#ifdef __cplusplus
}
#endif

#endif
