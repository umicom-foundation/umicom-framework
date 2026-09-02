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

/*
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

/**
 * Represent the decl diagnostic data shared with callers of this public contract.
 */
typedef struct UmiDeclDiagnostic {
    UmiDeclDiagnosticSeverity severity;
    size_t line;
    size_t column;
    char code[UMI_DECL_ID_CAPACITY];
    char message[UMI_DECL_TEXT_CAPACITY];
} UmiDeclDiagnostic;

/**
 * Represent the decl diagnostic list data shared with callers of this public contract.
 */
typedef struct UmiDeclDiagnosticList {
    UmiDeclDiagnostic items[UMI_DECL_MAX_DIAGNOSTICS];
    size_t count;
} UmiDeclDiagnosticList;

/**
 * Release or reset state held by decl diagnostics so the same storage can be reused
 * safely.
 */
void umi_decl_diagnostics_clear(UmiDeclDiagnosticList *list);
/**
 * Add decl diagnostics only after its inputs and available capacity have been checked.
 */
UmiStatus umi_decl_diagnostics_add(UmiDeclDiagnosticList *list, UmiDeclDiagnosticSeverity severity, size_t line, size_t column, const char *code, const char *message);
/**
 * Return the number of records represented by decl diagnostics error without changing
 * their state.
 */
size_t umi_decl_diagnostics_error_count(const UmiDeclDiagnosticList *list);

#ifdef __cplusplus
}
#endif

#endif
