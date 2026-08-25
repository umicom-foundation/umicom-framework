/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/lesson_catalogue.h
 *
 * PURPOSE:
 *   Catalogue reusable lessons for AI Teacher and non-AI frontends.
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
#ifndef UMICOM_TEACHER_LESSON_CATALOGUE_H
#define UMICOM_TEACHER_LESSON_CATALOGUE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/teacher/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTeacherLessonCatalogueEntry {
    char id[UMI_TEACHER_ID_CAPACITY];
    UmiTeacherLanguage language;
    UmiTeacherLevel level;
    uint32_t priority;
    int enabled;
} UmiTeacherLessonCatalogueEntry;

typedef struct UmiTeacherLessonCatalogue {
    UmiTeacherLessonCatalogueEntry entries[UMI_TEACHER_SMALL_CAPACITY];
    size_t count;
    uint64_t revision;
} UmiTeacherLessonCatalogue;

void umi_teacher_lesson_catalogue_init(UmiTeacherLessonCatalogue *catalogue);
UmiStatus umi_teacher_lesson_catalogue_upsert(UmiTeacherLessonCatalogue *catalogue, const char *id, UmiTeacherLanguage language, UmiTeacherLevel level, uint32_t priority);
const UmiTeacherLessonCatalogueEntry *umi_teacher_lesson_catalogue_find(const UmiTeacherLessonCatalogue *catalogue, const char *id);
size_t umi_teacher_lesson_catalogue_eligible_count(const UmiTeacherLessonCatalogue *catalogue, UmiTeacherLanguage language, UmiTeacherLevel maximum_level);

#ifdef __cplusplus
}
#endif

#endif
