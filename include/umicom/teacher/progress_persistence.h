/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/progress_persistence.h
 *
 * PURPOSE:
 *   Track save/load revision evidence for learner progress state.
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
#ifndef UMICOM_TEACHER_PROGRESS_PERSISTENCE_H
#define UMICOM_TEACHER_PROGRESS_PERSISTENCE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/teacher/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTeacherProgressPersistence {
    UmiTeacherState state;
    uint32_t score;
    uint32_t attempts;
    uint32_t completed_units;
    uint32_t total_units;
    uint64_t revision;
} UmiTeacherProgressPersistence;

void umi_teacher_progress_persistence_init(UmiTeacherProgressPersistence *value);
UmiStatus umi_teacher_progress_persistence_record(UmiTeacherProgressPersistence *value, uint32_t score, int passed);
void umi_teacher_progress_persistence_set_units(UmiTeacherProgressPersistence *value, uint32_t completed_units, uint32_t total_units);
uint32_t umi_teacher_progress_persistence_completion(const UmiTeacherProgressPersistence *value);
int umi_teacher_progress_persistence_complete(const UmiTeacherProgressPersistence *value);

#ifdef __cplusplus
}
#endif

#endif
