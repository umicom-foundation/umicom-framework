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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * Represent the teacher progress snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiTeacherProgressSnapshot {
    UmiTeacherState state;
    uint32_t score;
    uint32_t attempts;
    uint32_t completed_units;
    uint32_t total_units;
    uint64_t revision;
} UmiTeacherProgressSnapshot;

/**
 * Initialise teacher progress snapshot from caller-provided values so later operations
 * receive a known state.
 */
void umi_teacher_progress_snapshot_init(UmiTeacherProgressSnapshot *value);
/**
 * Provide the teacher progress snapshot record operation used by this module and its
 * client applications.
 */
UmiStatus umi_teacher_progress_snapshot_record(UmiTeacherProgressSnapshot *value, uint32_t score, int passed);
/**
 * Provide the teacher progress snapshot set units operation used by this module and its
 * client applications.
 */
void umi_teacher_progress_snapshot_set_units(UmiTeacherProgressSnapshot *value, uint32_t completed_units, uint32_t total_units);
/**
 * Provide the teacher progress snapshot completion operation used by this module and its
 * client applications.
 */
uint32_t umi_teacher_progress_snapshot_completion(const UmiTeacherProgressSnapshot *value);
/**
 * Provide the teacher progress snapshot complete operation used by this module and its
 * client applications.
 */
int umi_teacher_progress_snapshot_complete(const UmiTeacherProgressSnapshot *value);

#ifdef __cplusplus
}
#endif

#endif
