/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/code_quality_rubric.h
 *
 * PURPOSE:
 *   Score code clarity, safety, structure and maintainability.
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
#ifndef UMICOM_TEACHER_CODE_QUALITY_RUBRIC_H
#define UMICOM_TEACHER_CODE_QUALITY_RUBRIC_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/teacher/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the teacher code quality rubric data shared with callers of this public
 * contract.
 */
typedef struct UmiTeacherCodeQualityRubric {
    uint32_t correctness_weight;
    uint32_t quality_weight;
    uint32_t efficiency_weight;
    uint32_t minimum_score;
} UmiTeacherCodeQualityRubric;

/**
 * Initialise teacher code quality rubric from caller-provided values so later operations
 * receive a known state.
 */
void umi_teacher_code_quality_rubric_init(UmiTeacherCodeQualityRubric *rubric);
/**
 * Provide the teacher code quality rubric configure operation used by this module and its
 * client applications.
 */
UmiStatus umi_teacher_code_quality_rubric_configure(UmiTeacherCodeQualityRubric *rubric, uint32_t correctness_weight, uint32_t quality_weight, uint32_t efficiency_weight, uint32_t minimum_score);
/**
 * Provide the teacher code quality rubric compute operation used by this module and its
 * client applications.
 */
uint32_t umi_teacher_code_quality_rubric_compute(const UmiTeacherCodeQualityRubric *rubric, uint32_t correctness, uint32_t quality, uint32_t efficiency);
/**
 * Provide the teacher code quality rubric passes operation used by this module and its
 * client applications.
 */
int umi_teacher_code_quality_rubric_passes(const UmiTeacherCodeQualityRubric *rubric, uint32_t score);

#ifdef __cplusplus
}
#endif

#endif
