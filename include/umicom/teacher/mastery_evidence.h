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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

void umi_teacher_mastery_evidence_init(UmiTeacherMasteryEvidence *value);
UmiStatus umi_teacher_mastery_evidence_configure(UmiTeacherMasteryEvidence *value, const char *id, const char *title, UmiTeacherLanguage language, UmiTeacherLevel level, uint32_t weight, uint32_t required_score);
UmiStatus umi_teacher_mastery_evidence_validate(const UmiTeacherMasteryEvidence *value);
uint32_t umi_teacher_mastery_evidence_priority(const UmiTeacherMasteryEvidence *value, uint32_t relevance);

#ifdef __cplusplus
}
#endif

#endif
