/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/compiler_diagnostic_mapping.h
 *
 * PURPOSE:
 *   Map compiler diagnostic evidence to learner-facing categories.
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
#ifndef UMICOM_TEACHER_COMPILER_DIAGNOSTIC_MAPPING_H
#define UMICOM_TEACHER_COMPILER_DIAGNOSTIC_MAPPING_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/teacher/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the teacher compiler diagnostic mapping data shared with callers of this
 * public contract.
 */
typedef struct UmiTeacherCompilerDiagnosticMapping { uint32_t error_count; uint32_t warning_count; uint32_t note_count; uint32_t syntax_count; uint32_t type_count; uint32_t linker_count; } UmiTeacherCompilerDiagnosticMapping;
/**
 * Initialise teacher compiler diagnostic mapping from caller-provided values so later
 * operations receive a known state.
 */
void umi_teacher_compiler_diagnostic_mapping_init(UmiTeacherCompilerDiagnosticMapping *mapping);
/**
 * Provide the teacher compiler diagnostic mapping record operation used by this module and
 * its client applications.
 */
void umi_teacher_compiler_diagnostic_mapping_record(UmiTeacherCompilerDiagnosticMapping *mapping, int severity, int category);
/**
 * Provide the teacher compiler diagnostic mapping learning penalty operation used by this
 * module and its client applications.
 */
uint32_t umi_teacher_compiler_diagnostic_mapping_learning_penalty(const UmiTeacherCompilerDiagnosticMapping *mapping);
/**
 * Provide the teacher compiler diagnostic mapping category text operation used by this
 * module and its client applications.
 */
const char *umi_teacher_compiler_diagnostic_mapping_category_text(int category);

#ifdef __cplusplus
}
#endif

#endif
