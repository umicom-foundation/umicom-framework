/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/compiler_result.h
 *
 * PURPOSE:
 *   Track compilation outcome evidence without embedding compiler ownership in Teacher.
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
#ifndef UMICOM_TEACHER_COMPILER_RESULT_H
#define UMICOM_TEACHER_COMPILER_RESULT_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/teacher/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTeacherCompilerResult {
    UmiTeacherState state;
    uint32_t score;
    uint32_t attempts;
    uint32_t completed_units;
    uint32_t total_units;
    uint64_t revision;
} UmiTeacherCompilerResult;

void umi_teacher_compiler_result_init(UmiTeacherCompilerResult *value);
UmiStatus umi_teacher_compiler_result_record(UmiTeacherCompilerResult *value, uint32_t score, int passed);
void umi_teacher_compiler_result_set_units(UmiTeacherCompilerResult *value, uint32_t completed_units, uint32_t total_units);
uint32_t umi_teacher_compiler_result_completion(const UmiTeacherCompilerResult *value);
int umi_teacher_compiler_result_complete(const UmiTeacherCompilerResult *value);

#ifdef __cplusplus
}
#endif

#endif
