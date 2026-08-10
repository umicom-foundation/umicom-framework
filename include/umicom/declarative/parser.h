/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/declarative/parser.h
 *
 * PURPOSE:
 *   Parse the line-oriented .umiapp format into an authoritative semantic document and diagnostics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The declarations below describe semantic application data and behaviour.
 * They deliberately avoid GUI-toolkit types so the same contract can be used
 * by GTK4, web, headless tests and future frontend adapters.
 */

#ifndef UMICOM_DECLARATIVE_PARSER_H
#define UMICOM_DECLARATIVE_PARSER_H

#include "umicom/declarative/diagnostic.h"
#include "umicom/declarative/document.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_decl_parse_text(const char *text, UmiDeclDocument **out_document, UmiDeclDiagnosticList *diagnostics);
UmiStatus umi_decl_parse_file(const char *path, UmiDeclDocument **out_document, UmiDeclDiagnosticList *diagnostics);

#ifdef __cplusplus
}
#endif

#endif
