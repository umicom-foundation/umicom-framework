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

/**
 * Represent the teacher curriculum catalogue entry data shared with callers of this public
 * contract.
 */
typedef struct UmiTeacherCurriculumCatalogueEntry {
    char id[UMI_TEACHER_ID_CAPACITY];
    UmiTeacherLanguage language;
    UmiTeacherLevel level;
    uint32_t priority;
    int enabled;
} UmiTeacherCurriculumCatalogueEntry;

/**
 * Represent the teacher curriculum catalogue data shared with callers of this public
 * contract.
 */
typedef struct UmiTeacherCurriculumCatalogue {
    UmiTeacherCurriculumCatalogueEntry entries[UMI_TEACHER_SMALL_CAPACITY];
    size_t count;
    uint64_t revision;
} UmiTeacherCurriculumCatalogue;

/**
 * Initialise teacher curriculum catalogue from caller-provided values so later operations
 * receive a known state.
 */
void umi_teacher_curriculum_catalogue_init(UmiTeacherCurriculumCatalogue *catalogue);
/**
 * Provide the teacher curriculum catalogue upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_teacher_curriculum_catalogue_upsert(UmiTeacherCurriculumCatalogue *catalogue, const char *id, UmiTeacherLanguage language, UmiTeacherLevel level, uint32_t priority);
/**
 * Find teacher curriculum catalogue while leaving the underlying catalogue or model owned
 * by this module.
 */
const UmiTeacherCurriculumCatalogueEntry *umi_teacher_curriculum_catalogue_find(const UmiTeacherCurriculumCatalogue *catalogue, const char *id);
/**
 * Return the number of records represented by teacher curriculum catalogue eligible
 * without changing their state.
 */
size_t umi_teacher_curriculum_catalogue_eligible_count(const UmiTeacherCurriculumCatalogue *catalogue, UmiTeacherLanguage language, UmiTeacherLevel maximum_level);

#ifdef __cplusplus
}
#endif

#endif
