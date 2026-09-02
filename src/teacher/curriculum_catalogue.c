/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/teacher/curriculum_catalogue.c
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
#include "umicom/teacher/curriculum_catalogue.h"
#include <string.h>
/*
 * Copy teacher curriculum catalogue into module-owned storage so callers keep ownership of
 * their input values.
 */
static void umi_teacher_curriculum_catalogue_copy(char *destination, size_t capacity, const char *source) {
    size_t i = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U) {
        return;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (source != NULL) {
        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (i + 1U < capacity && source[i] != '\0') {
            destination[i] = source[i];
            ++i;
        }
    }
    destination[i] = '\0';
}

/*
 * Initialise teacher curriculum catalogue from caller-provided values so later operations
 * receive a known state.
 */
void umi_teacher_curriculum_catalogue_init(UmiTeacherCurriculumCatalogue *catalogue) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (catalogue != NULL) memset(catalogue, 0, sizeof(*catalogue));
    }
/*
 * Find teacher curriculum catalogue while leaving the underlying catalogue or model owned
 * by this module.
 */
const UmiTeacherCurriculumCatalogueEntry *umi_teacher_curriculum_catalogue_find(const UmiTeacherCurriculumCatalogue *catalogue, const char *id) {
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U;
    i < catalogue->count;
    ++i) /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(catalogue->entries[i].id, id) == 0) return &catalogue->entries[i];
    return NULL;
}
/*
 * Provide the teacher curriculum catalogue upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_teacher_curriculum_catalogue_upsert(UmiTeacherCurriculumCatalogue *catalogue, const char *id, UmiTeacherLanguage language, UmiTeacherLevel level, uint32_t priority) {
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || id == NULL || id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U;
    i < catalogue->count;
    ++i) /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(catalogue->entries[i].id, id) == 0) { catalogue->entries[i].language=language;
    catalogue->entries[i].level=level;
    catalogue->entries[i].priority=priority;
    catalogue->entries[i].enabled=1;
    ++catalogue->revision;
    return UMI_STATUS_OK;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (catalogue->count >= UMI_TEACHER_SMALL_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    i = catalogue->count++;
    umi_teacher_curriculum_catalogue_copy(catalogue->entries[i].id, UMI_TEACHER_ID_CAPACITY, id);
    catalogue->entries[i].language=language;
    catalogue->entries[i].level=level;
    catalogue->entries[i].priority=priority;
    catalogue->entries[i].enabled=1;
    ++catalogue->revision;
    return UMI_STATUS_OK;
}
/*
 * Return the number of records represented by teacher curriculum catalogue eligible
 * without changing their state.
 */
size_t umi_teacher_curriculum_catalogue_eligible_count(const UmiTeacherCurriculumCatalogue *catalogue, UmiTeacherLanguage language, UmiTeacherLevel maximum_level) {
    size_t i, count=0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL) return 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i=0U;i<catalogue->count;++i) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (catalogue->entries[i].enabled && (catalogue->entries[i].language==language || catalogue->entries[i].language==UMI_TEACHER_LANGUAGE_GENERAL) && catalogue->entries[i].level<=maximum_level) ++count;
    return count;
}
