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

/**
 * Represent the teacher assessment catalogue entry data shared with callers of this public
 * contract.
 */
typedef struct UmiTeacherAssessmentCatalogueEntry {
    char id[UMI_TEACHER_ID_CAPACITY];
    UmiTeacherLanguage language;
    UmiTeacherLevel level;
    uint32_t priority;
    int enabled;
} UmiTeacherAssessmentCatalogueEntry;

/**
 * Represent the teacher assessment catalogue data shared with callers of this public
 * contract.
 */
typedef struct UmiTeacherAssessmentCatalogue {
    UmiTeacherAssessmentCatalogueEntry entries[UMI_TEACHER_SMALL_CAPACITY];
    size_t count;
    uint64_t revision;
} UmiTeacherAssessmentCatalogue;

/**
 * Initialise teacher assessment catalogue from caller-provided values so later operations
 * receive a known state.
 */
void umi_teacher_assessment_catalogue_init(UmiTeacherAssessmentCatalogue *catalogue);
/**
 * Provide the teacher assessment catalogue upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_teacher_assessment_catalogue_upsert(UmiTeacherAssessmentCatalogue *catalogue, const char *id, UmiTeacherLanguage language, UmiTeacherLevel level, uint32_t priority);
/**
 * Find teacher assessment catalogue while leaving the underlying catalogue or model owned
 * by this module.
 */
const UmiTeacherAssessmentCatalogueEntry *umi_teacher_assessment_catalogue_find(const UmiTeacherAssessmentCatalogue *catalogue, const char *id);
/**
 * Return the number of records represented by teacher assessment catalogue eligible
 * without changing their state.
 */
size_t umi_teacher_assessment_catalogue_eligible_count(const UmiTeacherAssessmentCatalogue *catalogue, UmiTeacherLanguage language, UmiTeacherLevel maximum_level);

#ifdef __cplusplus
}
#endif

#endif
