/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/cpp_track.h
 *
 * PURPOSE:
 *   Describe optional C++ learning progression while preserving C-first priorities.
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
#ifndef UMICOM_TEACHER_CPP_TRACK_H
#define UMICOM_TEACHER_CPP_TRACK_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/teacher/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTeacherCppTrack {
    char id[UMI_TEACHER_ID_CAPACITY];
    char title[UMI_TEACHER_TEXT_CAPACITY];
    UmiTeacherLanguage language;
    UmiTeacherLevel level;
    uint32_t weight;
    uint32_t required_score;
    uint64_t revision;
    int enabled;
} UmiTeacherCppTrack;

void umi_teacher_cpp_track_init(UmiTeacherCppTrack *value);
UmiStatus umi_teacher_cpp_track_configure(UmiTeacherCppTrack *value, const char *id, const char *title, UmiTeacherLanguage language, UmiTeacherLevel level, uint32_t weight, uint32_t required_score);
UmiStatus umi_teacher_cpp_track_validate(const UmiTeacherCppTrack *value);
uint32_t umi_teacher_cpp_track_priority(const UmiTeacherCppTrack *value, uint32_t relevance);

#ifdef __cplusplus
}
#endif

#endif
