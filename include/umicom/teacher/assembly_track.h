/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/assembly_track.h
 *
 * PURPOSE:
 *   Describe architecture-aware Assembly learning progression.
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
#ifndef UMICOM_TEACHER_ASSEMBLY_TRACK_H
#define UMICOM_TEACHER_ASSEMBLY_TRACK_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/teacher/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTeacherAssemblyTrack {
    char id[UMI_TEACHER_ID_CAPACITY];
    char title[UMI_TEACHER_TEXT_CAPACITY];
    UmiTeacherLanguage language;
    UmiTeacherLevel level;
    uint32_t weight;
    uint32_t required_score;
    uint64_t revision;
    int enabled;
} UmiTeacherAssemblyTrack;

void umi_teacher_assembly_track_init(UmiTeacherAssemblyTrack *value);
UmiStatus umi_teacher_assembly_track_configure(UmiTeacherAssemblyTrack *value, const char *id, const char *title, UmiTeacherLanguage language, UmiTeacherLevel level, uint32_t weight, uint32_t required_score);
UmiStatus umi_teacher_assembly_track_validate(const UmiTeacherAssemblyTrack *value);
uint32_t umi_teacher_assembly_track_priority(const UmiTeacherAssemblyTrack *value, uint32_t relevance);

#ifdef __cplusplus
}
#endif

#endif
