/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/curriculum_prerequisite.h
 *
 * PURPOSE:
 *   Evaluate prerequisite mastery before a learner enters a curriculum item.
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
#ifndef UMICOM_TEACHER_CURRICULUM_PREREQUISITE_H
#define UMICOM_TEACHER_CURRICULUM_PREREQUISITE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/teacher/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTeacherCurriculumPrerequisite {
    uint32_t minimum_mastery;
    uint32_t minimum_attempts;
    uint32_t maximum_failures;
    UmiTeacherLevel maximum_level;
    uint32_t language_mask;
    int enabled;
} UmiTeacherCurriculumPrerequisite;

void umi_teacher_curriculum_prerequisite_init(UmiTeacherCurriculumPrerequisite *policy);
UmiStatus umi_teacher_curriculum_prerequisite_configure(UmiTeacherCurriculumPrerequisite *policy, uint32_t minimum_mastery, uint32_t minimum_attempts, uint32_t maximum_failures, UmiTeacherLevel maximum_level, uint32_t language_mask);
int umi_teacher_curriculum_prerequisite_allows(const UmiTeacherCurriculumPrerequisite *policy, uint32_t mastery, uint32_t attempts, uint32_t failures, UmiTeacherLevel level, UmiTeacherLanguage language);
uint32_t umi_teacher_curriculum_prerequisite_deficit(const UmiTeacherCurriculumPrerequisite *policy, uint32_t mastery);

#ifdef __cplusplus
}
#endif

#endif
