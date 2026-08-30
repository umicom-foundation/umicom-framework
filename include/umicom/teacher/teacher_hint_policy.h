/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/teacher_hint_policy.h
 *
 * PURPOSE:
 *   Control hint depth according to attempts, mastery and solution policy.
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
#ifndef UMICOM_TEACHER_TEACHER_HINT_POLICY_H
#define UMICOM_TEACHER_TEACHER_HINT_POLICY_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/teacher/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTeacherTeacherHintPolicy {
    uint32_t minimum_mastery;
    uint32_t minimum_attempts;
    uint32_t maximum_failures;
    UmiTeacherLevel maximum_level;
    uint32_t language_mask;
    int enabled;
} UmiTeacherTeacherHintPolicy;

void umi_teacher_teacher_hint_policy_init(UmiTeacherTeacherHintPolicy *policy);
UmiStatus umi_teacher_teacher_hint_policy_configure(UmiTeacherTeacherHintPolicy *policy, uint32_t minimum_mastery, uint32_t minimum_attempts, uint32_t maximum_failures, UmiTeacherLevel maximum_level, uint32_t language_mask);
int umi_teacher_teacher_hint_policy_allows(const UmiTeacherTeacherHintPolicy *policy, uint32_t mastery, uint32_t attempts, uint32_t failures, UmiTeacherLevel level, UmiTeacherLanguage language);
uint32_t umi_teacher_teacher_hint_policy_deficit(const UmiTeacherTeacherHintPolicy *policy, uint32_t mastery);

#ifdef __cplusplus
}
#endif

#endif
