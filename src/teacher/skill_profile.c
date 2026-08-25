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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/teacher/skill_profile.h"
#include <string.h>
static void umi_teacher_skill_profile_copy(char *destination, size_t capacity, const char *source) {
    size_t i = 0U;
    if (destination == NULL || capacity == 0U) {
        return;
    }
    if (source != NULL) {
        while (i + 1U < capacity && source[i] != '\0') {
            destination[i] = source[i];
            ++i;
        }
    }
    destination[i] = '\0';
}

void umi_teacher_skill_profile_init(UmiTeacherSkillProfile *profile) { if(profile!=NULL) memset(profile,0,sizeof(*profile));
    }
const UmiTeacherSkillProfileSkill *umi_teacher_skill_profile_find(const UmiTeacherSkillProfile *profile,const char *id) { size_t i;
    if(profile==NULL||id==NULL) return NULL;
    for(i=0U;i<profile->count;++i) if(strcmp(profile->skills[i].id,id)==0) return &profile->skills[i];
    return NULL;
    }
UmiStatus umi_teacher_skill_profile_set(UmiTeacherSkillProfile *profile,const char *id,uint32_t mastery,uint32_t confidence) { size_t i;
    if(profile==NULL||id==NULL||id[0]=='\0'||mastery>100U||confidence>100U) return UMI_STATUS_INVALID_ARGUMENT;
    for(i=0U;i<profile->count;++i) if(strcmp(profile->skills[i].id,id)==0) { profile->skills[i].mastery=mastery;
    profile->skills[i].confidence=confidence;
    ++profile->revision;
    return UMI_STATUS_OK;
    } if(profile->count>=UMI_TEACHER_SMALL_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    i=profile->count++;
    umi_teacher_skill_profile_copy(profile->skills[i].id,UMI_TEACHER_ID_CAPACITY,id);
    profile->skills[i].mastery=mastery;
    profile->skills[i].confidence=confidence;
    ++profile->revision;
    return UMI_STATUS_OK;
    }
uint32_t umi_teacher_skill_profile_average_mastery(const UmiTeacherSkillProfile *profile) { size_t i;
    uint64_t total=0U;
    if(profile==NULL||profile->count==0U) return 0U;
    for(i=0U;i<profile->count;++i) total+=profile->skills[i].mastery;
    return (uint32_t)(total/profile->count);
    }
