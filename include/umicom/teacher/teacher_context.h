/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/teacher_context.h
 *
 * PURPOSE:
 *   Collect bounded evidence identifiers for AI Teacher prompts.
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
#ifndef UMICOM_TEACHER_TEACHER_CONTEXT_H
#define UMICOM_TEACHER_TEACHER_CONTEXT_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/teacher/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTeacherTeacherContext {
    char ids[UMI_TEACHER_MEDIUM_CAPACITY][UMI_TEACHER_ID_CAPACITY];
    size_t count;
    size_t cursor;
    uint64_t revision;
} UmiTeacherTeacherContext;

void umi_teacher_teacher_context_init(UmiTeacherTeacherContext *sequence);
UmiStatus umi_teacher_teacher_context_append(UmiTeacherTeacherContext *sequence, const char *id);
const char *umi_teacher_teacher_context_current(const UmiTeacherTeacherContext *sequence);
UmiStatus umi_teacher_teacher_context_next(UmiTeacherTeacherContext *sequence);
UmiStatus umi_teacher_teacher_context_previous(UmiTeacherTeacherContext *sequence);
size_t umi_teacher_teacher_context_count(const UmiTeacherTeacherContext *sequence);

#ifdef __cplusplus
}
#endif

#endif
