/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/static_analysis_rule.h
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
#ifndef UMICOM_TEACHER_STATIC_ANALYSIS_RULE_H
#define UMICOM_TEACHER_STATIC_ANALYSIS_RULE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/teacher/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTeacherStaticAnalysisRule {
    uint32_t minimum_mastery;
    uint32_t minimum_attempts;
    uint32_t maximum_failures;
    UmiTeacherLevel maximum_level;
    uint32_t language_mask;
    int enabled;
} UmiTeacherStaticAnalysisRule;

void umi_teacher_static_analysis_rule_init(UmiTeacherStaticAnalysisRule *policy);
UmiStatus umi_teacher_static_analysis_rule_configure(UmiTeacherStaticAnalysisRule *policy, uint32_t minimum_mastery, uint32_t minimum_attempts, uint32_t maximum_failures, UmiTeacherLevel maximum_level, uint32_t language_mask);
int umi_teacher_static_analysis_rule_allows(const UmiTeacherStaticAnalysisRule *policy, uint32_t mastery, uint32_t attempts, uint32_t failures, UmiTeacherLevel level, UmiTeacherLanguage language);
uint32_t umi_teacher_static_analysis_rule_deficit(const UmiTeacherStaticAnalysisRule *policy, uint32_t mastery);

#ifdef __cplusplus
}
#endif

#endif
