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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * Represent the teacher progress history entry data shared with callers of this public
 * contract.
 */
typedef struct UmiTeacherProgressHistoryEntry { uint64_t revision; uint32_t mastery; uint32_t completion; uint32_t score; } UmiTeacherProgressHistoryEntry;
/**
 * Represent the teacher progress history data shared with callers of this public contract.
 */
typedef struct UmiTeacherProgressHistory { UmiTeacherProgressHistoryEntry entries[UMI_TEACHER_MEDIUM_CAPACITY]; size_t count; size_t next_slot; } UmiTeacherProgressHistory;
/**
 * Initialise teacher progress history from caller-provided values so later operations
 * receive a known state.
 */
void umi_teacher_progress_history_init(UmiTeacherProgressHistory *history);
/**
 * Provide the teacher progress history record operation used by this module and its client
 * applications.
 */
void umi_teacher_progress_history_record(UmiTeacherProgressHistory *history, uint64_t revision, uint32_t mastery, uint32_t completion, uint32_t score);
/**
 * Return the number of records represented by teacher progress history without changing
 * their state.
 */
size_t umi_teacher_progress_history_count(const UmiTeacherProgressHistory *history);
/**
 * Provide the teacher progress history latest operation used by this module and its client
 * applications.
 */
const UmiTeacherProgressHistoryEntry *umi_teacher_progress_history_latest(const UmiTeacherProgressHistory *history);

#ifdef __cplusplus
}
#endif

#endif
