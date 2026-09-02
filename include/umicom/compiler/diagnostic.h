/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/diagnostic.h
 *
 * PURPOSE:
 *   Publish the public diagnostic contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_DIAGNOSTIC_H
#define UMICOM_COMPILER_DIAGNOSTIC_H
#include "umicom/compiler/common.h"
#define UMI_COMPILER_MAX_DIAGNOSTICS 256U
/**
 * Represent the compiler diagnostic data shared with callers of this public contract.
 */
typedef struct UmiCompilerDiagnostic {
    char file[UMI_COMPILER_PATH_CAPACITY];
    uint32_t line;
    uint32_t column;
    UmiCompilerDiagnosticSeverity severity;
    char code[64U];
    char message[UMI_COMPILER_TEXT_CAPACITY];
} UmiCompilerDiagnostic;
/**
 * Represent the compiler diagnostic set data shared with callers of this public contract.
 */
typedef struct UmiCompilerDiagnosticSet { UmiCompilerDiagnostic items[UMI_COMPILER_MAX_DIAGNOSTICS]; size_t count; size_t errors; size_t warnings; uint64_t revision; } UmiCompilerDiagnosticSet;
/**
 * Provide the compiler diagnostic parse line operation used by this module and its client
 * applications.
 */
UmiStatus umi_compiler_diagnostic_parse_line(const char *text,UmiCompilerDiagnostic *out_diagnostic);
/**
 * Add compiler diagnostic set only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_compiler_diagnostic_set_add(UmiCompilerDiagnosticSet *set,const UmiCompilerDiagnostic *diagnostic);
/**
 * Find compiler diagnostic set while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiCompilerDiagnostic *umi_compiler_diagnostic_set_at(const UmiCompilerDiagnosticSet *set,size_t index);
#endif
