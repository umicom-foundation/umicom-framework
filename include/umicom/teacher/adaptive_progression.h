/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/adaptive_progression.h
 *
 * PURPOSE:
 *   Choose advance/repeat/remediate progression from mastery evidence.
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
#ifndef UMICOM_TEACHER_ADAPTIVE_PROGRESSION_H
#define UMICOM_TEACHER_ADAPTIVE_PROGRESSION_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/teacher/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTeacherAdaptiveProgression { uint32_t remediate_below; uint32_t repeat_below; uint32_t advance_at; uint32_t minimum_confidence; } UmiTeacherAdaptiveProgression;
void umi_teacher_adaptive_progression_init(UmiTeacherAdaptiveProgression *model);
UmiStatus umi_teacher_adaptive_progression_configure(UmiTeacherAdaptiveProgression *model, uint32_t remediate_below, uint32_t repeat_below, uint32_t advance_at, uint32_t minimum_confidence);
UmiTeacherProgressDecision umi_teacher_adaptive_progression_decide(const UmiTeacherAdaptiveProgression *model, uint32_t mastery, uint32_t confidence);

#ifdef __cplusplus
}
#endif

#endif
