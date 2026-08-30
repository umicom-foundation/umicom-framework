/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/teacher_explanation_policy.h
 *
 * PURPOSE:
 *   Control explanation depth and technical level.
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
#ifndef UMICOM_TEACHER_TEACHER_EXPLANATION_POLICY_H
#define UMICOM_TEACHER_TEACHER_EXPLANATION_POLICY_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/teacher/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTeacherTeacherExplanationPolicy {
    uint32_t minimum_mastery;
    uint32_t minimum_attempts;
    uint32_t maximum_failures;
    UmiTeacherLevel maximum_level;
    uint32_t language_mask;
    int enabled;
} UmiTeacherTeacherExplanationPolicy;

void umi_teacher_teacher_explanation_policy_init(UmiTeacherTeacherExplanationPolicy *policy);
UmiStatus umi_teacher_teacher_explanation_policy_configure(UmiTeacherTeacherExplanationPolicy *policy, uint32_t minimum_mastery, uint32_t minimum_attempts, uint32_t maximum_failures, UmiTeacherLevel maximum_level, uint32_t language_mask);
int umi_teacher_teacher_explanation_policy_allows(const UmiTeacherTeacherExplanationPolicy *policy, uint32_t mastery, uint32_t attempts, uint32_t failures, UmiTeacherLevel level, UmiTeacherLanguage language);
uint32_t umi_teacher_teacher_explanation_policy_deficit(const UmiTeacherTeacherExplanationPolicy *policy, uint32_t mastery);

#ifdef __cplusplus
}
#endif

#endif
