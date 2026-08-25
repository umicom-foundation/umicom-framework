/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/teacher/static_analysis_rule.c
 *
 * PURPOSE:
 *   Gate static-analysis findings by severity and allowed count.
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
#include "umicom/teacher/static_analysis_rule.h"
#include <string.h>
void umi_teacher_static_analysis_rule_init(UmiTeacherStaticAnalysisRule *policy) { if (policy!=NULL) { memset(policy,0,sizeof(*policy));
    policy->maximum_level=UMI_TEACHER_LEVEL_EXPERT;
    policy->language_mask=UINT32_MAX;
    policy->enabled=1;
    } }
UmiStatus umi_teacher_static_analysis_rule_configure(UmiTeacherStaticAnalysisRule *policy,uint32_t minimum_mastery,uint32_t minimum_attempts,uint32_t maximum_failures,UmiTeacherLevel maximum_level,uint32_t language_mask) {
    if (policy==NULL || minimum_mastery>100U) return UMI_STATUS_INVALID_ARGUMENT;
    umi_teacher_static_analysis_rule_init(policy);
    policy->minimum_mastery=minimum_mastery;
    policy->minimum_attempts=minimum_attempts;
    policy->maximum_failures=maximum_failures;
    policy->maximum_level=maximum_level;
    policy->language_mask=language_mask;
    return UMI_STATUS_OK;
}
int umi_teacher_static_analysis_rule_allows(const UmiTeacherStaticAnalysisRule *policy,uint32_t mastery,uint32_t attempts,uint32_t failures,UmiTeacherLevel level,UmiTeacherLanguage language) {
    uint32_t bit;
    if (policy==NULL || !policy->enabled || mastery<policy->minimum_mastery || attempts<policy->minimum_attempts || failures>policy->maximum_failures || level>policy->maximum_level) return 0;
    bit = UINT32_C(1) << (uint32_t)language;
    return (policy->language_mask & bit) != 0U;
}
uint32_t umi_teacher_static_analysis_rule_deficit(const UmiTeacherStaticAnalysisRule *policy,uint32_t mastery) { if (policy==NULL || mastery>=policy->minimum_mastery) return 0U;
    return policy->minimum_mastery-mastery;
    }
