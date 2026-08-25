/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/progress_snapshot.h
 *
 * PURPOSE:
 *   Capture aggregate progress state for persistence and reporting.
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
#ifndef UMICOM_TEACHER_PROGRESS_SNAPSHOT_H
#define UMICOM_TEACHER_PROGRESS_SNAPSHOT_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/teacher/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTeacherProgressSnapshot {
    UmiTeacherState state;
    uint32_t score;
    uint32_t attempts;
    uint32_t completed_units;
    uint32_t total_units;
    uint64_t revision;
} UmiTeacherProgressSnapshot;

void umi_teacher_progress_snapshot_init(UmiTeacherProgressSnapshot *value);
UmiStatus umi_teacher_progress_snapshot_record(UmiTeacherProgressSnapshot *value, uint32_t score, int passed);
void umi_teacher_progress_snapshot_set_units(UmiTeacherProgressSnapshot *value, uint32_t completed_units, uint32_t total_units);
uint32_t umi_teacher_progress_snapshot_completion(const UmiTeacherProgressSnapshot *value);
int umi_teacher_progress_snapshot_complete(const UmiTeacherProgressSnapshot *value);

#ifdef __cplusplus
}
#endif

#endif
