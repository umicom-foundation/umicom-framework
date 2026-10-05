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
#include "umicom/base/value_archive.h"
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/teacher/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the teacher compiler exercise data shared with callers of this public
 * contract.
 */
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

/**
 * Initialise teacher compiler exercise from caller-provided values so later operations
 * receive a known state.
 */
void umi_teacher_compiler_exercise_init(UmiTeacherCompilerExercise *value);
/**
 * Provide the teacher compiler exercise configure operation used by this module and its
 * client applications.
 */
UmiStatus umi_teacher_compiler_exercise_configure(UmiTeacherCompilerExercise *value, const char *id, const char *title, UmiTeacherLanguage language, UmiTeacherLevel level, uint32_t weight, uint32_t required_score);
/**
 * Check that teacher compiler exercise satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_teacher_compiler_exercise_validate(const UmiTeacherCompilerExercise *value);
/**
 * Provide the teacher compiler exercise priority operation used by this module and its
 * client applications.
 */
uint32_t umi_teacher_compiler_exercise_priority(const UmiTeacherCompilerExercise *value, uint32_t relevance);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_teacher_compiler_exercise_archive_encode(const UmiTeacherCompilerExercise *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_teacher_compiler_exercise_archive_decode(const void *bytes, size_t byte_count,
    UmiTeacherCompilerExercise *value);

#ifdef __cplusplus
}
#endif

#endif
