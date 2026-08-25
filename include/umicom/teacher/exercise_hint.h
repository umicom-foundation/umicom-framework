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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

void umi_teacher_exercise_hint_init(UmiTeacherExerciseHint *value);
UmiStatus umi_teacher_exercise_hint_configure(UmiTeacherExerciseHint *value, const char *id, const char *title, UmiTeacherLanguage language, UmiTeacherLevel level, uint32_t weight, uint32_t required_score);
UmiStatus umi_teacher_exercise_hint_validate(const UmiTeacherExerciseHint *value);
uint32_t umi_teacher_exercise_hint_priority(const UmiTeacherExerciseHint *value, uint32_t relevance);

#ifdef __cplusplus
}
#endif

#endif
