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

/**
 * Represent the teacher adaptive progression data shared with callers of this public
 * contract.
 */
typedef struct UmiTeacherAdaptiveProgression { uint32_t remediate_below; uint32_t repeat_below; uint32_t advance_at; uint32_t minimum_confidence; } UmiTeacherAdaptiveProgression;
/**
 * Initialise teacher adaptive progression from caller-provided values so later operations
 * receive a known state.
 */
void umi_teacher_adaptive_progression_init(UmiTeacherAdaptiveProgression *model);
/**
 * Provide the teacher adaptive progression configure operation used by this module and its
 * client applications.
 */
UmiStatus umi_teacher_adaptive_progression_configure(UmiTeacherAdaptiveProgression *model, uint32_t remediate_below, uint32_t repeat_below, uint32_t advance_at, uint32_t minimum_confidence);
/**
 * Provide the teacher adaptive progression decide operation used by this module and its
 * client applications.
 */
UmiTeacherProgressDecision umi_teacher_adaptive_progression_decide(const UmiTeacherAdaptiveProgression *model, uint32_t mastery, uint32_t confidence);

#ifdef __cplusplus
}
#endif

#endif
