/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/exercise_descriptor.h
 *
 * PURPOSE:
 *   Describe one coding, reasoning or knowledge exercise.
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
#ifndef UMICOM_TEACHER_EXERCISE_DESCRIPTOR_H
#define UMICOM_TEACHER_EXERCISE_DESCRIPTOR_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/teacher/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the teacher exercise descriptor data shared with callers of this public
 * contract.
 */
typedef struct UmiTeacherExerciseDescriptor {
    char id[UMI_TEACHER_ID_CAPACITY];
    char title[UMI_TEACHER_TEXT_CAPACITY];
    UmiTeacherLanguage language;
    UmiTeacherLevel level;
    uint32_t weight;
    uint32_t required_score;
    uint64_t revision;
    int enabled;
} UmiTeacherExerciseDescriptor;

/**
 * Initialise teacher exercise descriptor from caller-provided values so later operations
 * receive a known state.
 */
void umi_teacher_exercise_descriptor_init(UmiTeacherExerciseDescriptor *value);
/**
 * Provide the teacher exercise descriptor configure operation used by this module and its
 * client applications.
 */
UmiStatus umi_teacher_exercise_descriptor_configure(UmiTeacherExerciseDescriptor *value, const char *id, const char *title, UmiTeacherLanguage language, UmiTeacherLevel level, uint32_t weight, uint32_t required_score);
/**
 * Check that teacher exercise descriptor satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_teacher_exercise_descriptor_validate(const UmiTeacherExerciseDescriptor *value);
/**
 * Provide the teacher exercise descriptor priority operation used by this module and its
 * client applications.
 */
uint32_t umi_teacher_exercise_descriptor_priority(const UmiTeacherExerciseDescriptor *value, uint32_t relevance);

#ifdef __cplusplus
}
#endif

#endif
