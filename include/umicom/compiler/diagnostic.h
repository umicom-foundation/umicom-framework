/* Umicom Framework | Compiler diagnostics | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_COMPILER_DIAGNOSTIC_H
#define UMICOM_COMPILER_DIAGNOSTIC_H
#include "umicom/compiler/common.h"
#define UMI_COMPILER_MAX_DIAGNOSTICS 256U
typedef struct UmiCompilerDiagnostic {
    char file[UMI_COMPILER_PATH_CAPACITY];
    uint32_t line;
    uint32_t column;
    UmiCompilerDiagnosticSeverity severity;
    char code[64U];
    char message[UMI_COMPILER_TEXT_CAPACITY];
} UmiCompilerDiagnostic;
typedef struct UmiCompilerDiagnosticSet { UmiCompilerDiagnostic items[UMI_COMPILER_MAX_DIAGNOSTICS]; size_t count; size_t errors; size_t warnings; uint64_t revision; } UmiCompilerDiagnosticSet;
UmiStatus umi_compiler_diagnostic_parse_line(const char *text,UmiCompilerDiagnostic *out_diagnostic);
UmiStatus umi_compiler_diagnostic_set_add(UmiCompilerDiagnosticSet *set,const UmiCompilerDiagnostic *diagnostic);
const UmiCompilerDiagnostic *umi_compiler_diagnostic_set_at(const UmiCompilerDiagnosticSet *set,size_t index);
#endif
