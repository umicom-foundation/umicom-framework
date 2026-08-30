/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/teacher/lesson_catalogue.c
 *
 * PURPOSE:
 *   Catalogue reusable lessons for AI Teacher and non-AI frontends.
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
#include "umicom/teacher/lesson_catalogue.h"
#include <string.h>
static void umi_teacher_lesson_catalogue_copy(char *destination, size_t capacity, const char *source) {
    size_t i = 0U;
    if (destination == NULL || capacity == 0U) {
        return;
    }
    if (source != NULL) {
        while (i + 1U < capacity && source[i] != '\0') {
            destination[i] = source[i];
            ++i;
        }
    }
    destination[i] = '\0';
}

void umi_teacher_lesson_catalogue_init(UmiTeacherLessonCatalogue *catalogue) { if (catalogue != NULL) memset(catalogue, 0, sizeof(*catalogue));
    }
const UmiTeacherLessonCatalogueEntry *umi_teacher_lesson_catalogue_find(const UmiTeacherLessonCatalogue *catalogue, const char *id) {
    size_t i;
    if (catalogue == NULL || id == NULL) return NULL;
    for (i = 0U;
    i < catalogue->count;
    ++i) if (strcmp(catalogue->entries[i].id, id) == 0) return &catalogue->entries[i];
    return NULL;
}
UmiStatus umi_teacher_lesson_catalogue_upsert(UmiTeacherLessonCatalogue *catalogue, const char *id, UmiTeacherLanguage language, UmiTeacherLevel level, uint32_t priority) {
    size_t i;
    if (catalogue == NULL || id == NULL || id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    for (i = 0U;
    i < catalogue->count;
    ++i) if (strcmp(catalogue->entries[i].id, id) == 0) { catalogue->entries[i].language=language;
    catalogue->entries[i].level=level;
    catalogue->entries[i].priority=priority;
    catalogue->entries[i].enabled=1;
    ++catalogue->revision;
    return UMI_STATUS_OK;
    }
    if (catalogue->count >= UMI_TEACHER_SMALL_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    i = catalogue->count++;
    umi_teacher_lesson_catalogue_copy(catalogue->entries[i].id, UMI_TEACHER_ID_CAPACITY, id);
    catalogue->entries[i].language=language;
    catalogue->entries[i].level=level;
    catalogue->entries[i].priority=priority;
    catalogue->entries[i].enabled=1;
    ++catalogue->revision;
    return UMI_STATUS_OK;
}
size_t umi_teacher_lesson_catalogue_eligible_count(const UmiTeacherLessonCatalogue *catalogue, UmiTeacherLanguage language, UmiTeacherLevel maximum_level) {
    size_t i, count=0U;
    if (catalogue == NULL) return 0U;
    for (i=0U;i<catalogue->count;++i) if (catalogue->entries[i].enabled && (catalogue->entries[i].language==language || catalogue->entries[i].language==UMI_TEACHER_LANGUAGE_GENERAL) && catalogue->entries[i].level<=maximum_level) ++count;
    return count;
}
