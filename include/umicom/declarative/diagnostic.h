/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/declarative/diagnostic.h
 *
 * PURPOSE:
 *   Collect bounded parse, validation and compilation diagnostics with source line information.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The declarations below describe semantic application data and behaviour.
 * They deliberately avoid GUI-toolkit types so the same contract can be used
 * by GTK4, web, headless tests and future frontend adapters.
 */

#ifndef UMICOM_DECLARATIVE_DIAGNOSTIC_H
#define UMICOM_DECLARATIVE_DIAGNOSTIC_H

#include "umicom/declarative/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDeclDiagnostic {
    UmiDeclDiagnosticSeverity severity;
    size_t line;
    size_t column;
    char code[UMI_DECL_ID_CAPACITY];
    char message[UMI_DECL_TEXT_CAPACITY];
} UmiDeclDiagnostic;

typedef struct UmiDeclDiagnosticList {
    UmiDeclDiagnostic items[UMI_DECL_MAX_DIAGNOSTICS];
    size_t count;
} UmiDeclDiagnosticList;

void umi_decl_diagnostics_clear(UmiDeclDiagnosticList *list);
UmiStatus umi_decl_diagnostics_add(UmiDeclDiagnosticList *list, UmiDeclDiagnosticSeverity severity, size_t line, size_t column, const char *code, const char *message);
size_t umi_decl_diagnostics_error_count(const UmiDeclDiagnosticList *list);

#ifdef __cplusplus
}
#endif

#endif
