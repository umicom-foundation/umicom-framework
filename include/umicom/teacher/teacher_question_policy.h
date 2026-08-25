/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/teacher_question_policy.h
 *
 * PURPOSE:
 *   Select question challenge level from current mastery.
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
#ifndef UMICOM_TEACHER_TEACHER_QUESTION_POLICY_H
#define UMICOM_TEACHER_TEACHER_QUESTION_POLICY_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/teacher/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTeacherTeacherQuestionPolicy {
    uint32_t minimum_mastery;
    uint32_t minimum_attempts;
    uint32_t maximum_failures;
    UmiTeacherLevel maximum_level;
    uint32_t language_mask;
    int enabled;
} UmiTeacherTeacherQuestionPolicy;

void umi_teacher_teacher_question_policy_init(UmiTeacherTeacherQuestionPolicy *policy);
UmiStatus umi_teacher_teacher_question_policy_configure(UmiTeacherTeacherQuestionPolicy *policy, uint32_t minimum_mastery, uint32_t minimum_attempts, uint32_t maximum_failures, UmiTeacherLevel maximum_level, uint32_t language_mask);
int umi_teacher_teacher_question_policy_allows(const UmiTeacherTeacherQuestionPolicy *policy, uint32_t mastery, uint32_t attempts, uint32_t failures, UmiTeacherLevel level, UmiTeacherLanguage language);
uint32_t umi_teacher_teacher_question_policy_deficit(const UmiTeacherTeacherQuestionPolicy *policy, uint32_t mastery);

#ifdef __cplusplus
}
#endif

#endif
