/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/assessment_feedback.h
 *
 * PURPOSE:
 *   Describe feedback tied to assessment evidence.
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
#ifndef UMICOM_TEACHER_ASSESSMENT_FEEDBACK_H
#define UMICOM_TEACHER_ASSESSMENT_FEEDBACK_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/teacher/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the teacher assessment feedback data shared with callers of this public
 * contract.
 */
typedef struct UmiTeacherAssessmentFeedback {
    char id[UMI_TEACHER_ID_CAPACITY];
    char title[UMI_TEACHER_TEXT_CAPACITY];
    UmiTeacherLanguage language;
    UmiTeacherLevel level;
    uint32_t weight;
    uint32_t required_score;
    uint64_t revision;
    int enabled;
} UmiTeacherAssessmentFeedback;

/**
 * Initialise teacher assessment feedback from caller-provided values so later operations
 * receive a known state.
 */
void umi_teacher_assessment_feedback_init(UmiTeacherAssessmentFeedback *value);
/**
 * Provide the teacher assessment feedback configure operation used by this module and its
 * client applications.
 */
UmiStatus umi_teacher_assessment_feedback_configure(UmiTeacherAssessmentFeedback *value, const char *id, const char *title, UmiTeacherLanguage language, UmiTeacherLevel level, uint32_t weight, uint32_t required_score);
/**
 * Check that teacher assessment feedback satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_teacher_assessment_feedback_validate(const UmiTeacherAssessmentFeedback *value);
/**
 * Provide the teacher assessment feedback priority operation used by this module and its
 * client applications.
 */
uint32_t umi_teacher_assessment_feedback_priority(const UmiTeacherAssessmentFeedback *value, uint32_t relevance);

#ifdef __cplusplus
}
#endif

#endif
