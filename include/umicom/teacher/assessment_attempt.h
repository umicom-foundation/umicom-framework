/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/assessment_attempt.h
 *
 * PURPOSE:
 *   Track an assessment attempt and pass/fail evidence.
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
#ifndef UMICOM_TEACHER_ASSESSMENT_ATTEMPT_H
#define UMICOM_TEACHER_ASSESSMENT_ATTEMPT_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/teacher/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the teacher assessment attempt data shared with callers of this public
 * contract.
 */
typedef struct UmiTeacherAssessmentAttempt {
    char activity_id[UMI_TEACHER_ID_CAPACITY];
    uint32_t attempt_number;
    uint32_t score;
    uint32_t hints_used;
    uint32_t elapsed_seconds;
    int passed;
} UmiTeacherAssessmentAttempt;

/**
 * Initialise teacher assessment attempt from caller-provided values so later operations
 * receive a known state.
 */
void umi_teacher_assessment_attempt_init(UmiTeacherAssessmentAttempt *attempt);
/**
 * Provide the teacher assessment attempt begin operation used by this module and its
 * client applications.
 */
UmiStatus umi_teacher_assessment_attempt_begin(UmiTeacherAssessmentAttempt *attempt, const char *activity_id, uint32_t attempt_number);
/**
 * Provide the teacher assessment attempt finish operation used by this module and its
 * client applications.
 */
UmiStatus umi_teacher_assessment_attempt_finish(UmiTeacherAssessmentAttempt *attempt, uint32_t score, uint32_t hints_used, uint32_t elapsed_seconds, uint32_t pass_score);
/**
 * Provide the teacher assessment attempt effective score operation used by this module and
 * its client applications.
 */
uint32_t umi_teacher_assessment_attempt_effective_score(const UmiTeacherAssessmentAttempt *attempt, uint32_t hint_penalty);

#ifdef __cplusplus
}
#endif

#endif
