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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * Represent the teacher curriculum prerequisite data shared with callers of this public
 * contract.
 */
typedef struct UmiTeacherCurriculumPrerequisite {
    uint32_t minimum_mastery;
    uint32_t minimum_attempts;
    uint32_t maximum_failures;
    UmiTeacherLevel maximum_level;
    uint32_t language_mask;
    int enabled;
} UmiTeacherCurriculumPrerequisite;

/**
 * Initialise teacher curriculum prerequisite from caller-provided values so later
 * operations receive a known state.
 */
void umi_teacher_curriculum_prerequisite_init(UmiTeacherCurriculumPrerequisite *policy);
/**
 * Provide the teacher curriculum prerequisite configure operation used by this module and
 * its client applications.
 */
UmiStatus umi_teacher_curriculum_prerequisite_configure(UmiTeacherCurriculumPrerequisite *policy, uint32_t minimum_mastery, uint32_t minimum_attempts, uint32_t maximum_failures, UmiTeacherLevel maximum_level, uint32_t language_mask);
/**
 * Provide the teacher curriculum prerequisite allows operation used by this module and its
 * client applications.
 */
int umi_teacher_curriculum_prerequisite_allows(const UmiTeacherCurriculumPrerequisite *policy, uint32_t mastery, uint32_t attempts, uint32_t failures, UmiTeacherLevel level, UmiTeacherLanguage language);
/**
 * Provide the teacher curriculum prerequisite deficit operation used by this module and
 * its client applications.
 */
uint32_t umi_teacher_curriculum_prerequisite_deficit(const UmiTeacherCurriculumPrerequisite *policy, uint32_t mastery);

#ifdef __cplusplus
}
#endif

#endif
