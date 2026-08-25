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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiTeacherExerciseCatalogueEntry {
    char id[UMI_TEACHER_ID_CAPACITY];
    UmiTeacherLanguage language;
    UmiTeacherLevel level;
    uint32_t priority;
    int enabled;
} UmiTeacherExerciseCatalogueEntry;

typedef struct UmiTeacherExerciseCatalogue {
    UmiTeacherExerciseCatalogueEntry entries[UMI_TEACHER_SMALL_CAPACITY];
    size_t count;
    uint64_t revision;
} UmiTeacherExerciseCatalogue;

void umi_teacher_exercise_catalogue_init(UmiTeacherExerciseCatalogue *catalogue);
UmiStatus umi_teacher_exercise_catalogue_upsert(UmiTeacherExerciseCatalogue *catalogue, const char *id, UmiTeacherLanguage language, UmiTeacherLevel level, uint32_t priority);
const UmiTeacherExerciseCatalogueEntry *umi_teacher_exercise_catalogue_find(const UmiTeacherExerciseCatalogue *catalogue, const char *id);
size_t umi_teacher_exercise_catalogue_eligible_count(const UmiTeacherExerciseCatalogue *catalogue, UmiTeacherLanguage language, UmiTeacherLevel maximum_level);

#ifdef __cplusplus
}
#endif

#endif
