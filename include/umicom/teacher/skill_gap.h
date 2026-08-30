/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/skill_gap.h
 *
 * PURPOSE:
 *   Calculate the gap between current and required mastery.
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
#ifndef UMICOM_TEACHER_SKILL_GAP_H
#define UMICOM_TEACHER_SKILL_GAP_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/teacher/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTeacherSkillGap {
    uint32_t correctness_weight;
    uint32_t quality_weight;
    uint32_t efficiency_weight;
    uint32_t minimum_score;
} UmiTeacherSkillGap;

void umi_teacher_skill_gap_init(UmiTeacherSkillGap *rubric);
UmiStatus umi_teacher_skill_gap_configure(UmiTeacherSkillGap *rubric, uint32_t correctness_weight, uint32_t quality_weight, uint32_t efficiency_weight, uint32_t minimum_score);
uint32_t umi_teacher_skill_gap_compute(const UmiTeacherSkillGap *rubric, uint32_t correctness, uint32_t quality, uint32_t efficiency);
int umi_teacher_skill_gap_passes(const UmiTeacherSkillGap *rubric, uint32_t score);

#ifdef __cplusplus
}
#endif

#endif
