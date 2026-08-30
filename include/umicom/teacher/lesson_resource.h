/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/lesson_resource.h
 *
 * PURPOSE:
 *   Describe a resource linked to a lesson without tying the model to a UI toolkit.
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
#ifndef UMICOM_TEACHER_LESSON_RESOURCE_H
#define UMICOM_TEACHER_LESSON_RESOURCE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/teacher/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTeacherLessonResource {
    char id[UMI_TEACHER_ID_CAPACITY];
    char title[UMI_TEACHER_TEXT_CAPACITY];
    UmiTeacherLanguage language;
    UmiTeacherLevel level;
    uint32_t weight;
    uint32_t required_score;
    uint64_t revision;
    int enabled;
} UmiTeacherLessonResource;

void umi_teacher_lesson_resource_init(UmiTeacherLessonResource *value);
UmiStatus umi_teacher_lesson_resource_configure(UmiTeacherLessonResource *value, const char *id, const char *title, UmiTeacherLanguage language, UmiTeacherLevel level, uint32_t weight, uint32_t required_score);
UmiStatus umi_teacher_lesson_resource_validate(const UmiTeacherLessonResource *value);
uint32_t umi_teacher_lesson_resource_priority(const UmiTeacherLessonResource *value, uint32_t relevance);

#ifdef __cplusplus
}
#endif

#endif
