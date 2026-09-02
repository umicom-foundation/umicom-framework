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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * Represent the teacher teacher context data shared with callers of this public contract.
 */
typedef struct UmiTeacherTeacherContext {
    char ids[UMI_TEACHER_MEDIUM_CAPACITY][UMI_TEACHER_ID_CAPACITY];
    size_t count;
    size_t cursor;
    uint64_t revision;
} UmiTeacherTeacherContext;

/**
 * Initialise teacher teacher context from caller-provided values so later operations
 * receive a known state.
 */
void umi_teacher_teacher_context_init(UmiTeacherTeacherContext *sequence);
/**
 * Add teacher teacher context only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_teacher_teacher_context_append(UmiTeacherTeacherContext *sequence, const char *id);
/**
 * Provide the teacher teacher context current operation used by this module and its client
 * applications.
 */
const char *umi_teacher_teacher_context_current(const UmiTeacherTeacherContext *sequence);
/**
 * Provide the teacher teacher context next operation used by this module and its client
 * applications.
 */
UmiStatus umi_teacher_teacher_context_next(UmiTeacherTeacherContext *sequence);
/**
 * Provide the teacher teacher context previous operation used by this module and its
 * client applications.
 */
UmiStatus umi_teacher_teacher_context_previous(UmiTeacherTeacherContext *sequence);
/**
 * Return the number of records represented by teacher teacher context without changing
 * their state.
 */
size_t umi_teacher_teacher_context_count(const UmiTeacherTeacherContext *sequence);

#ifdef __cplusplus
}
#endif

#endif
