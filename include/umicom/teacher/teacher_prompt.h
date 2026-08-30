/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/teacher_prompt.h
 *
 * PURPOSE:
 *   Describe a governed AI Teacher prompt request.
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
#ifndef UMICOM_TEACHER_TEACHER_PROMPT_H
#define UMICOM_TEACHER_TEACHER_PROMPT_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/teacher/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTeacherTeacherPrompt {
    char id[UMI_TEACHER_ID_CAPACITY];
    char title[UMI_TEACHER_TEXT_CAPACITY];
    UmiTeacherLanguage language;
    UmiTeacherLevel level;
    uint32_t weight;
    uint32_t required_score;
    uint64_t revision;
    int enabled;
} UmiTeacherTeacherPrompt;

void umi_teacher_teacher_prompt_init(UmiTeacherTeacherPrompt *value);
UmiStatus umi_teacher_teacher_prompt_configure(UmiTeacherTeacherPrompt *value, const char *id, const char *title, UmiTeacherLanguage language, UmiTeacherLevel level, uint32_t weight, uint32_t required_score);
UmiStatus umi_teacher_teacher_prompt_validate(const UmiTeacherTeacherPrompt *value);
uint32_t umi_teacher_teacher_prompt_priority(const UmiTeacherTeacherPrompt *value, uint32_t relevance);

#ifdef __cplusplus
}
#endif

#endif
