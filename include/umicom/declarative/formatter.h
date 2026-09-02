/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/declarative/formatter.h
 *
 * PURPOSE:
 *   Provide canonical formatting for declarative source by parsing and serialising one semantic document.
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

#ifndef UMICOM_DECLARATIVE_FORMATTER_H
#define UMICOM_DECLARATIVE_FORMATTER_H

#include "umicom/declarative/parser.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the decl format text operation used by this module and its client applications.
 */
UmiStatus umi_decl_format_text(const char *source, char *out_text, size_t capacity, UmiDeclDiagnosticList *diagnostics);

#ifdef __cplusplus
}
#endif

#endif
