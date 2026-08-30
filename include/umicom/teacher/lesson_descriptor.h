/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/lesson_descriptor.h
 *
 * PURPOSE:
 *   Describe a lesson and its required language, level and mastery threshold.
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
#ifndef UMICOM_TEACHER_LESSON_DESCRIPTOR_H
#define UMICOM_TEACHER_LESSON_DESCRIPTOR_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/teacher/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTeacherLessonDescriptor {
    char id[UMI_TEACHER_ID_CAPACITY];
    char title[UMI_TEACHER_TEXT_CAPACITY];
    UmiTeacherLanguage language;
    UmiTeacherLevel level;
    uint32_t weight;
    uint32_t required_score;
    uint64_t revision;
    int enabled;
} UmiTeacherLessonDescriptor;

void umi_teacher_lesson_descriptor_init(UmiTeacherLessonDescriptor *value);
UmiStatus umi_teacher_lesson_descriptor_configure(UmiTeacherLessonDescriptor *value, const char *id, const char *title, UmiTeacherLanguage language, UmiTeacherLevel level, uint32_t weight, uint32_t required_score);
UmiStatus umi_teacher_lesson_descriptor_validate(const UmiTeacherLessonDescriptor *value);
uint32_t umi_teacher_lesson_descriptor_priority(const UmiTeacherLessonDescriptor *value, uint32_t relevance);

#ifdef __cplusplus
}
#endif

#endif
