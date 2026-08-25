/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/assessment_threshold.h
 *
 * PURPOSE:
 *   Apply pass/mastery thresholds to assessment scores.
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
#ifndef UMICOM_TEACHER_ASSESSMENT_THRESHOLD_H
#define UMICOM_TEACHER_ASSESSMENT_THRESHOLD_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/teacher/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTeacherAssessmentThreshold {
    uint32_t minimum_mastery;
    uint32_t minimum_attempts;
    uint32_t maximum_failures;
    UmiTeacherLevel maximum_level;
    uint32_t language_mask;
    int enabled;
} UmiTeacherAssessmentThreshold;

void umi_teacher_assessment_threshold_init(UmiTeacherAssessmentThreshold *policy);
UmiStatus umi_teacher_assessment_threshold_configure(UmiTeacherAssessmentThreshold *policy, uint32_t minimum_mastery, uint32_t minimum_attempts, uint32_t maximum_failures, UmiTeacherLevel maximum_level, uint32_t language_mask);
int umi_teacher_assessment_threshold_allows(const UmiTeacherAssessmentThreshold *policy, uint32_t mastery, uint32_t attempts, uint32_t failures, UmiTeacherLevel level, UmiTeacherLanguage language);
uint32_t umi_teacher_assessment_threshold_deficit(const UmiTeacherAssessmentThreshold *policy, uint32_t mastery);

#ifdef __cplusplus
}
#endif

#endif
