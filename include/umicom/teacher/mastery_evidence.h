/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/mastery_evidence.h
 *
 * PURPOSE:
 *   Describe one unit of mastery evidence.
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
#ifndef UMICOM_TEACHER_MASTERY_EVIDENCE_H
#define UMICOM_TEACHER_MASTERY_EVIDENCE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/teacher/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the teacher mastery evidence data shared with callers of this public contract.
 */
typedef struct UmiTeacherMasteryEvidence {
    char id[UMI_TEACHER_ID_CAPACITY];
    char title[UMI_TEACHER_TEXT_CAPACITY];
    UmiTeacherLanguage language;
    UmiTeacherLevel level;
    uint32_t weight;
    uint32_t required_score;
    uint64_t revision;
    int enabled;
} UmiTeacherMasteryEvidence;

/**
 * Initialise teacher mastery evidence from caller-provided values so later operations
 * receive a known state.
 */
void umi_teacher_mastery_evidence_init(UmiTeacherMasteryEvidence *value);
/**
 * Provide the teacher mastery evidence configure operation used by this module and its
 * client applications.
 */
UmiStatus umi_teacher_mastery_evidence_configure(UmiTeacherMasteryEvidence *value, const char *id, const char *title, UmiTeacherLanguage language, UmiTeacherLevel level, uint32_t weight, uint32_t required_score);
/**
 * Check that teacher mastery evidence satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_teacher_mastery_evidence_validate(const UmiTeacherMasteryEvidence *value);
/**
 * Provide the teacher mastery evidence priority operation used by this module and its
 * client applications.
 */
uint32_t umi_teacher_mastery_evidence_priority(const UmiTeacherMasteryEvidence *value, uint32_t relevance);

#ifdef __cplusplus
}
#endif

#endif
