/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/exercise_solution_policy.h
 *
 * PURPOSE:
 *   Control when full solutions may be revealed.
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
#ifndef UMICOM_TEACHER_EXERCISE_SOLUTION_POLICY_H
#define UMICOM_TEACHER_EXERCISE_SOLUTION_POLICY_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/teacher/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the teacher exercise solution policy data shared with callers of this public
 * contract.
 */
typedef struct UmiTeacherExerciseSolutionPolicy {
    uint32_t minimum_mastery;
    uint32_t minimum_attempts;
    uint32_t maximum_failures;
    UmiTeacherLevel maximum_level;
    uint32_t language_mask;
    int enabled;
} UmiTeacherExerciseSolutionPolicy;

/**
 * Initialise teacher exercise solution policy from caller-provided values so later
 * operations receive a known state.
 */
void umi_teacher_exercise_solution_policy_init(UmiTeacherExerciseSolutionPolicy *policy);
/**
 * Provide the teacher exercise solution policy configure operation used by this module and
 * its client applications.
 */
UmiStatus umi_teacher_exercise_solution_policy_configure(UmiTeacherExerciseSolutionPolicy *policy, uint32_t minimum_mastery, uint32_t minimum_attempts, uint32_t maximum_failures, UmiTeacherLevel maximum_level, uint32_t language_mask);
/**
 * Provide the teacher exercise solution policy allows operation used by this module and
 * its client applications.
 */
int umi_teacher_exercise_solution_policy_allows(const UmiTeacherExerciseSolutionPolicy *policy, uint32_t mastery, uint32_t attempts, uint32_t failures, UmiTeacherLevel level, UmiTeacherLanguage language);
/**
 * Provide the teacher exercise solution policy deficit operation used by this module and
 * its client applications.
 */
uint32_t umi_teacher_exercise_solution_policy_deficit(const UmiTeacherExerciseSolutionPolicy *policy, uint32_t mastery);

#ifdef __cplusplus
}
#endif

#endif
