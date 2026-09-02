/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/cpython_track.h
 *
 * PURPOSE:
 *   Describe CPython and Python/C interoperability learning progression.
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
#ifndef UMICOM_TEACHER_CPYTHON_TRACK_H
#define UMICOM_TEACHER_CPYTHON_TRACK_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/teacher/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the teacher cpython track data shared with callers of this public contract.
 */
typedef struct UmiTeacherCpythonTrack {
    char id[UMI_TEACHER_ID_CAPACITY];
    char title[UMI_TEACHER_TEXT_CAPACITY];
    UmiTeacherLanguage language;
    UmiTeacherLevel level;
    uint32_t weight;
    uint32_t required_score;
    uint64_t revision;
    int enabled;
} UmiTeacherCpythonTrack;

/**
 * Initialise teacher cpython track from caller-provided values so later operations receive
 * a known state.
 */
void umi_teacher_cpython_track_init(UmiTeacherCpythonTrack *value);
/**
 * Provide the teacher cpython track configure operation used by this module and its client
 * applications.
 */
UmiStatus umi_teacher_cpython_track_configure(UmiTeacherCpythonTrack *value, const char *id, const char *title, UmiTeacherLanguage language, UmiTeacherLevel level, uint32_t weight, uint32_t required_score);
/**
 * Check that teacher cpython track satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_teacher_cpython_track_validate(const UmiTeacherCpythonTrack *value);
/**
 * Provide the teacher cpython track priority operation used by this module and its client
 * applications.
 */
uint32_t umi_teacher_cpython_track_priority(const UmiTeacherCpythonTrack *value, uint32_t relevance);

#ifdef __cplusplus
}
#endif

#endif
