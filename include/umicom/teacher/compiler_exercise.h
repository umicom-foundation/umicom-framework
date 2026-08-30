/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/compiler_exercise.h
 *
 * PURPOSE:
 *   Describe compiler-backed coding exercises and expected compilation outcomes.
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
#ifndef UMICOM_TEACHER_COMPILER_EXERCISE_H
#define UMICOM_TEACHER_COMPILER_EXERCISE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/teacher/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTeacherCompilerExercise {
    char id[UMI_TEACHER_ID_CAPACITY];
    char title[UMI_TEACHER_TEXT_CAPACITY];
    UmiTeacherLanguage language;
    UmiTeacherLevel level;
    uint32_t weight;
    uint32_t required_score;
    uint64_t revision;
    int enabled;
} UmiTeacherCompilerExercise;

void umi_teacher_compiler_exercise_init(UmiTeacherCompilerExercise *value);
UmiStatus umi_teacher_compiler_exercise_configure(UmiTeacherCompilerExercise *value, const char *id, const char *title, UmiTeacherLanguage language, UmiTeacherLevel level, uint32_t weight, uint32_t required_score);
UmiStatus umi_teacher_compiler_exercise_validate(const UmiTeacherCompilerExercise *value);
uint32_t umi_teacher_compiler_exercise_priority(const UmiTeacherCompilerExercise *value, uint32_t relevance);

#ifdef __cplusplus
}
#endif

#endif
