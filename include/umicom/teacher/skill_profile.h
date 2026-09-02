/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/skill_profile.h
 *
 * PURPOSE:
 *   Maintain bounded skill mastery entries for a learner.
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
#ifndef UMICOM_TEACHER_SKILL_PROFILE_H
#define UMICOM_TEACHER_SKILL_PROFILE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/teacher/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the teacher skill profile skill data shared with callers of this public
 * contract.
 */
typedef struct UmiTeacherSkillProfileSkill { char id[UMI_TEACHER_ID_CAPACITY]; uint32_t mastery; uint32_t confidence; } UmiTeacherSkillProfileSkill;
/**
 * Represent the teacher skill profile data shared with callers of this public contract.
 */
typedef struct UmiTeacherSkillProfile { UmiTeacherSkillProfileSkill skills[UMI_TEACHER_SMALL_CAPACITY]; size_t count; uint64_t revision; } UmiTeacherSkillProfile;
/**
 * Initialise teacher skill profile from caller-provided values so later operations receive
 * a known state.
 */
void umi_teacher_skill_profile_init(UmiTeacherSkillProfile *profile);
/**
 * Copy teacher skill profile into module-owned storage so callers keep ownership of their
 * input values.
 */
UmiStatus umi_teacher_skill_profile_set(UmiTeacherSkillProfile *profile, const char *id, uint32_t mastery, uint32_t confidence);
/**
 * Find teacher skill profile while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiTeacherSkillProfileSkill *umi_teacher_skill_profile_find(const UmiTeacherSkillProfile *profile, const char *id);
/**
 * Provide the teacher skill profile average mastery operation used by this module and its
 * client applications.
 */
uint32_t umi_teacher_skill_profile_average_mastery(const UmiTeacherSkillProfile *profile);

#ifdef __cplusplus
}
#endif

#endif
