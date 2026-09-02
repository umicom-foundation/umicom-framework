/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/exercise_hint.h
 *
 * PURPOSE:
 *   Describe a bounded hint with escalation level and pedagogical cost.
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
#ifndef UMICOM_TEACHER_EXERCISE_HINT_H
#define UMICOM_TEACHER_EXERCISE_HINT_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/teacher/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the teacher exercise hint data shared with callers of this public contract.
 */
typedef struct UmiTeacherExerciseHint {
    char id[UMI_TEACHER_ID_CAPACITY];
    char title[UMI_TEACHER_TEXT_CAPACITY];
    UmiTeacherLanguage language;
    UmiTeacherLevel level;
    uint32_t weight;
    uint32_t required_score;
    uint64_t revision;
    int enabled;
} UmiTeacherExerciseHint;

/**
 * Initialise teacher exercise hint from caller-provided values so later operations receive
 * a known state.
 */
void umi_teacher_exercise_hint_init(UmiTeacherExerciseHint *value);
/**
 * Provide the teacher exercise hint configure operation used by this module and its client
 * applications.
 */
UmiStatus umi_teacher_exercise_hint_configure(UmiTeacherExerciseHint *value, const char *id, const char *title, UmiTeacherLanguage language, UmiTeacherLevel level, uint32_t weight, uint32_t required_score);
/**
 * Check that teacher exercise hint satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_teacher_exercise_hint_validate(const UmiTeacherExerciseHint *value);
/**
 * Provide the teacher exercise hint priority operation used by this module and its client
 * applications.
 */
uint32_t umi_teacher_exercise_hint_priority(const UmiTeacherExerciseHint *value, uint32_t relevance);

#ifdef __cplusplus
}
#endif

#endif
