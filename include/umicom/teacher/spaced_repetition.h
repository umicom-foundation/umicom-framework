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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiTeacherSpacedRepetition { uint32_t interval_days; uint32_t repetitions; uint32_t ease_percent; uint32_t last_quality; } UmiTeacherSpacedRepetition;
void umi_teacher_spaced_repetition_init(UmiTeacherSpacedRepetition *schedule);
UmiStatus umi_teacher_spaced_repetition_record(UmiTeacherSpacedRepetition *schedule, uint32_t recall_quality);
uint32_t umi_teacher_spaced_repetition_next_interval(const UmiTeacherSpacedRepetition *schedule);

#ifdef __cplusplus
}
#endif

#endif
