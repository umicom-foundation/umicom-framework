/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/teacher/skill_profile.c
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
#include "umicom/teacher/skill_profile.h"
#include <string.h>
/*
 * Copy teacher skill profile into module-owned storage so callers keep ownership of their
 * input values.
 */
static void umi_teacher_skill_profile_copy(char *destination, size_t capacity, const char *source) {
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
 * Initialise teacher skill profile from caller-provided values so later operations receive
 * a known state.
 */
void umi_teacher_skill_profile_init(UmiTeacherSkillProfile *profile) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(profile!=NULL) memset(profile,0,sizeof(*profile));
    }
/*
 * Find teacher skill profile while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiTeacherSkillProfileSkill *umi_teacher_skill_profile_find(const UmiTeacherSkillProfile *profile,const char *id) { size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(profile==NULL||id==NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for(i=0U;i<profile->count;++i) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(profile->skills[i].id,id)==0) return &profile->skills[i];
    return NULL;
    }
/*
 * Copy teacher skill profile into module-owned storage so callers keep ownership of their
 * input values.
 */
UmiStatus umi_teacher_skill_profile_set(UmiTeacherSkillProfile *profile,const char *id,uint32_t mastery,uint32_t confidence) { size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(profile==NULL||id==NULL||id[0]=='\0'||mastery>100U||confidence>100U) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for(i=0U;i<profile->count;++i) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(profile->skills[i].id,id)==0) { profile->skills[i].mastery=mastery;
    profile->skills[i].confidence=confidence;
    ++profile->revision;
    return UMI_STATUS_OK;
    } /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(profile->count>=UMI_TEACHER_SMALL_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    i=profile->count++;
    umi_teacher_skill_profile_copy(profile->skills[i].id,UMI_TEACHER_ID_CAPACITY,id);
    profile->skills[i].mastery=mastery;
    profile->skills[i].confidence=confidence;
    ++profile->revision;
    return UMI_STATUS_OK;
    }
/*
 * Provide the teacher skill profile average mastery operation used by this module and its
 * client applications.
 */
uint32_t umi_teacher_skill_profile_average_mastery(const UmiTeacherSkillProfile *profile) { size_t i;
    uint64_t total=0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(profile==NULL||profile->count==0U) return 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for(i=0U;i<profile->count;++i) total+=profile->skills[i].mastery;
    return (uint32_t)(total/profile->count);
    }
