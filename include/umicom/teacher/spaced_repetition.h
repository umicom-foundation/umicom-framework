/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/spaced_repetition.h
 *
 * PURPOSE:
 *   Schedule review intervals from recall quality and prior interval.
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
#ifndef UMICOM_TEACHER_SPACED_REPETITION_H
#define UMICOM_TEACHER_SPACED_REPETITION_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/teacher/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the teacher spaced repetition data shared with callers of this public
 * contract.
 */
typedef struct UmiTeacherSpacedRepetition { uint32_t interval_days; uint32_t repetitions; uint32_t ease_percent; uint32_t last_quality; } UmiTeacherSpacedRepetition;
/**
 * Initialise teacher spaced repetition from caller-provided values so later operations
 * receive a known state.
 */
void umi_teacher_spaced_repetition_init(UmiTeacherSpacedRepetition *schedule);
/**
 * Provide the teacher spaced repetition record operation used by this module and its
 * client applications.
 */
UmiStatus umi_teacher_spaced_repetition_record(UmiTeacherSpacedRepetition *schedule, uint32_t recall_quality);
/**
 * Provide the teacher spaced repetition next interval operation used by this module and
 * its client applications.
 */
uint32_t umi_teacher_spaced_repetition_next_interval(const UmiTeacherSpacedRepetition *schedule);

#ifdef __cplusplus
}
#endif

#endif
