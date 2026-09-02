/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/exercise_catalogue.h
 *
 * PURPOSE:
 *   Catalogue exercises independently of Studio presentation.
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
#ifndef UMICOM_TEACHER_EXERCISE_CATALOGUE_H
#define UMICOM_TEACHER_EXERCISE_CATALOGUE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/teacher/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the teacher exercise catalogue entry data shared with callers of this public
 * contract.
 */
typedef struct UmiTeacherExerciseCatalogueEntry {
    char id[UMI_TEACHER_ID_CAPACITY];
    UmiTeacherLanguage language;
    UmiTeacherLevel level;
    uint32_t priority;
    int enabled;
} UmiTeacherExerciseCatalogueEntry;

/**
 * Represent the teacher exercise catalogue data shared with callers of this public
 * contract.
 */
typedef struct UmiTeacherExerciseCatalogue {
    UmiTeacherExerciseCatalogueEntry entries[UMI_TEACHER_SMALL_CAPACITY];
    size_t count;
    uint64_t revision;
} UmiTeacherExerciseCatalogue;

/**
 * Initialise teacher exercise catalogue from caller-provided values so later operations
 * receive a known state.
 */
void umi_teacher_exercise_catalogue_init(UmiTeacherExerciseCatalogue *catalogue);
/**
 * Provide the teacher exercise catalogue upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_teacher_exercise_catalogue_upsert(UmiTeacherExerciseCatalogue *catalogue, const char *id, UmiTeacherLanguage language, UmiTeacherLevel level, uint32_t priority);
/**
 * Find teacher exercise catalogue while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiTeacherExerciseCatalogueEntry *umi_teacher_exercise_catalogue_find(const UmiTeacherExerciseCatalogue *catalogue, const char *id);
/**
 * Return the number of records represented by teacher exercise catalogue eligible without
 * changing their state.
 */
size_t umi_teacher_exercise_catalogue_eligible_count(const UmiTeacherExerciseCatalogue *catalogue, UmiTeacherLanguage language, UmiTeacherLevel maximum_level);

#ifdef __cplusplus
}
#endif

#endif
