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

/**
 * Represent the teacher teacher hint policy data shared with callers of this public
 * contract.
 */
typedef struct UmiTeacherTeacherHintPolicy {
    uint32_t minimum_mastery;
    uint32_t minimum_attempts;
    uint32_t maximum_failures;
    UmiTeacherLevel maximum_level;
    uint32_t language_mask;
    int enabled;
} UmiTeacherTeacherHintPolicy;

/**
 * Initialise teacher teacher hint policy from caller-provided values so later operations
 * receive a known state.
 */
void umi_teacher_teacher_hint_policy_init(UmiTeacherTeacherHintPolicy *policy);
/**
 * Provide the teacher teacher hint policy configure operation used by this module and its
 * client applications.
 */
UmiStatus umi_teacher_teacher_hint_policy_configure(UmiTeacherTeacherHintPolicy *policy, uint32_t minimum_mastery, uint32_t minimum_attempts, uint32_t maximum_failures, UmiTeacherLevel maximum_level, uint32_t language_mask);
/**
 * Provide the teacher teacher hint policy allows operation used by this module and its
 * client applications.
 */
int umi_teacher_teacher_hint_policy_allows(const UmiTeacherTeacherHintPolicy *policy, uint32_t mastery, uint32_t attempts, uint32_t failures, UmiTeacherLevel level, UmiTeacherLanguage language);
/**
 * Provide the teacher teacher hint policy deficit operation used by this module and its
 * client applications.
 */
uint32_t umi_teacher_teacher_hint_policy_deficit(const UmiTeacherTeacherHintPolicy *policy, uint32_t mastery);

#ifdef __cplusplus
}
#endif

#endif
