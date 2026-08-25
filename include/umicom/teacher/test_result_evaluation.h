/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/test_result_evaluation.h
 *
 * PURPOSE:
 *   Score test execution evidence for an exercise.
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
#ifndef UMICOM_TEACHER_TEST_RESULT_EVALUATION_H
#define UMICOM_TEACHER_TEST_RESULT_EVALUATION_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/teacher/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTeacherTestResultEvaluation {
    uint32_t correctness_weight;
    uint32_t quality_weight;
    uint32_t efficiency_weight;
    uint32_t minimum_score;
} UmiTeacherTestResultEvaluation;

void umi_teacher_test_result_evaluation_init(UmiTeacherTestResultEvaluation *rubric);
UmiStatus umi_teacher_test_result_evaluation_configure(UmiTeacherTestResultEvaluation *rubric, uint32_t correctness_weight, uint32_t quality_weight, uint32_t efficiency_weight, uint32_t minimum_score);
uint32_t umi_teacher_test_result_evaluation_compute(const UmiTeacherTestResultEvaluation *rubric, uint32_t correctness, uint32_t quality, uint32_t efficiency);
int umi_teacher_test_result_evaluation_passes(const UmiTeacherTestResultEvaluation *rubric, uint32_t score);

#ifdef __cplusplus
}
#endif

#endif
