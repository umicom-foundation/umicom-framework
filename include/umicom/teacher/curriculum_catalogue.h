/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/curriculum_catalogue.h
 *
 * PURPOSE:
 *   Maintain the bounded catalogue of Framework-owned curriculum tracks.
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
#ifndef UMICOM_TEACHER_CURRICULUM_CATALOGUE_H
#define UMICOM_TEACHER_CURRICULUM_CATALOGUE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/teacher/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTeacherCurriculumCatalogueEntry {
    char id[UMI_TEACHER_ID_CAPACITY];
    UmiTeacherLanguage language;
    UmiTeacherLevel level;
    uint32_t priority;
    int enabled;
} UmiTeacherCurriculumCatalogueEntry;

typedef struct UmiTeacherCurriculumCatalogue {
    UmiTeacherCurriculumCatalogueEntry entries[UMI_TEACHER_SMALL_CAPACITY];
    size_t count;
    uint64_t revision;
} UmiTeacherCurriculumCatalogue;

void umi_teacher_curriculum_catalogue_init(UmiTeacherCurriculumCatalogue *catalogue);
UmiStatus umi_teacher_curriculum_catalogue_upsert(UmiTeacherCurriculumCatalogue *catalogue, const char *id, UmiTeacherLanguage language, UmiTeacherLevel level, uint32_t priority);
const UmiTeacherCurriculumCatalogueEntry *umi_teacher_curriculum_catalogue_find(const UmiTeacherCurriculumCatalogue *catalogue, const char *id);
size_t umi_teacher_curriculum_catalogue_eligible_count(const UmiTeacherCurriculumCatalogue *catalogue, UmiTeacherLanguage language, UmiTeacherLevel maximum_level);

#ifdef __cplusplus
}
#endif

#endif
