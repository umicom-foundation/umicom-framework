/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/c23_track.h
 *
 * PURPOSE:
 *   Describe C23 learning progression from syntax through systems engineering.
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
#ifndef UMICOM_TEACHER_C23_TRACK_H
#define UMICOM_TEACHER_C23_TRACK_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/teacher/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the teacher c23 track data shared with callers of this public contract.
 */
typedef struct UmiTeacherC23Track {
    char id[UMI_TEACHER_ID_CAPACITY];
    char title[UMI_TEACHER_TEXT_CAPACITY];
    UmiTeacherLanguage language;
    UmiTeacherLevel level;
    uint32_t weight;
    uint32_t required_score;
    uint64_t revision;
    int enabled;
} UmiTeacherC23Track;

/**
 * Initialise teacher c23 track from caller-provided values so later operations receive a
 * known state.
 */
void umi_teacher_c23_track_init(UmiTeacherC23Track *value);
/**
 * Provide the teacher c23 track configure operation used by this module and its client
 * applications.
 */
UmiStatus umi_teacher_c23_track_configure(UmiTeacherC23Track *value, const char *id, const char *title, UmiTeacherLanguage language, UmiTeacherLevel level, uint32_t weight, uint32_t required_score);
/**
 * Check that teacher c23 track satisfies its contract before another service relies on it.
 */
UmiStatus umi_teacher_c23_track_validate(const UmiTeacherC23Track *value);
/**
 * Provide the teacher c23 track priority operation used by this module and its client
 * applications.
 */
uint32_t umi_teacher_c23_track_priority(const UmiTeacherC23Track *value, uint32_t relevance);

#ifdef __cplusplus
}
#endif

#endif
