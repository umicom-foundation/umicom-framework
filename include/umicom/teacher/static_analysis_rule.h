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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * Represent the teacher static analysis rule data shared with callers of this public
 * contract.
 */
typedef struct UmiTeacherStaticAnalysisRule {
    uint32_t minimum_mastery;
    uint32_t minimum_attempts;
    uint32_t maximum_failures;
    UmiTeacherLevel maximum_level;
    uint32_t language_mask;
    int enabled;
} UmiTeacherStaticAnalysisRule;

/**
 * Initialise teacher static analysis rule from caller-provided values so later operations
 * receive a known state.
 */
void umi_teacher_static_analysis_rule_init(UmiTeacherStaticAnalysisRule *policy);
/**
 * Provide the teacher static analysis rule configure operation used by this module and its
 * client applications.
 */
UmiStatus umi_teacher_static_analysis_rule_configure(UmiTeacherStaticAnalysisRule *policy, uint32_t minimum_mastery, uint32_t minimum_attempts, uint32_t maximum_failures, UmiTeacherLevel maximum_level, uint32_t language_mask);
/**
 * Provide the teacher static analysis rule allows operation used by this module and its
 * client applications.
 */
int umi_teacher_static_analysis_rule_allows(const UmiTeacherStaticAnalysisRule *policy, uint32_t mastery, uint32_t attempts, uint32_t failures, UmiTeacherLevel level, UmiTeacherLanguage language);
/**
 * Provide the teacher static analysis rule deficit operation used by this module and its
 * client applications.
 */
uint32_t umi_teacher_static_analysis_rule_deficit(const UmiTeacherStaticAnalysisRule *policy, uint32_t mastery);

#ifdef __cplusplus
}
#endif

#endif
