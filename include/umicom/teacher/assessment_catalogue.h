/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/assessment_catalogue.h
 *
 * PURPOSE:
 *   Catalogue Framework-owned assessments.
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
#ifndef UMICOM_TEACHER_ASSESSMENT_CATALOGUE_H
#define UMICOM_TEACHER_ASSESSMENT_CATALOGUE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/teacher/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTeacherAssessmentCatalogueEntry {
    char id[UMI_TEACHER_ID_CAPACITY];
    UmiTeacherLanguage language;
    UmiTeacherLevel level;
    uint32_t priority;
    int enabled;
} UmiTeacherAssessmentCatalogueEntry;

typedef struct UmiTeacherAssessmentCatalogue {
    UmiTeacherAssessmentCatalogueEntry entries[UMI_TEACHER_SMALL_CAPACITY];
    size_t count;
    uint64_t revision;
} UmiTeacherAssessmentCatalogue;

void umi_teacher_assessment_catalogue_init(UmiTeacherAssessmentCatalogue *catalogue);
UmiStatus umi_teacher_assessment_catalogue_upsert(UmiTeacherAssessmentCatalogue *catalogue, const char *id, UmiTeacherLanguage language, UmiTeacherLevel level, uint32_t priority);
const UmiTeacherAssessmentCatalogueEntry *umi_teacher_assessment_catalogue_find(const UmiTeacherAssessmentCatalogue *catalogue, const char *id);
size_t umi_teacher_assessment_catalogue_eligible_count(const UmiTeacherAssessmentCatalogue *catalogue, UmiTeacherLanguage language, UmiTeacherLevel maximum_level);

#ifdef __cplusplus
}
#endif

#endif
