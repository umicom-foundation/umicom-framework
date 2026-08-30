/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/curriculum_ordering.h
 *
 * PURPOSE:
 *   Maintain deterministic curriculum ordering without application-owned sequencing logic.
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
#ifndef UMICOM_TEACHER_CURRICULUM_ORDERING_H
#define UMICOM_TEACHER_CURRICULUM_ORDERING_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/teacher/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTeacherCurriculumOrdering {
    char ids[UMI_TEACHER_MEDIUM_CAPACITY][UMI_TEACHER_ID_CAPACITY];
    size_t count;
    size_t cursor;
    uint64_t revision;
} UmiTeacherCurriculumOrdering;

void umi_teacher_curriculum_ordering_init(UmiTeacherCurriculumOrdering *sequence);
UmiStatus umi_teacher_curriculum_ordering_append(UmiTeacherCurriculumOrdering *sequence, const char *id);
const char *umi_teacher_curriculum_ordering_current(const UmiTeacherCurriculumOrdering *sequence);
UmiStatus umi_teacher_curriculum_ordering_next(UmiTeacherCurriculumOrdering *sequence);
UmiStatus umi_teacher_curriculum_ordering_previous(UmiTeacherCurriculumOrdering *sequence);
size_t umi_teacher_curriculum_ordering_count(const UmiTeacherCurriculumOrdering *sequence);

#ifdef __cplusplus
}
#endif

#endif
