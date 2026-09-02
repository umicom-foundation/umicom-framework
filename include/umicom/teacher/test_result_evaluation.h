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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * Represent the teacher test result evaluation data shared with callers of this public
 * contract.
 */
typedef struct UmiTeacherTestResultEvaluation {
    uint32_t correctness_weight;
    uint32_t quality_weight;
    uint32_t efficiency_weight;
    uint32_t minimum_score;
} UmiTeacherTestResultEvaluation;

/**
 * Initialise teacher test result evaluation from caller-provided values so later
 * operations receive a known state.
 */
void umi_teacher_test_result_evaluation_init(UmiTeacherTestResultEvaluation *rubric);
/**
 * Provide the teacher test result evaluation configure operation used by this module and
 * its client applications.
 */
UmiStatus umi_teacher_test_result_evaluation_configure(UmiTeacherTestResultEvaluation *rubric, uint32_t correctness_weight, uint32_t quality_weight, uint32_t efficiency_weight, uint32_t minimum_score);
/**
 * Provide the teacher test result evaluation compute operation used by this module and its
 * client applications.
 */
uint32_t umi_teacher_test_result_evaluation_compute(const UmiTeacherTestResultEvaluation *rubric, uint32_t correctness, uint32_t quality, uint32_t efficiency);
/**
 * Provide the teacher test result evaluation passes operation used by this module and its
 * client applications.
 */
int umi_teacher_test_result_evaluation_passes(const UmiTeacherTestResultEvaluation *rubric, uint32_t score);

#ifdef __cplusplus
}
#endif

#endif
