/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/progress_history.h
 *
 * PURPOSE:
 *   Maintain bounded historical progress evidence.
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
#ifndef UMICOM_TEACHER_PROGRESS_HISTORY_H
#define UMICOM_TEACHER_PROGRESS_HISTORY_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/teacher/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTeacherProgressHistoryEntry { uint64_t revision; uint32_t mastery; uint32_t completion; uint32_t score; } UmiTeacherProgressHistoryEntry;
typedef struct UmiTeacherProgressHistory { UmiTeacherProgressHistoryEntry entries[UMI_TEACHER_MEDIUM_CAPACITY]; size_t count; size_t next_slot; } UmiTeacherProgressHistory;
void umi_teacher_progress_history_init(UmiTeacherProgressHistory *history);
void umi_teacher_progress_history_record(UmiTeacherProgressHistory *history, uint64_t revision, uint32_t mastery, uint32_t completion, uint32_t score);
size_t umi_teacher_progress_history_count(const UmiTeacherProgressHistory *history);
const UmiTeacherProgressHistoryEntry *umi_teacher_progress_history_latest(const UmiTeacherProgressHistory *history);

#ifdef __cplusplus
}
#endif

#endif
