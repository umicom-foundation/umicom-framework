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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

void umi_teacher_cpython_track_init(UmiTeacherCpythonTrack *value);
UmiStatus umi_teacher_cpython_track_configure(UmiTeacherCpythonTrack *value, const char *id, const char *title, UmiTeacherLanguage language, UmiTeacherLevel level, uint32_t weight, uint32_t required_score);
UmiStatus umi_teacher_cpython_track_validate(const UmiTeacherCpythonTrack *value);
uint32_t umi_teacher_cpython_track_priority(const UmiTeacherCpythonTrack *value, uint32_t relevance);

#ifdef __cplusplus
}
#endif

#endif
