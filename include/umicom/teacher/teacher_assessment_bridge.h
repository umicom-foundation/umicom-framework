/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/teacher_assessment_bridge.h
 *
 * PURPOSE:
 *   Bind Teacher assessments to existing Framework compiler/testing services.
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
#ifndef UMICOM_TEACHER_TEACHER_ASSESSMENT_BRIDGE_H
#define UMICOM_TEACHER_TEACHER_ASSESSMENT_BRIDGE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/teacher/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the teacher teacher assessment bridge data shared with callers of this public
 * contract.
 */
typedef struct UmiTeacherTeacherAssessmentBridge {
    char id[UMI_TEACHER_ID_CAPACITY];
    char title[UMI_TEACHER_TEXT_CAPACITY];
    UmiTeacherLanguage language;
    UmiTeacherLevel level;
    uint32_t weight;
    uint32_t required_score;
    uint64_t revision;
    int enabled;
} UmiTeacherTeacherAssessmentBridge;

/**
 * Initialise teacher teacher assessment bridge from caller-provided values so later
 * operations receive a known state.
 */
void umi_teacher_teacher_assessment_bridge_init(UmiTeacherTeacherAssessmentBridge *value);
/**
 * Provide the teacher teacher assessment bridge configure operation used by this module
 * and its client applications.
 */
UmiStatus umi_teacher_teacher_assessment_bridge_configure(UmiTeacherTeacherAssessmentBridge *value, const char *id, const char *title, UmiTeacherLanguage language, UmiTeacherLevel level, uint32_t weight, uint32_t required_score);
/**
 * Check that teacher teacher assessment bridge satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_teacher_teacher_assessment_bridge_validate(const UmiTeacherTeacherAssessmentBridge *value);
/**
 * Provide the teacher teacher assessment bridge priority operation used by this module and
 * its client applications.
 */
uint32_t umi_teacher_teacher_assessment_bridge_priority(const UmiTeacherTeacherAssessmentBridge *value, uint32_t relevance);

#ifdef __cplusplus
}
#endif

#endif
