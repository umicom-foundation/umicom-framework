/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/lesson_navigation.h
 *
 * PURPOSE:
 *   Track deterministic next/previous lesson navigation order.
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
#ifndef UMICOM_TEACHER_LESSON_NAVIGATION_H
#define UMICOM_TEACHER_LESSON_NAVIGATION_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/teacher/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the teacher lesson navigation data shared with callers of this public
 * contract.
 */
typedef struct UmiTeacherLessonNavigation {
    char ids[UMI_TEACHER_MEDIUM_CAPACITY][UMI_TEACHER_ID_CAPACITY];
    size_t count;
    size_t cursor;
    uint64_t revision;
} UmiTeacherLessonNavigation;

/**
 * Initialise teacher lesson navigation from caller-provided values so later operations
 * receive a known state.
 */
void umi_teacher_lesson_navigation_init(UmiTeacherLessonNavigation *sequence);
/**
 * Add teacher lesson navigation only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_teacher_lesson_navigation_append(UmiTeacherLessonNavigation *sequence, const char *id);
/**
 * Provide the teacher lesson navigation current operation used by this module and its
 * client applications.
 */
const char *umi_teacher_lesson_navigation_current(const UmiTeacherLessonNavigation *sequence);
/**
 * Provide the teacher lesson navigation next operation used by this module and its client
 * applications.
 */
UmiStatus umi_teacher_lesson_navigation_next(UmiTeacherLessonNavigation *sequence);
/**
 * Provide the teacher lesson navigation previous operation used by this module and its
 * client applications.
 */
UmiStatus umi_teacher_lesson_navigation_previous(UmiTeacherLessonNavigation *sequence);
/**
 * Return the number of records represented by teacher lesson navigation without changing
 * their state.
 */
size_t umi_teacher_lesson_navigation_count(const UmiTeacherLessonNavigation *sequence);

#ifdef __cplusplus
}
#endif

#endif
