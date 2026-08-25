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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiTeacherLessonNavigation {
    char ids[UMI_TEACHER_MEDIUM_CAPACITY][UMI_TEACHER_ID_CAPACITY];
    size_t count;
    size_t cursor;
    uint64_t revision;
} UmiTeacherLessonNavigation;

void umi_teacher_lesson_navigation_init(UmiTeacherLessonNavigation *sequence);
UmiStatus umi_teacher_lesson_navigation_append(UmiTeacherLessonNavigation *sequence, const char *id);
const char *umi_teacher_lesson_navigation_current(const UmiTeacherLessonNavigation *sequence);
UmiStatus umi_teacher_lesson_navigation_next(UmiTeacherLessonNavigation *sequence);
UmiStatus umi_teacher_lesson_navigation_previous(UmiTeacherLessonNavigation *sequence);
size_t umi_teacher_lesson_navigation_count(const UmiTeacherLessonNavigation *sequence);

#ifdef __cplusplus
}
#endif

#endif
