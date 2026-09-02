/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/teacher_review.h
 *
 * PURPOSE:
 *   Score a learner submission review across correctness and engineering quality.
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
#ifndef UMICOM_TEACHER_TEACHER_REVIEW_H
#define UMICOM_TEACHER_TEACHER_REVIEW_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/teacher/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the teacher teacher review data shared with callers of this public contract.
 */
typedef struct UmiTeacherTeacherReview {
    uint32_t correctness_weight;
    uint32_t quality_weight;
    uint32_t efficiency_weight;
    uint32_t minimum_score;
} UmiTeacherTeacherReview;

/**
 * Initialise teacher teacher review from caller-provided values so later operations
 * receive a known state.
 */
void umi_teacher_teacher_review_init(UmiTeacherTeacherReview *rubric);
/**
 * Provide the teacher teacher review configure operation used by this module and its
 * client applications.
 */
UmiStatus umi_teacher_teacher_review_configure(UmiTeacherTeacherReview *rubric, uint32_t correctness_weight, uint32_t quality_weight, uint32_t efficiency_weight, uint32_t minimum_score);
/**
 * Provide the teacher teacher review compute operation used by this module and its client
 * applications.
 */
uint32_t umi_teacher_teacher_review_compute(const UmiTeacherTeacherReview *rubric, uint32_t correctness, uint32_t quality, uint32_t efficiency);
/**
 * Provide the teacher teacher review passes operation used by this module and its client
 * applications.
 */
int umi_teacher_teacher_review_passes(const UmiTeacherTeacherReview *rubric, uint32_t score);

#ifdef __cplusplus
}
#endif

#endif
