/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/lesson_progress.h
 *
 * PURPOSE:
 *   Track lesson progress, attempts and completion evidence.
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
#ifndef UMICOM_TEACHER_LESSON_PROGRESS_H
#define UMICOM_TEACHER_LESSON_PROGRESS_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/teacher/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the teacher lesson progress data shared with callers of this public contract.
 */
typedef struct UmiTeacherLessonProgress { uint32_t sections_completed; uint32_t section_count; uint32_t exercises_passed; uint32_t exercise_count; uint32_t mastery; uint64_t revision; } UmiTeacherLessonProgress;
/**
 * Initialise teacher lesson progress from caller-provided values so later operations
 * receive a known state.
 */
void umi_teacher_lesson_progress_init(UmiTeacherLessonProgress *progress);
/**
 * Provide the teacher lesson progress update operation used by this module and its client
 * applications.
 */
void umi_teacher_lesson_progress_update(UmiTeacherLessonProgress *progress, uint32_t sections_completed, uint32_t section_count, uint32_t exercises_passed, uint32_t exercise_count, uint32_t mastery);
/**
 * Provide the teacher lesson progress completion operation used by this module and its
 * client applications.
 */
uint32_t umi_teacher_lesson_progress_completion(const UmiTeacherLessonProgress *progress);
/**
 * Provide the teacher lesson progress ready for assessment operation used by this module
 * and its client applications.
 */
int umi_teacher_lesson_progress_ready_for_assessment(const UmiTeacherLessonProgress *progress, uint32_t required_mastery);

#ifdef __cplusplus
}
#endif

#endif
