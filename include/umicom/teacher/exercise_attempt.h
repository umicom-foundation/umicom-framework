/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/exercise_attempt.h
 *
 * PURPOSE:
 *   Track one learner exercise attempt and its score.
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
#ifndef UMICOM_TEACHER_EXERCISE_ATTEMPT_H
#define UMICOM_TEACHER_EXERCISE_ATTEMPT_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/teacher/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the teacher exercise attempt data shared with callers of this public contract.
 */
typedef struct UmiTeacherExerciseAttempt {
    char activity_id[UMI_TEACHER_ID_CAPACITY];
    uint32_t attempt_number;
    uint32_t score;
    uint32_t hints_used;
    uint32_t elapsed_seconds;
    int passed;
} UmiTeacherExerciseAttempt;

/**
 * Initialise teacher exercise attempt from caller-provided values so later operations
 * receive a known state.
 */
void umi_teacher_exercise_attempt_init(UmiTeacherExerciseAttempt *attempt);
/**
 * Provide the teacher exercise attempt begin operation used by this module and its client
 * applications.
 */
UmiStatus umi_teacher_exercise_attempt_begin(UmiTeacherExerciseAttempt *attempt, const char *activity_id, uint32_t attempt_number);
/**
 * Provide the teacher exercise attempt finish operation used by this module and its client
 * applications.
 */
UmiStatus umi_teacher_exercise_attempt_finish(UmiTeacherExerciseAttempt *attempt, uint32_t score, uint32_t hints_used, uint32_t elapsed_seconds, uint32_t pass_score);
/**
 * Provide the teacher exercise attempt effective score operation used by this module and
 * its client applications.
 */
uint32_t umi_teacher_exercise_attempt_effective_score(const UmiTeacherExerciseAttempt *attempt, uint32_t hint_penalty);

#ifdef __cplusplus
}
#endif

#endif
